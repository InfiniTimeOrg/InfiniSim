#include "displayapp/screens/Missing.h"
#include "displayapp/DisplayApp.h"

using namespace Pinetime::Applications::Screens;

Missing::Missing(Pinetime::Applications::DisplayApp* app, Pinetime::Applications::Apps app_key) : Screen(app) {
  const char *screen_lbl;
  switch (app_key) {
    case Pinetime::Applications::Apps::None:
      screen_lbl = "None";
      break;
    case Pinetime::Applications::Apps::Launcher:
      screen_lbl = "Launcher";
      break;
    case Pinetime::Applications::Apps::Clock:
      screen_lbl = "Clock";
      break;
    case Pinetime::Applications::Apps::SysInfo:
      screen_lbl = "SysInfo";
      break;
    case Pinetime::Applications::Apps::FirmwareUpdate:
      screen_lbl = "FirmwareUpdate";
      break;
    case Pinetime::Applications::Apps::FirmwareValidation:
      screen_lbl = "FirmwareValidation";
      break;
    case Pinetime::Applications::Apps::NotificationsPreview:
      screen_lbl = "NotificationPreview";
      break;
    case Pinetime::Applications::Apps::Notifications:
      screen_lbl = "Notifications";
      break;
    case Pinetime::Applications::Apps::Timer:
      screen_lbl = "Timer";
      break;
    case Pinetime::Applications::Apps::Alarm:
      screen_lbl = "Alarm";
      break;
    case Pinetime::Applications::Apps::FlashLight:
      screen_lbl = "FlashLight";
      break;
    case Pinetime::Applications::Apps::BatteryInfo:
      screen_lbl = "BatteryInfo";
      break;
    case Pinetime::Applications::Apps::Music:
      screen_lbl = "Music";
      break;
    case Pinetime::Applications::Apps::Paint:
      screen_lbl = "Paint";
      break;
    case Pinetime::Applications::Apps::Paddle:
      screen_lbl = "Paddle";
      break;
    case Pinetime::Applications::Apps::Twos:
      screen_lbl = "Twos";
      break;
    case Pinetime::Applications::Apps::HeartRate:
      screen_lbl = "HeartRate";
      break;
    case Pinetime::Applications::Apps::Navigation:
      screen_lbl = "Navigation";
      break;
    case Pinetime::Applications::Apps::StopWatch:
      screen_lbl = "StopWatch";
      break;
    case Pinetime::Applications::Apps::Metronome:
      screen_lbl = "Metronome";
      break;
    case Pinetime::Applications::Apps::Motion:
      screen_lbl = "Motion";
      break;
    case Pinetime::Applications::Apps::Steps:
      screen_lbl = "Steps";
      break;
    case Pinetime::Applications::Apps::Weather:
      screen_lbl = "Weather";
      break;
    case Pinetime::Applications::Apps::PassKey:
      screen_lbl = "PassKey";
      break;
    case Pinetime::Applications::Apps::QuickSettings:
      screen_lbl = "QuickSettings";
      break;
    case Pinetime::Applications::Apps::Settings:
      screen_lbl = "Settings";
      break;
    case Pinetime::Applications::Apps::SettingWatchFace:
      screen_lbl = "SettingWatchFace";
      break;
    case Pinetime::Applications::Apps::SettingTimeFormat:
      screen_lbl = "SettingTimeFormat";
      break;
    case Pinetime::Applications::Apps::SettingDisplay:
      screen_lbl = "SettingDisplay";
      break;
    case Pinetime::Applications::Apps::SettingWakeUp:
      screen_lbl = "SettingWakeUp";
      break;
    case Pinetime::Applications::Apps::SettingSteps:
      screen_lbl = "SettingSteps";
      break;
    case Pinetime::Applications::Apps::SettingSetDate:
      screen_lbl = "SettingSetDate";
      break;
    case Pinetime::Applications::Apps::SettingSetTime:
      screen_lbl = "SettingSetTime";
      break;
    case Pinetime::Applications::Apps::SettingChimes:
      screen_lbl = "SettingChimes";
      break;
    case Pinetime::Applications::Apps::SettingShakeThreshold:
      screen_lbl = "SettingThreshold";
      break;
    case Pinetime::Applications::Apps::Error:
      screen_lbl = "Error";
      break;
    //case Pinetime::Applications::Apps::Weather:
    //  screen_lbl = "Weather";
    //  break;
    default:
      screen_lbl = "unkown screen";
  }
  lv_obj_t* label = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(label, screen_lbl);
  lv_obj_align(label, nullptr, LV_ALIGN_CENTER, 0, -20);
}

Missing::~Missing() {
  lv_obj_clean(lv_scr_act());
}

