#include "displayapp/LittleVgl.h"
#if defined(INFINITIME_THEME_CPP)
#include "displayapp/InfiniTimeTheme.h"
#else
#include "displayapp/lv_pinetime_theme.h"
#endif

#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
////#include <projdefs.h>
#include "drivers/Cst816s.h"
#include "drivers/St7789.h"

// lv-sim monitor display driver for monitor_flush() function
#include "lv_drivers/display/monitor.h"

#include <array>

using namespace Pinetime::Components;

lv_style_t* LabelBigStyle = nullptr;

static void disp_flush(lv_disp_drv_t* disp_drv, const lv_area_t* area, lv_color_t* color_p) {
  auto* lvgl = static_cast<LittleVgl*>(disp_drv->user_data);
  lvgl->FlushDisplay(area, color_p);
}

static void rounder(lv_disp_drv_t* disp_drv, lv_area_t* area) {
  auto* lvgl = static_cast<LittleVgl*>(disp_drv->user_data);
  if (lvgl->GetFullRefresh()) {
    area->x1 = 0;
    area->x2 = LV_HOR_RES - 1;
    area->y1 = 0;
    area->y2 = LV_VER_RES - 1;
  }
}

bool touchpad_read(lv_indev_drv_t* indev_drv, lv_indev_data_t* data) {
  auto* lvgl = static_cast<LittleVgl*>(indev_drv->user_data);
  return lvgl->GetTouchPadInfo(data);
}

LittleVgl::LittleVgl(Pinetime::Drivers::St7789& lcd, Pinetime::Drivers::Cst816S& touchPanel)
  : lcd {lcd}, touchPanel {touchPanel} {
}

void LittleVgl::Init() {
//  lv_init();
  InitTheme();
  InitDisplay();
  InitTouchpad();
}

void LittleVgl::InitDisplay() {
  lv_disp_buf_init(&disp_buf_2, buf2_1, buf2_2, LV_HOR_RES_MAX * 4); /*Initialize the display buffer*/
  lv_disp_drv_init(&disp_drv);                                       /*Basic initialization*/

  /*Set up the functions to access to your display*/

  /*Set the resolution of the display*/
  disp_drv.hor_res = 240;
  disp_drv.ver_res = 240;

  /*Used to copy the buffer's content to the display*/
  disp_drv.flush_cb = disp_flush;
  /*Set a display buffer*/
  disp_drv.buffer = &disp_buf_2;
  disp_drv.user_data = this;
  disp_drv.rounder_cb = rounder;

  /*Finally register the driver*/
  lv_disp_drv_register(&disp_drv);
}

void LittleVgl::InitTouchpad() {
  lv_indev_drv_t indev_drv;

  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = touchpad_read;
  indev_drv.user_data = this;
  lv_indev_drv_register(&indev_drv);
}

void LittleVgl::SetFullRefresh(FullRefreshDirections direction) {
  if (scrollDirection == FullRefreshDirections::None) {
    scrollDirection = direction;
    if (scrollDirection == FullRefreshDirections::Down) {
      lv_disp_set_direction(lv_disp_get_default(), 1);
    } else if (scrollDirection == FullRefreshDirections::Right) {
      lv_disp_set_direction(lv_disp_get_default(), 2);
    } else if (scrollDirection == FullRefreshDirections::Left) {
      lv_disp_set_direction(lv_disp_get_default(), 3);
    } else if (scrollDirection == FullRefreshDirections::RightAnim) {
      lv_disp_set_direction(lv_disp_get_default(), 5);
    } else if (scrollDirection == FullRefreshDirections::LeftAnim) {
      lv_disp_set_direction(lv_disp_get_default(), 4);
    }
  }
  fullRefresh = true;
}

// glue the lvgl code to the lv-sim monitor driver
void DrawBuffer(lv_disp_drv_t *disp_drv, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t* data, size_t size) {
  lv_area_t area;
  area.x1 = x;
  area.x2 = x+width-1;
  area.y1 = y;
  area.y2 = y+height-1;
  lv_color_t* color_p = reinterpret_cast<lv_color_t*>(data);
  monitor_flush(disp_drv, &area, color_p);
}

// copied from lv_drivers/display/monitor.c to get the SDL_Window for the InfiniTime screen
extern "C"
{
typedef struct {
  SDL_Window * window;
  SDL_Renderer * renderer;
  SDL_Texture * texture;
  volatile bool sdl_refr_qry;
#if MONITOR_DOUBLE_BUFFERED
  uint32_t * tft_fb_act;
#else
  uint32_t tft_fb[LV_HOR_RES_MAX * LV_VER_RES_MAX];
#endif
}monitor_t;
extern monitor_t monitor;
}

// positive height moves screen down (draw y=0 to y=height)
// negative height moves screen up   (draw y=height to y=0)
void MoveScreen(lv_disp_drv_t *disp_drv, int16_t height) {
  if (height == 0)
    return; // nothing to do

  const int sdl_width = 240;
  const int sdl_height = 240;
  auto renderer = monitor.renderer;

  const Uint32 format = SDL_PIXELFORMAT_RGBA8888;
  SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, sdl_width, sdl_height, 32, format);
  SDL_RenderReadPixels(renderer, NULL, format, surface->pixels, surface->pitch);
  uint8_t *pixels = (uint8_t*) surface->pixels;

  std::array<lv_color16_t, 240*240> color_p;
  for (int hi = 0; hi < sdl_height; hi++) {
    for (int wi = 0; wi < sdl_width; wi++) {
      auto red   = pixels[hi*surface->pitch + wi*4 + 3]; // red
      auto green = pixels[hi*surface->pitch + wi*4 + 2]; // greeen
      auto blue  = pixels[hi*surface->pitch + wi*4 + 1]; // blue
      color_p.at(hi * sdl_width + wi) = LV_COLOR_MAKE(red, green, blue);
    }
  }
  int16_t buffer_height = sdl_height - abs(height);
  if (height >= 0) {
    DrawBuffer(disp_drv, 0, height, sdl_width, sdl_height, (uint8_t*)color_p.data(), sdl_width*buffer_height *2);
  } else {
    DrawBuffer(disp_drv, 0, 0, sdl_width, sdl_height, (uint8_t*)(&color_p.at(sdl_width*abs(height))), sdl_width*buffer_height *2);
  }
}

void LittleVgl::FlushDisplay(const lv_area_t* area, lv_color_t* color_p) {
  uint16_t y1, y2, width, height = 0;

  //ulTaskNotifyTake(pdTRUE, 200);
  // Notification is still needed (even if there is a mutex on SPI) because of the DataCommand pin
  // which cannot be set/clear during a transfer.

  //if ((scrollDirection == LittleVgl::FullRefreshDirections::Down) && (area->y2 == visibleNbLines - 1)) {
  //  writeOffset = ((writeOffset + totalNbLines) - visibleNbLines) % totalNbLines;
  //} else if ((scrollDirection == FullRefreshDirections::Up) && (area->y1 == 0)) {
  //  writeOffset = (writeOffset + visibleNbLines) % totalNbLines;
  //}

  y1 = (area->y1 + writeOffset) % totalNbLines;
  y2 = (area->y2 + writeOffset) % totalNbLines;

  width = (area->x2 - area->x1) + 1;
  height = (area->y2 - area->y1) + 1;

  if (scrollDirection == LittleVgl::FullRefreshDirections::Down) {

    if (area->y2 < visibleNbLines - 1) {
      uint16_t toScroll = 0;
      if (area->y1 == 0) {
        toScroll = height * 2;
        scrollDirection = FullRefreshDirections::None;
        lv_disp_set_direction(lv_disp_get_default(), 0);
      } else {
        toScroll = height;
      }

      if (scrollOffset >= toScroll)
        scrollOffset -= toScroll;
      else {
        toScroll -= scrollOffset;
        scrollOffset = (totalNbLines) -toScroll;
      }
      lcd.VerticalScrollStartAddress(scrollOffset);

    }
    // move the whole screen down and draw the new screen at the top of the display
    MoveScreen(&disp_drv, static_cast<int16_t>(height));
    y1 = 0;
    y2 = height;

  } else if (scrollDirection == FullRefreshDirections::Up) {

    if (area->y1 > 0) {
      if (area->y2 == visibleNbLines - 1) {
        scrollOffset += (height * 2);
        scrollDirection = FullRefreshDirections::None;
        lv_disp_set_direction(lv_disp_get_default(), 0);
      } else {
        scrollOffset += height;
      }
      scrollOffset = scrollOffset % totalNbLines;
      lcd.VerticalScrollStartAddress(scrollOffset);
    }
    // move the whole screen up and draw the new screen at the bottom the display
    MoveScreen(&disp_drv, -static_cast<int16_t>(height));
    y1 = LV_VER_RES - height;
    y2 = LV_VER_RES;
  } else if (scrollDirection == FullRefreshDirections::Left or scrollDirection == FullRefreshDirections::LeftAnim) {
    if (area->x2 == visibleNbLines - 1) {
      scrollDirection = FullRefreshDirections::None;
      lv_disp_set_direction(lv_disp_get_default(), 0);
    }
  } else if (scrollDirection == FullRefreshDirections::Right or scrollDirection == FullRefreshDirections::RightAnim) {
    if (area->x1 == 0) {
      scrollDirection = FullRefreshDirections::None;
      lv_disp_set_direction(lv_disp_get_default(), 0);
    }
  }

  if (y2 < y1) {
    height = totalNbLines - y1;

    if (height > 0) {
      //lcd.DrawBuffer(area->x1, y1, width, height, reinterpret_cast<const uint8_t*>(color_p), width * height * 2);
      DrawBuffer(&disp_drv, area->x1, y1, width, height, reinterpret_cast<uint8_t*>(color_p), width * height * 2);
      //ulTaskNotifyTake(pdTRUE, 100);
    }

    uint16_t pixOffset = width * height;
    height = y2 + 1;
    //lcd.DrawBuffer(area->x1, 0, width, height, reinterpret_cast<const uint8_t*>(color_p + pixOffset), width * height * 2);
    DrawBuffer(&disp_drv, area->x1, 0, width, height, reinterpret_cast<uint8_t*>(color_p + pixOffset), width * height * 2);

  } else {
    //lcd.DrawBuffer(area->x1, y1, width, height, reinterpret_cast<const uint8_t*>(color_p), width * height * 2);
    DrawBuffer(&disp_drv, area->x1, y1, width, height, reinterpret_cast<uint8_t*>(color_p), width * height * 2);
  }

  // IMPORTANT!!!
  // Inform the graphics library that you are ready with the flushing
  //lv_disp_flush_ready(&disp_drv);

  // call flush with flushing_last set
  // workaround because lv_disp_flush_ready() doesn't seem to trigger monitor_flush
  lv_disp_t *disp = lv_disp_get_default();
  lv_disp_get_buf(disp)->flushing_last = true;
  lv_area_t area_zero {0,0,0,0};
  monitor_flush(&disp_drv, &area_zero, color_p);
  // delay drawing to mimic PineTime display rendering speed
  vTaskDelay(pdMS_TO_TICKS(3));
}

void LittleVgl::SetNewTouchPoint(uint16_t x, uint16_t y, bool contact) {
  tap_x = x;
  tap_y = y;
  tapped = contact;
}

bool LittleVgl::GetTouchPadInfo(lv_indev_data_t* ptr) {
  ptr->point.x = tap_x;
  ptr->point.y = tap_y;
  if (tapped) {
    ptr->state = LV_INDEV_STATE_PR;
  } else {
    ptr->state = LV_INDEV_STATE_REL;
  }
  return false;
}

void LittleVgl::InitTheme() {

  lv_theme_t* th = lv_pinetime_theme_init(LV_COLOR_WHITE,
                                          LV_COLOR_SILVER,
                                          0,
                                          &jetbrains_mono_bold_20,
                                          &jetbrains_mono_bold_20,
                                          &jetbrains_mono_bold_20,
                                          &jetbrains_mono_bold_20);

  lv_theme_set_act(th);
}
