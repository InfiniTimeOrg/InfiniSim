
/**
 * @file main
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define _DEFAULT_SOURCE /* needed for usleep() */
#include <stdlib.h>
#include <unistd.h>
#define SDL_MAIN_HANDLED /*To fix SDL's "undefined reference to WinMain" issue*/
#include <SDL2/SDL.h>
#include "lvgl/lvgl.h"
//#include "lvgl/examples/lv_examples.h"
//#include "lv_demos/lv_demo.h"
#include "lv_drivers/display/monitor.h"
#include "lv_drivers/indev/mouse.h"
#include "lv_drivers/indev/keyboard.h"
#include "lv_drivers/indev/mousewheel.h"

// get PineTime header
#if defined(INFINITIME_THEME_CPP)
#include "displayapp/InfiniTimeTheme.h"
#else
#include "displayapp/lv_pinetime_theme.h"
#endif
#include "sim/drivers/infinisim/Bma421.h"
#include <drivers/Hrs3300.h>

#include "BootloaderVersion.h"
#include "buttonhandler/ButtonHandler.h"
#include "components/battery/BatteryController.h"
#include "components/ble/BleController.h"
#include "components/ble/NotificationManager.h"
#include "components/brightness/BrightnessController.h"
#include "components/datetime/DateTimeController.h"
#include "components/fs/FS.h"
#include "components/heartrate/HeartRateController.h"
#include "components/motor/MotorController.h"
#include "drivers/PinMap.h"
#include "drivers/Spi.h"
#include "drivers/St7789.h"
#include "sim/drivers/infinisim/SdlTouchPanel.h"
#include "sim/drivers/infinisim/SpiMaster.h"
#include "sim/drivers/infinisim/SpiNorFlash.h"
#include "sim/drivers/infinisim/TwiMaster.h"
#include "systemtask/SystemTask.h"
#include "touchhandler/TouchHandler.h"

// get the simulator-headers
#include "displayapp/DisplayApp.h"
#include "displayapp/LittleVgl.h"

#include <nrfx_gpiote.h>
#include <hal/nrf_gpio.h>

#include <iostream>
#include <typeinfo>
#include <algorithm>
#include <cmath> // std::pow

// additional includes for 'saveScreenshot()' function
#include <date/date.h>
#include <chrono>
#if defined(WITH_PNG)
#include <libpng/png.h>
#endif
#include <gif.h>

#include "port/Spi.h"
#include "port/SpiMaster.h"
#include "port/SpiNorFlash.h"
#include "port/TwiMaster.h"
#include "port/TouchPanel.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
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

void saveScreenshot()
{
  auto now = std::chrono::system_clock::now();
  // TODO: timestamped png filename
  std::string screenshot_filename_base = date::format("InfiniSim_%F_%H%M%S", date::floor<std::chrono::seconds>(now));
  //std::string screenshot_filename_base = "InfiniSim";

  const int width = 240;
  const int height = 240;
  auto renderer = monitor.renderer;


#if defined(WITH_PNG)
  std::string screenshot_filename = screenshot_filename_base + ".png";

  FILE * fp2 = fopen(screenshot_filename.c_str(), "wb");
  if (!fp2) {
    // dealing with error
    return;
  }
  // 1. Create png struct pointer
  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr){
      // dealing with error
  }
  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
      png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
      // dealing with error
  }
  int bit_depth = 8;
  png_init_io(png_ptr, fp2);
  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, \
      PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, \
      PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

  // 3. Convert 1d array to 2d array to be suitable for png struct
  //    I assumed the original array is 1d
  std::array<png_bytep, 240> row_pointers;
  //png_bytepp row_pointers = (png_bytepp)png_malloc(png_ptr, sizeof(png_bytep) * height);
  for (int i = 0; i < height; i++) {
      row_pointers[i] = (png_bytep)png_malloc(png_ptr, width*4);
  }
  const Uint32 format = SDL_PIXELFORMAT_RGBA8888;
  SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, format);
  SDL_RenderReadPixels(renderer, NULL, format, surface->pixels, surface->pitch);
  png_bytep pixels = (png_bytep)surface->pixels;
  for (int hi = 0; hi < height; hi++) {
    for (int wi = 0; wi < width; wi++) {
      int c = wi * 4;
      row_pointers.at(hi)[wi*4+0] = pixels[hi*surface->pitch + wi*4 + 3]; // red
      row_pointers.at(hi)[wi*4+1] = pixels[hi*surface->pitch + wi*4 + 2]; // greeen
      row_pointers.at(hi)[wi*4+2] = pixels[hi*surface->pitch + wi*4 + 1]; // blue
      row_pointers.at(hi)[wi*4+3] = 255; // alpha
    }
  }

  // 4. Write png file
  png_write_info(png_ptr, info_ptr);
  png_write_image(png_ptr, row_pointers.data());
  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp2);

  SDL_FreeSurface(surface);
#else
  std::string screenshot_filename = screenshot_filename_base + ".bmp";
  const Uint32 format = SDL_PIXELFORMAT_RGB888;
  SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 24, format);
  SDL_RenderReadPixels(renderer, NULL, format, surface->pixels, surface->pitch);
  SDL_SaveBMP(surface, screenshot_filename.c_str());
  SDL_FreeSurface(surface);
#endif
  std::cout << "InfiniSim: Screenshot created: " << screenshot_filename << std::endl;
}

class GifManager
{
private:
  GifWriter writer = {};
  std::chrono::system_clock::time_point last_frame;
  bool in_progress = false;
  static constexpr uint32_t delay_ds = 100/20; // in 1/100 s, so 1 ds = 10 ms
  static constexpr int sdl_width = 240;
  static constexpr int sdl_height = 240;

public:
  GifManager()
  {}
  ~GifManager()
  {
    if (in_progress) {
      close();
    }
  }

  bool is_in_progress() const
  {
    return in_progress;
  }
  void create_new()
  {
    assert(!in_progress);
    auto now = std::chrono::system_clock::now();
    std::string screenshot_filename_base = date::format("InfiniSim_%F_%H%M%S", date::floor<std::chrono::seconds>(now));
    std::string screenshot_filename = screenshot_filename_base + ".gif";
    std::cout << "InfiniSim: Screen-capture started: " << screenshot_filename << std::endl;
    GifBegin( &writer, screenshot_filename.c_str(), sdl_width, sdl_height, delay_ds, 8, true );
    in_progress = true;
    write_frame(true);
  }
  void write_frame(bool force = false)
  {
    assert(in_progress);
    auto now = std::chrono::system_clock::now();
    if (force || ((now - last_frame) > std::chrono::milliseconds(delay_ds*10)) )
    {
      last_frame = std::chrono::system_clock::now();
      auto renderer = monitor.renderer;
      const Uint32 format = SDL_PIXELFORMAT_RGBA8888;
      SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, sdl_width, sdl_height, 32, format);
      SDL_RenderReadPixels(renderer, NULL, format, surface->pixels, surface->pitch);
      uint8_t *pixels = (uint8_t*) surface->pixels;

      std::array<uint8_t, 240*240*4> image;
      for (int hi = 0; hi < sdl_height; hi++) {
        for (int wi = 0; wi < sdl_width; wi++) {
          auto red   = pixels[hi*surface->pitch + wi*4 + 3]; // red
          auto green = pixels[hi*surface->pitch + wi*4 + 2]; // green
          auto blue  = pixels[hi*surface->pitch + wi*4 + 1]; // blue
          image[(hi * sdl_width + wi)*4 + 0] = red;
          image[(hi * sdl_width + wi)*4 + 1] = green;
          image[(hi * sdl_width + wi)*4 + 2] = blue;
          image[(hi * sdl_width + wi)*4 + 3] = 255; // no alpha
        }
      }
      GifWriteFrame(&writer, image.data(), sdl_width, sdl_height, delay_ds, 8, true);
    }
  }
  void close()
  {
    assert(in_progress);
    in_progress = false;
    GifEnd(&writer);
    std::cout << "InfiniSim: Screen-capture finished" << std::endl;
  }
};

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void hal_init(void);
static int tick_thread(void *data);

/**********************
 *  STATIC VARIABLES
 **********************/
lv_indev_t *kb_indev;
lv_indev_t *mouse_indev = nullptr;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void nrfx_gpiote_evt_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {}

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      VARIABLES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
static constexpr uint8_t touchPanelTwiAddress = 0x15;
static constexpr uint8_t motionSensorTwiAddress = 0x18;
static constexpr uint8_t heartRateSensorTwiAddress = 0x44;

Pinetime::Drivers::Infinisim::SpiMaster spiImpl {};
Pinetime::Drivers::SpiMaster spi {spiImpl};


Pinetime::Drivers::Infinisim::Spi lcdSpiImpl {spiImpl, Pinetime::PinMap::SpiLcdCsn};
Pinetime::Drivers::Spi lcdSpi {lcdSpiImpl};
Pinetime::Drivers::St7789 lcd {lcdSpi, Pinetime::PinMap::LcdDataCommand};

Pinetime::Drivers::Infinisim::Spi flashSpiImpl {spiImpl, Pinetime::PinMap::SpiFlashCsn};
Pinetime::Drivers::Spi flashSpi {flashSpiImpl};

Pinetime::Drivers::Infinisim::SpiNorFlash spiNorFlashImpl {"spiNorFlash.raw"};
Pinetime::Drivers::SpiNorFlash spiNorFlash {spiNorFlashImpl};

Pinetime::Drivers::Infinisim::TwiMaster twiMasterImpl {};
Pinetime::Drivers::TwiMaster twiMaster{twiMasterImpl};

Pinetime::Drivers::Infinisim::TouchPanels::SdlTouchPanel touchPanelImpl;
Pinetime::Drivers::TouchPanel touchPanel {touchPanelImpl};

//#ifdef PINETIME_IS_RECOVERY
//  #include "displayapp/DummyLittleVgl.h"
//  #include "displayapp/DisplayAppRecovery.h"
//#else
//  #include "displayapp/LittleVgl.h"
//  #include "displayapp/DisplayApp.h"
//#endif
Pinetime::Components::LittleVgl lvgl {lcd, touchPanel};

Pinetime::Drivers::Infinisim::MotionSensors::Bma421 motionSensorImpl{};
Pinetime::Drivers::MotionSensor motionSensor{motionSensorImpl};
Pinetime::Drivers::Hrs3300 heartRateSensor {twiMaster, heartRateSensorTwiAddress};

TimerHandle_t debounceTimer;
TimerHandle_t debounceChargeTimer;
Pinetime::Controllers::Battery batteryController;
Pinetime::Controllers::Ble bleController;

Pinetime::Controllers::HeartRateController heartRateController;
Pinetime::Applications::HeartRateTask heartRateApp(heartRateSensor, heartRateController);

Pinetime::Controllers::FS fs {spiNorFlash};
Pinetime::Controllers::Settings settingsController {fs};
Pinetime::Controllers::MotorController motorController {};

Pinetime::Controllers::DateTime dateTimeController {settingsController};
Pinetime::Drivers::Watchdog watchdog;
Pinetime::Drivers::WatchdogView watchdogView(watchdog);
Pinetime::Controllers::NotificationManager notificationManager;
Pinetime::Controllers::MotionController motionController;
Pinetime::Controllers::TimerController timerController;
Pinetime::Controllers::AlarmController alarmController {dateTimeController};
Pinetime::Controllers::TouchHandler touchHandler(touchPanel, lvgl);
Pinetime::Controllers::ButtonHandler buttonHandler;
Pinetime::Controllers::BrightnessController brightnessController {};

Pinetime::Applications::DisplayApp displayApp(lcd,
                                              lvgl,
                                              touchPanel,
                                              batteryController,
                                              bleController,
                                              dateTimeController,
                                              watchdogView,
                                              notificationManager,
                                              heartRateController,
                                              settingsController,
                                              motorController,
                                              motionController,
                                              timerController,
                                              alarmController,
                                              brightnessController,
                                              touchHandler,
                                              fs);

Pinetime::System::SystemTask systemTask(spi,
                                        lcd,
                                        spiNorFlash,
                                        twiMaster,
                                        touchPanel,
                                        lvgl,
                                        batteryController,
                                        bleController,
                                        dateTimeController,
                                        timerController,
                                        alarmController,
                                        watchdog,
                                        notificationManager,
                                        motorController,
                                        heartRateSensor,
                                        motionController,
                                        motionSensor,
                                        settingsController,
                                        heartRateController,
                                        displayApp,
                                        heartRateApp,
                                        fs,
                                        touchHandler,
                                        buttonHandler);

// variable used in SystemTask.cpp Work loop
std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> NoInit_BackUpTime;

class Framework {
public:
    // Contructor which initialize the parameters.
    Framework(bool visible_, int height_, int width_) :
      visible(visible_), height(height_), width(width_)
      {
        if (visible) {
          //SDL_Init(SDL_INIT_VIDEO);       // Initializing SDL as Video
          SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
          SDL_SetWindowTitle(window, "LV Simulator Status");
          { // move window a bit to the right, to not be over the PineTime Screen
              int x,y;
              SDL_GetWindowPosition(window, &x, &y);
              SDL_SetWindowPosition(window, x+LV_HOR_RES_MAX, y);
          }
          SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);      // setting draw color
          SDL_RenderClear(renderer);      // Clear the newly created window
          SDL_RenderPresent(renderer);    // Reflects the changes done in the
                                          //  window.
        }
        motorController.Init();
        settingsController.Init();

        lvgl.Init();

        lv_mem_monitor(&mem_mon);
        printf("initial free_size = %u\n", mem_mon.free_size);

        // update time to current system time once on startup
        dateTimeController.SetCurrentTime(std::chrono::system_clock::now());

        systemTask.Start();

        // initialize the first LVGL screen
        //const auto clockface = settingsController.GetClockFace();
        //switch_to_screen(1+clockface);
    }

    // Destructor
    ~Framework(){
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void draw_circle_red(int center_x, int center_y, int radius_){
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        draw_circle_(center_x, center_y, radius_);
    }
    void draw_circle_green(int center_x, int center_y, int radius_){
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        draw_circle_(center_x, center_y, radius_);
    }
    void draw_circle_blue(int center_x, int center_y, int radius_){
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        draw_circle_(center_x, center_y, radius_);
    }
    void draw_circle_yellow(int center_x, int center_y, int radius_){
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        draw_circle_(center_x, center_y, radius_);
    }
    void draw_circle_grey(int center_x, int center_y, int radius_){
        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
        draw_circle_(center_x, center_y, radius_);
    }
    void draw_circle_white(int center_x, int center_y, int radius_){
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        draw_circle_(center_x, center_y, radius_);
    }
    void draw_circle_(int center_x, int center_y, int radius_){
        // Drawing circle
        for(int x=center_x-radius_; x<=center_x+radius_; x++){
            for(int y=center_y-radius_; y<=center_y+radius_; y++){
                if((std::pow(center_y-y,2)+std::pow(center_x-x,2)) <=
                    std::pow(radius_,2)){
                    SDL_RenderDrawPoint(renderer, x, y);
                }
            }
        }
    }

    void refresh() {
      // always refresh the LVGL screen
      this->refresh_screen();

      if (!visible) {
        return;
      }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        { // motorController.motor_is_running
          constexpr const int center_x = 45;
          constexpr const int center_y = 15;
          bool motor_is_running = nrf_gpio_pin_read(Pinetime::PinMap::Motor);
          if (motor_is_running) {
              draw_circle_red(center_x, center_y, 15);
          } else {
              draw_circle_grey(center_x, center_y, 15);
          }
        }
        { // ble.motor_is_running
          constexpr const int center_x = 75;
          constexpr const int center_y = 15;
          if (bleController.IsConnected()) {
              draw_circle_blue(center_x, center_y, 15);
          } else {
              draw_circle_grey(center_x, center_y, 15);
          }
        }
        // batteryController.percentRemaining
        for (uint8_t percent=0; percent<=10; percent++) {
          const int center_x = 15+15*percent;
          const int center_y = 60;
          if (batteryController.percentRemaining < percent*10) {
            draw_circle_grey(center_x, center_y, 15);
          } else {
            draw_circle_green(center_x, center_y, 15);
          }
        }
        { // batteryController.isCharging
          constexpr const int center_x = 15;
          constexpr const int center_y = 90;
          if (batteryController.isCharging) {
              draw_circle_yellow(center_x, center_y, 15);
          } else
          {
              draw_circle_grey(center_x, center_y, 15);
          }
        }
        { // brightnessController.Level
          constexpr const int center_y = 15;
          const Pinetime::Controllers::BrightnessController::Levels level = brightnessController.Level();
          uint8_t level_idx = 0;
          if (level == Pinetime::Controllers::BrightnessController::Levels::Low)
          {
            level_idx = 1;
          } else if (level == Pinetime::Controllers::BrightnessController::Levels::Medium)
          {
            level_idx = 2;
          } else if (level == Pinetime::Controllers::BrightnessController::Levels::High)
          {
            level_idx = 3;
          }
          for (uint8_t i=0; i<4; i++) {
            const int center_x = 115+15*i;
            if (i <= level_idx) {
              draw_circle_white(center_x, center_y, 15);
            } else {
              draw_circle_grey(center_x, center_y, 15);
            }
          }
        }
        // Show the change on the screen
        SDL_RenderPresent(renderer);
    }

    // prepared notficitions, one per message category
    const std::vector<std::string> notification_messages {
      "0category:\nUnknown",
      "Lorem ipsum\ndolor sit amet,\nconsectetur adipiscing elit,\n",
      "1SimpleAlert",
      "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.",
      "2Email:",
      "Vitae aliquet nec ullamcorper sit amet.",
      "3News:",
      "Id\naliquet\nrisus\nfeugiat\nin\nante\nmetus\ndictum\nat.",
      "4IncomingCall:",
      "Ut porttitor leo a diam sollicitudin.",
      "5MissedCall:",
      "Ultrices tincidunt arcu non sodales neque sodales ut etiam sit.",
      "6Sms:",
      "Pellentesque dignissim enim sit amet.",
      "7VoiceMail:",
      "Urna nec tincidunt praesent semper feugiat nibh sed pulvinar proin.",
      "8Schedule:",
      "Tellus id interdum velit laoreet id donec ultrices tincidunt.",
      "9HighProriotyAlert:",
      "Viverra maecenas accumsan lacus vel facilisis volutpat est velit egestas.",
      "10InstantMessage:",
      "Volutpat consequat mauris nunc congue.",
    };
    size_t notification_idx = 0; // which message to send next
    void send_notification() {
      Pinetime::Controllers::NotificationManager::Notification notif;
      const std::string &title   = notification_messages.at(notification_idx*2);
      const std::string &message = notification_messages.at(notification_idx*2+1);
      std::copy(title.begin(), title.end(), notif.message.data());
      notif.message[title.size()] = '\0'; // title and message is \0 separated
      std::copy(message.begin(), message.end(), notif.message.data()+title.size()+1);
      notif.message[title.size() + 1 + message.size()] = '\0'; // zero terminate the message
      notif.size = title.size() + 1 + message.size();
      notif.category = static_cast<Pinetime::Controllers::NotificationManager::Categories>(notification_idx % 11);
      notificationManager.Push(std::move(notif));
      // send next message the next time
      notification_idx++;
      if (notification_idx >= notification_messages.size()/2) {
        notification_idx = 0;
      }
      if (settingsController.GetNotificationStatus() == Pinetime::Controllers::Settings::Notification::On)
      {
        if (screen_off_created) {
          // wake up! (deletes screen_off label)
          systemTask.PushMessage(Pinetime::System::Messages::GoToRunning);
        }
        displayApp.PushMessage(Pinetime::Applications::Display::Messages::NewNotification);
      }
    }

    // can't use SDL_PollEvent, as those are fed to lvgl
    // implement a non-descructive key-pressed handler (as not consuming SDL_Events)
    void handle_keys() {
      const Uint8 *state = SDL_GetKeyboardState(NULL);
      const bool key_shift = state[SDL_SCANCODE_LSHIFT] || state[SDL_SCANCODE_RSHIFT];
      auto debounce = [&] (const char key_low, const char key_capital, const bool scancode, bool &key_handled) {
        if (scancode && !key_handled) {
          if (key_shift) {
            this->handle_key(key_capital);
          } else {
            this->handle_key(key_low);
          }
          key_handled = true;
        } else if (scancode && key_handled) {
          // ignore, already handled
        } else {
          key_handled = false;
        }
      };
      debounce('r', 'R', state[SDL_SCANCODE_R], key_handled_r);
      debounce('n', 'N', state[SDL_SCANCODE_N], key_handled_n);
      debounce('m', 'M', state[SDL_SCANCODE_M], key_handled_m);
      debounce('b', 'B', state[SDL_SCANCODE_B], key_handled_b);
      debounce('v', 'V', state[SDL_SCANCODE_V], key_handled_v);
      debounce('c', 'C', state[SDL_SCANCODE_C], key_handled_c);
      debounce('l', 'L', state[SDL_SCANCODE_L], key_handled_l);
      debounce('p', 'P', state[SDL_SCANCODE_P], key_handled_p);
      debounce('s', 'S', state[SDL_SCANCODE_S], key_handled_s);
      debounce('h', 'H', state[SDL_SCANCODE_H], key_handled_h);
      debounce('i', 'I', state[SDL_SCANCODE_I], key_handled_i);
      // screen switcher buttons
      debounce('1', '!'+1, state[SDL_SCANCODE_1], key_handled_1);
      debounce('2', '!'+2, state[SDL_SCANCODE_2], key_handled_2);
      debounce('3', '!'+3, state[SDL_SCANCODE_3], key_handled_3);
      debounce('4', '!'+4, state[SDL_SCANCODE_4], key_handled_4);
      debounce('5', '!'+5, state[SDL_SCANCODE_5], key_handled_5);
      debounce('6', '!'+6, state[SDL_SCANCODE_6], key_handled_6);
      debounce('7', '!'+7, state[SDL_SCANCODE_7], key_handled_7);
      debounce('8', '!'+8, state[SDL_SCANCODE_8], key_handled_8);
      debounce('9', '!'+9, state[SDL_SCANCODE_9], key_handled_9);
      debounce('0', '!'+0, state[SDL_SCANCODE_0], key_handled_0);
    }
    // modify the simulated controller depending on the pressed key
    void handle_key(SDL_Keycode key) {
      if (key == 'r') {
        motorController.StartRinging();
      } else if (key == 'R') {
        motorController.StopRinging();
      } else if (key == 'm') {
        motorController.RunForDuration(100);
      } else if (key == 'M') {
        motorController.RunForDuration(255);
      } else if (key == 'n') {
        send_notification();
      } else if (key == 'N') {
        notificationManager.ClearNewNotificationFlag();
      } else if (key == 'b') {
        bleController.Connect();
      } else if (key == 'B') {
        bleController.Disconnect();
      } else if (key == 'v') {
        if (batteryController.percentRemaining >= 90) {
          batteryController.percentRemaining = 100;
        } else {
          batteryController.percentRemaining += 10;
        }
      } else if (key == 'V') {
        if (batteryController.percentRemaining <= 10) {
          batteryController.percentRemaining = 0;
        } else {
          batteryController.percentRemaining -= 10;
        }
      } else if (key == 'c') {
        batteryController.isCharging = true;
        batteryController.isPowerPresent = true;
      } else if (key == 'C') {
        batteryController.isCharging = false;
        batteryController.isPowerPresent = false;
      } else if (key == 'l' && !screen_off_created) {
        brightnessController.Higher();
      } else if (key == 'L' && !screen_off_created) {
        brightnessController.Lower();
      } else if (key == 'p') {
        this->print_memory_usage = true;
      } else if (key == 'P') {
        this->print_memory_usage = false;
      } else if (key == 's') {
        motionSensorImpl.steps += 500;
      } else if (key == 'S') {
        if (motionSensorImpl.steps > 500) {
          motionSensorImpl.steps -= 500;
        } else {
          motionSensorImpl.steps = 0;
        }
      } else if (key == 'h') {
        if (heartRateController.State() == Pinetime::Controllers::HeartRateController::States::Stopped) {
          heartRateController.Start();
        } else if (heartRateController.State() == Pinetime::Controllers::HeartRateController::States::NotEnoughData) {
          heartRateController.Update(Pinetime::Controllers::HeartRateController::States::Running, 10);
        } else {
          uint8_t heartrate = heartRateController.HeartRate();
          heartRateController.Update(Pinetime::Controllers::HeartRateController::States::Running, heartrate + 10);
        }
      } else if (key == 'H') {
        heartRateController.Stop();
      } else if (key == 'i') {
        saveScreenshot();
      } else if (key == 'I') {
        if (!gif_manager.is_in_progress())
        {
          gif_manager.create_new();
        } else {
          gif_manager.close();
        }
      } else if (key >= '0' && key <= '9') {
        this->switch_to_screen(key-'0');
      } else if (key >= '!'+0 && key <= '!'+9) {
        this->switch_to_screen(key-'!'+10);
      }
      batteryController.voltage = batteryController.percentRemaining * 50;
    }

    void handle_touch_and_button() {
      int x, y;
      uint32_t buttons = SDL_GetMouseState(&x, &y);
      const bool left_click = (buttons & SDL_BUTTON_LMASK) != 0;
      const bool right_click = (buttons & SDL_BUTTON_RMASK) != 0;
      if (left_click) {
        left_release_sent = false;
        systemTask.OnTouchEvent();
        return;
      } else {
        if (!left_release_sent) {
          left_release_sent = true;
          systemTask.OnTouchEvent();
          return;
        }
      }
      if (right_click != right_last_state) {
        right_last_state =right_click;
        systemTask.PushMessage(Pinetime::System::Messages::HandleButtonEvent);
        return;
      }
    }

    // helper function to switch between screens
    void switch_to_screen(uint8_t screen_idx)
    {
      if (screen_idx == 1) {
        settingsController.SetClockFace(0);
        displayApp.StartApp(Pinetime::Applications::Apps::Clock, Pinetime::Applications::DisplayApp::FullRefreshDirections::None);
      }
      else if (screen_idx == 2) {
        settingsController.SetClockFace(1);
        displayApp.StartApp(Pinetime::Applications::Apps::Clock, Pinetime::Applications::DisplayApp::FullRefreshDirections::None);
      }
      else if (screen_idx == 3) {
        settingsController.SetClockFace(2);
        displayApp.StartApp(Pinetime::Applications::Apps::Clock, Pinetime::Applications::DisplayApp::FullRefreshDirections::None);
      }
      else if (screen_idx == 4) {
        displayApp.StartApp(Pinetime::Applications::Apps::Paddle, Pinetime::Applications::DisplayApp::FullRefreshDirections::None);
      }
      else if (screen_idx == 5) {
        displayApp.StartApp(Pinetime::Applications::Apps::Twos, Pinetime::Applications::DisplayApp::FullRefreshDirections::None);
      }
      else if (screen_idx == 6) {
        displayApp.StartApp(Pinetime::Applications::Apps::Metronome, Pinetime::Applications::DisplayApp::FullRefreshDirections::None);
      }
      else if (screen_idx == 7) {
        displayApp.StartApp(Pinetime::Applications::Apps::FirmwareUpdate, Pinetime::Applications::DisplayApp::FullRefreshDirections::None);
      }
      else if (screen_idx == 8) {
        displayApp.StartApp(Pinetime::Applications::Apps::BatteryInfo, Pinetime::Applications::DisplayApp::FullRefreshDirections::None);
      }
      else if (screen_idx == 9) {
        displayApp.StartApp(Pinetime::Applications::Apps::FlashLight, Pinetime::Applications::DisplayApp::FullRefreshDirections::None);
      }
      else if (screen_idx == 0) {
        displayApp.StartApp(Pinetime::Applications::Apps::QuickSettings, Pinetime::Applications::DisplayApp::FullRefreshDirections::None);
      }
      else if (screen_idx == 11) {
        displayApp.StartApp(Pinetime::Applications::Apps::Music, Pinetime::Applications::DisplayApp::FullRefreshDirections::None);
      }
      else if (screen_idx == 12) {
        displayApp.StartApp(Pinetime::Applications::Apps::Paint, Pinetime::Applications::DisplayApp::FullRefreshDirections::None);
      }
      else if (screen_idx == 13) {
        displayApp.StartApp(Pinetime::Applications::Apps::SysInfo, Pinetime::Applications::DisplayApp::FullRefreshDirections::None);
      }
      else if (screen_idx == 14) {
        displayApp.StartApp(Pinetime::Applications::Apps::Steps, Pinetime::Applications::DisplayApp::FullRefreshDirections::None);
      }
      else if (screen_idx == 15) {
        displayApp.StartApp(Pinetime::Applications::Apps::Error, Pinetime::Applications::DisplayApp::FullRefreshDirections::None);
      }
      else if (screen_idx == 17) {
        displayApp.StartApp(Pinetime::Applications::Apps::PassKey, Pinetime::Applications::DisplayApp::FullRefreshDirections::None);
      }
      else {
        std::cout << "unhandled screen_idx: " << int(screen_idx) << std::endl;
      }
    }
    static void screen_off_delete_cb(lv_obj_t *obj, lv_event_t event)
    {
      if (event == LV_EVENT_DELETE) {
        auto* fw = static_cast<Framework*>(obj->user_data);
        if (obj == fw->screen_off_bg)
        {
          // on delete make sure to not double free the screen_off objects
          fw->screen_off_created = false;
        }
      }
    }
    // render the current status of the simulated controller
    void refresh_screen() {
      const Pinetime::Controllers::BrightnessController::Levels level = brightnessController.Level();
      if (level == Pinetime::Controllers::BrightnessController::Levels::Off) {
        if (!screen_off_created) {
          screen_off_created = true;
          screen_off_bg = lv_obj_create(lv_scr_act(), nullptr);
          lv_obj_set_size(screen_off_bg, 240, 240);
          lv_obj_set_pos(screen_off_bg, 0, 0);
          lv_obj_set_style_local_bg_color(screen_off_bg, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
          screen_off_bg->user_data = this; // add callback to prevent double free through Alarm Screen
          lv_obj_set_event_cb(screen_off_bg, screen_off_delete_cb);
          screen_off_label = lv_label_create(lv_scr_act(), nullptr);
          lv_label_set_text_static(screen_off_label, "Screen is OFF");
          lv_obj_align(screen_off_label, nullptr, LV_ALIGN_CENTER, 0, -20);
        }
        /* Periodically call the lv_task handler.
        * It could be done in a timer interrupt or an OS task too.*/
        // only call the task handler if the screen is off,
        // when the screen is enabled the call is done in the SystemTask class
        lv_task_handler();
      } else {
        if (screen_off_created) {
          screen_off_created = false;
          lv_obj_del(screen_off_bg);
          lv_obj_del(screen_off_label);
        }
      }

      if (print_memory_usage) {
        lv_mem_monitor(&mem_mon);
        if (mem_mon.free_size != mem_mon_last_free_size) {
          // 14KiB is the LVGL memory size used in InfiniTime
          constexpr uint32_t pinetime_lvgl_memory = 14U*1024U;
          uint32_t mem_used = LV_MEM_SIZE - mem_mon.free_size;
          // The "budget" value shows how much free lvgl memory the PineTime
          // would have free and will go negative when more memory is used
          // in the simulator than is available on the real hardware.
          int32_t budget = pinetime_lvgl_memory - mem_used;
          printf("Mem: %5u used (change: %+5d, peak: %5u) %d budget left\n", mem_used, mem_mon_last_free_size - mem_mon.free_size, mem_mon.max_used, budget);
          mem_mon_last_free_size = mem_mon.free_size;
        }
      }

      if (gif_manager.is_in_progress())
      {
        gif_manager.write_frame();
      }
    }

    bool print_memory_usage = false;
    lv_mem_monitor_t mem_mon;

    // variables to create and destroy an lvgl overlay to indicate a turned off screen
    bool screen_off_created = false;
    lv_obj_t *screen_off_bg;
    lv_obj_t *screen_off_label;

private:
    bool key_handled_r = false; // r ... enable ringing, R ... disable ringing
    bool key_handled_m = false; // m ... let motor run, M ... stop motor
    bool key_handled_n = false; // n ... send notification, N ... clear new notification flag
    bool key_handled_b = false; // b ... connect Bluetooth, B ... disconnect Bluetooth
    bool key_handled_v = false; // battery Voltage and percentage, v ... increase, V ... decrease
    bool key_handled_c = false; // c ... charging, C ... not charging
    bool key_handled_l = false; // l ... increase brightness level, L ... lower brightness level
    bool key_handled_p = false; // p ... enable print memory usage, P ... disable print memory usage
    bool key_handled_s = false; // s ... increase step count, S ... decrease step count
    bool key_handled_h = false; // h ... set heartrate running, H ... stop heartrate
    bool key_handled_i = false; // i ... take screenshot, I ... start/stop Gif screen capture
    // numbers from 0 to 9 to switch between screens
    bool key_handled_1 = false;
    bool key_handled_2 = false;
    bool key_handled_3 = false;
    bool key_handled_4 = false;
    bool key_handled_5 = false;
    bool key_handled_6 = false;
    bool key_handled_7 = false;
    bool key_handled_8 = false;
    bool key_handled_9 = false;
    bool key_handled_0 = false;
    bool visible;   // show Simulator window
    int height;     // Height of the window
    int width;      // Width of the window
    SDL_Renderer *renderer = NULL;      // Pointer for the renderer
    SDL_Window *window = NULL;      // Pointer for the window

    bool left_release_sent = true; // make sure to send one mouse button release event
    bool right_last_state = false; // varable used to send message only on changing state

    uint32_t mem_mon_last_free_size = LV_MEM_SIZE;

    GifManager gif_manager;
};

int main(int argc, char **argv)
{
  // parse arguments
  bool fw_status_window_visible = true;
  bool arg_help = false;
  for (int i=1; i<argc; i++)
  {
    const std::string arg(argv[i]);
    if (arg == "--hide-status")
    {
      fw_status_window_visible = false;
    } else if (arg == "-h" || arg == "--help")
    {
      arg_help = true;
    } else
    {
      std::cout << "unknown argument '" << arg << "'" << std::endl;
      return 1;
    }
  }
  if (arg_help) {
    std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help           show this help message and exit" << std::endl;
    std::cout << "      --hide-status    don't show simulator status window, so only lvgl window is open" << std::endl;
    return 0;
  }

  /*Initialize LVGL*/
  lv_init();

  /*Initialize the HAL (display, input devices, tick) for LVGL*/
  hal_init();

  fs.Init();

  // initialize the core of our Simulator
  Framework fw(fw_status_window_visible, 240,240);

  while(1) {
      fw.handle_keys(); // key event polling
      fw.handle_touch_and_button();
      fw.refresh();
      usleep(LV_DISP_DEF_REFR_PERIOD * 1000);
  }

  return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics
 * library
 */
static void hal_init(void)
{
  /* Use the 'monitor' driver which creates window on PC's monitor to simulate a display*/
  monitor_init();
  /* Tick init.
   * You have to call 'lv_tick_inc()' in periodically to inform LittelvGL about
   * how much time were elapsed Create an SDL thread to do this*/
  SDL_CreateThread(tick_thread, "tick", NULL);

  // use pinetime_theme
  //lv_theme_t* th = lv_pinetime_theme_init(
  //  LV_COLOR_WHITE, LV_COLOR_SILVER, 0, &jetbrains_mono_bold_20, &jetbrains_mono_bold_20, &jetbrains_mono_bold_20, &jetbrains_mono_bold_20);
  //lv_theme_set_act(th);

  ///*Create a display buffer*/
  //static lv_disp_buf_t disp_buf1;
  //static lv_color_t buf1_1[LV_HOR_RES_MAX * 120];
  //lv_disp_buf_init(&disp_buf1, buf1_1, NULL, LV_HOR_RES_MAX * 120);

  ///*Create a display*/
  //lv_disp_drv_t disp_drv;
  //lv_disp_drv_init(&disp_drv); /*Basic initialization*/
  //disp_drv.buffer = &disp_buf1;
  //disp_drv.flush_cb = monitor_flush;
  //lv_disp_drv_register(&disp_drv);

  /* Add the mouse as input device
   * Use the 'mouse' driver which reads the PC's mouse*/
  //mouse_init();
  //static lv_indev_drv_t indev_drv_1;
  //lv_indev_drv_init(&indev_drv_1); /*Basic initialization*/
  //indev_drv_1.type = LV_INDEV_TYPE_POINTER;

  /*This function will be called periodically (by the library) to get the mouse position and state*/
  //indev_drv_1.read_cb = mouse_read;
  //mouse_indev = lv_indev_drv_register(&indev_drv_1);

  /*Add the keyboard as input device.*/
  //lv_indev_drv_t kb_drv;
  //lv_indev_drv_init(&kb_drv);
  //kb_drv.type = LV_INDEV_TYPE_KEYPAD;
  //kb_drv.read_cb = keyboard_read;
  //lv_indev_drv_register(&kb_drv);

  /*Add the mousewheel as input device.*/
  //lv_indev_drv_t enc_drv;
  //lv_indev_drv_init(&enc_drv);
  //enc_drv.type = LV_INDEV_TYPE_ENCODER;
  //enc_drv.read_cb = mousewheel_read;
  //lv_indev_drv_register(&enc_drv);

  /*Set a cursor for the mouse*/
  //LV_IMG_DECLARE(mouse_cursor_icon); /*Declare the image file.*/
  //lv_obj_t * cursor_obj = lv_img_create(lv_scr_act(), NULL); /*Create an image object for the cursor */
  //lv_img_set_src(cursor_obj, &mouse_cursor_icon);           /*Set the image source*/
  //lv_indev_set_cursor(mouse_indev, cursor_obj);             /*Connect the image  object to the driver*/
}

/**
 * A task to measure the elapsed time for LVGL
 * @param data unused
 * @return never return
 */
static int tick_thread(void *data) {
    (void)data;

    while(1) {
        SDL_Delay(LV_DISP_DEF_REFR_PERIOD);
        lv_tick_inc(LV_DISP_DEF_REFR_PERIOD); /*Tell LittelvGL that 30 milliseconds were elapsed*/
    }

    return 0;
}
