#!/usr/bin/env python3
import argparse
import pathlib
import re
import sys

def main():
    parser = argparse.ArgumentParser(description="Parse settings.h and create a status printing c++ file")
    parser.add_argument("settings_h_file", type=str,
        help="path to 'settings.h' file to parse")
    parser.add_argument("-o", "--output", type=str,
        help="file to generated write c++ code into",
        default="-")
    args = parser.parse_args()

    with open(args.settings_h_file, encoding="utf-8") as f:
        settings = f.read()
    enum_matcher = re.compile(r"enum class ([a-zA-Z]+) : uint8_t \{[ \n]*([^}]*)\}\;")
    enum_matches = enum_matcher.findall(settings)

    # get all enums
    def match_to_enum(match):
        enum_name = match[0]
        entries_str = match[1]
        entries = [e.strip() for e in entries_str.split(",")]
        entries_filtered = [e.split("=")[0].strip() if "=" in e else e for e in entries if e]
        return (enum_name, entries_filtered)
    enum_list = [match_to_enum(e) for e in enum_matches]
    enums = {key: entries for key, entries in enum_list}
    print("extracted enums:")
    for e in enums:
        print("  ", e, enums[e])
    print("")
    #print(enums)

    # get all Getters
    m_getters = re.findall(r"([a-zA-Z_0-9<>]+) ([gG]et[a-zA-Z]+)\(\) const", settings)
    print("extracted getter:")
    for m in m_getters:
        print("  ", m)
    print("")
    
    def getter_to_cpp(m_getter, enums):
        getter_ret, getter_call = m_getter
        setting_name = getter_call[3:]
        kwargs = {
            "getter_ret": getter_ret,
            "getter_call": getter_call,
            "setting_name": setting_name,
        }
        out = ""

        if getter_ret in enums:
            out += """
    {{
      auto {setting_name}_str = [](auto val) {{""".format(**kwargs)
            enum_items = enums[getter_ret]
            for enum_entry in enum_items:
                out += """
        if (val == Settings::{getter_ret}::{enum_entry}) return "{enum_entry}";""".format(**kwargs, enum_entry=enum_entry)
            out += """
        return "unknown";
      }};
      std::cout << "{setting_name}: " << static_cast<int>(settingsController.{getter_call}()) << " " << {setting_name}_str(settingsController.{getter_call}()) << std::endl;
    }}""".format(**kwargs)

        elif setting_name == "ClockFace":
            out += """
    {{
      auto clockface = settingsController.GetClockFace();
      auto clockface_str = [](auto val) {{
        if (val == 0) return "Digital";
        if (val == 1) return "Analog";
        if (val == 2) return "PineTimeStyle";
        if (val == 3) return "Terminal";
        return "unknown";
      }}(clockface);
      std::cout << "ClockFace: " << static_cast<int>(clockface) << " " << clockface_str << std::endl;
    }}""".format(getter_ret=getter_ret, getter_call=getter_call, setting_name=setting_name)

        elif setting_name == "ScreenTimeOut":
            out += """
    {{
      {getter_ret} val = settingsController.{getter_call}();
      std::cout << "{setting_name}: " << static_cast<int>(settingsController.{getter_call}()) << " ms" << std::endl;
    }}""".format(getter_ret=getter_ret, getter_call=getter_call, setting_name=setting_name)

        elif setting_name == "WakeUpModes":
            out += """
    {{
      std::cout << "WakeUpModes: " << std::endl;
      std::cout << "- SingleTap:  " << (settingsController.isWakeUpModeOn(Settings::WakeUpMode::SingleTap) ? "ON" : "OFF") << std::endl;
      std::cout << "- DoubleTap:  " << (settingsController.isWakeUpModeOn(Settings::WakeUpMode::DoubleTap) ? "ON" : "OFF") << std::endl;
      std::cout << "- RaiseWrist: " << (settingsController.isWakeUpModeOn(Settings::WakeUpMode::RaiseWrist) ? "ON" : "OFF") << std::endl;
      std::cout << "- Shake:      " << (settingsController.isWakeUpModeOn(Settings::WakeUpMode::Shake) ? "ON" : "OFF") << std::endl;
    }}""".format(getter_ret=getter_ret, getter_call=getter_call, setting_name=setting_name)

        elif setting_name == "Brightness":
            # Brightness levels are stored in components/brightness/BrightnessController.h
            # create special handling
            out += """
    {
      auto brightness = settingsController.GetBrightness();
      auto brightness_str = [](auto val) {
        if (val == BrightnessController::Levels::Off) return "Off";
        if (val == BrightnessController::Levels::Low) return "Low";
        if (val == BrightnessController::Levels::Medium) return "Medium";
        if (val == BrightnessController::Levels::High) return "High";
        return "unknown";
      }(brightness);
      std::cout << "Brightness: " << static_cast<int>(brightness) << " " << brightness_str << std::endl;
    }"""

        elif getter_ret == "bool":
            out += """
    {{
      {getter_ret} val = settingsController.{getter_call}();
      std::cout << "{setting_name}: " << (settingsController.{getter_call}() ? "true" : "false") << std::endl;
    }}""".format(getter_ret=getter_ret, getter_call=getter_call, setting_name=setting_name)

        else: # just output non-enum entry
            out += """
    {{
      {getter_ret} val = settingsController.{getter_call}();
      std::cout << "{setting_name}: " << static_cast<int>(settingsController.{getter_call}()) << std::endl;
    }}""".format(getter_ret=getter_ret, getter_call=getter_call, setting_name=setting_name)

        return out
    
    list_settings_function = """
#include <iostream>
#include "components/settings/Settings.h"
void list_settings(const Pinetime::Controllers::Settings &settingsController) {
  using namespace Pinetime::Controllers;"""
    for m in m_getters:
        list_settings_function += getter_to_cpp(m, enums)

    list_settings_function += """
}"""
    if args.output == "" or args.output == "-":
        print(list_settings_function)
    else:
        with open(args.output, "w", encoding="utf-8") as f:
            f.write(list_settings_function)
    return 0

if __name__ == '__main__':
    sys.exit(main())
