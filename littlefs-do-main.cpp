
/**
 * @file littlefs-do-main.cpp
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define _DEFAULT_SOURCE /* needed for usleep() */
#include <stdlib.h>
#include <unistd.h>

#include <array>
#include <iostream>
#include <iomanip> // std::left, std::setw
#include <typeinfo>
#include <algorithm>
#include <filesystem>
#include <vector>
#include <cmath> // std::pow

#include "components/fs/FS.h"
#include "components/settings/Settings.h"
#include "drivers/SpiNorFlash.h"

#include "nlohmann/json.hpp"
#include "miniz.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

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

Pinetime::Drivers::SpiNorFlash spiNorFlash {"spiNorFlash.raw"};

Pinetime::Controllers::FS fs {spiNorFlash};
Pinetime::Controllers::Settings settingsController {fs};

const char* lfs_error_to_string(int err) {
  if (err == LFS_ERR_OK)          return "LFS_ERR_OK";           // No error
  if (err == LFS_ERR_IO)          return "LFS_ERR_IO";           // Error during device operation
  if (err == LFS_ERR_CORRUPT)     return "LFS_ERR_CORRUPT";      // Corrupted
  if (err == LFS_ERR_NOENT)       return "LFS_ERR_NOENT";        // No directory entry
  if (err == LFS_ERR_EXIST)       return "LFS_ERR_EXIST";        // Entry already exists
  if (err == LFS_ERR_NOTDIR)      return "LFS_ERR_NOTDIR";       // Entry is not a dir
  if (err == LFS_ERR_ISDIR)       return "LFS_ERR_ISDIR";        // Entry is a dir
  if (err == LFS_ERR_NOTEMPTY)    return "LFS_ERR_NOTEMPTY";     // Dir is not empty
  if (err == LFS_ERR_BADF)        return "LFS_ERR_BADF";         // Bad file number
  if (err == LFS_ERR_FBIG)        return "LFS_ERR_FBIG";         // File too large
  if (err == LFS_ERR_INVAL)       return "LFS_ERR_INVAL";        // Invalid parameter
  if (err == LFS_ERR_NOSPC)       return "LFS_ERR_NOSPC";        // No space left on device
  if (err == LFS_ERR_NOMEM)       return "LFS_ERR_NOMEM";        // No more memory available
  if (err == LFS_ERR_NOATTR)      return "LFS_ERR_NOATTR";       // No data/attr available
  if (err == LFS_ERR_NAMETOOLONG) return "LFS_ERR_NAMETOOLONG";  // File name too long
  return "unknown";
}

void print_help_generic(const std::string &program_name)
{
  std::cout << "Usage: " << program_name << " <command> [options]" << std::endl;
  std::cout << "Commands:" << std::endl;
  std::cout << "  -h, --help           show this help message for the selected command and exit" << std::endl;
  std::cout << "  -v, --verbose        print status messages to the console" << std::endl;
  std::cout << "  stat                 show information of specified file or directory" << std::endl;
  std::cout << "  ls                   list available files in 'spiNorFlash.raw' file" << std::endl;
  std::cout << "  mkdir                create directory" << std::endl;
  std::cout << "  rmdir                remove directory" << std::endl;
  std::cout << "  rm                   remove directory or file" << std::endl;
  std::cout << "  cp                   copy files into or out of flash file" << std::endl;
  std::cout << "  settings             list settings from 'settings.h'" << std::endl;
  std::cout << "  res                  resource.zip handling" << std::endl;
}
void print_help_stat(const std::string &program_name)
{
  std::cout << "Usage: " << program_name << " stat [options] [path]" << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "  -h, --help           show this help message for the selected command and exit" << std::endl;
  std::cout << "  path                 path to directory or file to work on, defaults to '/'" << std::endl;
}
void print_help_ls(const std::string &program_name)
{
  std::cout << "Usage: " << program_name << " ls [options] [path]" << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "  -h, --help           show this help message for the selected command and exit" << std::endl;
  std::cout << "  path                 path to directory or file to work on, defaults to '/'" << std::endl;
}
void print_help_mkdir(const std::string &program_name)
{
  std::cout << "Usage: " << program_name << " mkdir [options] path" << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "  -h, --help           show this help message for the selected command and exit" << std::endl;
  std::cout << "  path                 path to directory to create" << std::endl;
}
void print_help_rmdir(const std::string &program_name)
{
  std::cout << "Usage: " << program_name << " rmdir [options] path" << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "  -h, --help           show this help message for the selected command and exit" << std::endl;
  std::cout << "  path                 path to directory to remove" << std::endl;
}
void print_help_rm(const std::string &program_name)
{
  std::cout << "Usage: " << program_name << " rm [options] path" << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "  -h, --help           show this help message for the selected command and exit" << std::endl;
  std::cout << "  path                 path to file or directory directory to remove" << std::endl;
}
void print_help_cp(const std::string &program_name)
{
  std::cout << "Usage: " << program_name << " cp [options] source [source2 ...] destination" << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "  -h, --help           show this help message for the selected command and exit" << std::endl;
  std::cout << std::endl;
  std::cout << "if the destination starts with '/' it is assumed to copy sources from" << std::endl;
  std::cout << "the host system into the raw image, otherwise the files are copied" << std::endl;
  std::cout << "from the raw image to the host system provided directory." << std::endl;
}
void print_help_settings(const std::string &program_name)
{
  std::cout << "Usage: " << program_name << " settings [options]" << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "  -h, --help           show this help message for the selected command and exit" << std::endl;
}
void print_help_res(const std::string &program_name)
{
  std::cout << "Usage: " << program_name << " res <action> [options]" << std::endl;
  std::cout << "actions:" << std::endl;
  std::cout << "  load res.zip         load zip file into SPI memory" << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "  -h, --help           show this help message for the selected command and exit" << std::endl;
}

int command_stat(const std::string &program_name, const std::vector<std::string> &args, bool verbose)
{
  if (verbose)
    std::cout << "running command 'stat'" << std::endl;
  // argv: littlefs-do stat [args]
  std::string path = "/";
  // check for help flag first
  for (const std::string &arg : args)
  {
    if (arg == "-h" || arg == "--help")
    {
      print_help_stat(program_name);
      return 0;
    }
  }
  if (args.empty()) {
    if (verbose) {
      std::cout << "no path given, showing '/'" << std::endl;
    }
  } if (args.size() == 1) {
    path = args.at(0);
  }

  lfs_info info;
  int ret = fs.Stat(path.c_str(), &info);
  if (ret) {
    std::cout << "fs.Stat returned error code: " << ret  << " " << lfs_error_to_string(ret) << std::endl;
    return ret;
  }
  if (info.type == LFS_TYPE_REG) {
    std::cout << "type: REG" << std::endl;
    std::cout << "size: " << info.size << std::endl;
    std::cout << "name: " << std::string(info.name) << std::endl;
  } else if (info.type == LFS_TYPE_DIR) {
    std::cout << "type: DIR" << std::endl;
    std::cout << "name: " << std::string(info.name) << std::endl;
  }
  else {
    std::cout << "unknown type: " << info.type << std::endl;
    return 1;
  }
  return 0;
}

int command_ls(const std::string &program_name, const std::vector<std::string> &args, bool verbose)
{
  if (verbose)
    std::cout << "running command 'ls'" << std::endl;
  // argv: littlefs-do ls [args]
  std::string path = "/";
  // check for help flag first
  for (const std::string &arg : args)
  {
    if (arg == "-h" || arg == "--help")
    {
      print_help_ls(program_name);
      return 0;
    }
  }
  if (args.empty()) {
    if (verbose) {
      std::cout << "no path given, showing '/'" << std::endl;
    }
  } if (args.size() == 1) {
    path = args.at(0);
  }

  lfs_info info;
  int ret = fs.Stat(path.c_str(), &info);
  if (ret) {
    std::cout << "fs.Stat returned error code: " << ret  << " " << lfs_error_to_string(ret) << std::endl;
    return ret;
  }
  if (info.type == LFS_TYPE_REG) {
    std::cout << "type: REG" << std::endl;
    std::cout << "size: " << info.size << std::endl;
    std::cout << "name: " << std::string(info.name) << std::endl;
  } else if (info.type == LFS_TYPE_DIR) {
    std::cout << "type: DIR" << std::endl;
    std::cout << "name: " << std::string(info.name) << std::endl;
    lfs_dir_t lfs_dir;
    ret = fs.DirOpen(path.c_str(), &lfs_dir);
    if (ret) {
      std::cout << "fs.DirOpen returned error code: " << ret  << " " << lfs_error_to_string(ret) << std::endl;
      return ret;
    }
    ret = fs.DirRead(&lfs_dir, &info);
    while (ret > 0) {
      if (ret < 0) {
        std::cout << "fs.DirRead returned error code: " << ret  << " " << lfs_error_to_string(ret) << std::endl;
        return ret;
      }
      if (info.type == LFS_TYPE_REG) {
        std::cout << "type: REG";
        std::cout << " size: " << info.size;
        std::cout << " name: " << std::string(info.name) << std::endl;
      } else if (info.type == LFS_TYPE_DIR) {
        std::cout << "type: DIR";
        std::cout << " name: " << std::string(info.name) << std::endl;
      } else {
        std::cout << "unknown type: " << info.type << std::endl;
        return 1;
      }
      // fill for next iteration
      ret = fs.DirRead(&lfs_dir, &info);
    } // end of while loop
    if (ret < 0) {
      std::cout << "fs.DirRead returned error code: " << ret  << " " << lfs_error_to_string(ret) << std::endl;
      return ret;
    }
  } else { // endif provided path
    std::cout << "unknown type: " << info.type << std::endl;
    return 1;
  }
  return 0;
}
int command_mkdir(const std::string &program_name, const std::vector<std::string> &args, bool verbose)
{
  if (verbose) {
    std::cout << "running command 'mkdir'" << std::endl;
  }
  // argv: littlefs-do mkdir path
  // check for help flag first
  for (const std::string &arg : args)
  {
    if (arg == "-h" || arg == "--help")
    {
      print_help_mkdir(program_name);
      return 0;
    }
  }
  if (args.empty()) {
    std::cout << "error: no path given" << std::endl;
    print_help_mkdir(program_name);
    return 1;
  }
  for (const std::string &path : args)
  {
    if (verbose) {
      std::cout << "mkdir: " << path << std::endl;
    }
    int ret = fs.DirCreate(path.c_str());
    if (ret < 0) {
      std::cout << "fs.DirCreate returned error code: " << ret  << " " << lfs_error_to_string(ret) << std::endl;
      return ret;
    }
  }
  return 0;
}

int command_rmdir(const std::string &program_name, const std::vector<std::string> &args, bool verbose)
{
  if (verbose) {
    std::cout << "running command 'rmdir'" << std::endl;
  }
  // argv: littlefs-do rmdir path
  // check for help flag first
  for (const std::string &arg : args)
  {
    if (arg == "-h" || arg == "--help")
    {
      print_help_rmdir(program_name);
      return 0;
    }
  }
  if (args.empty()) {
    std::cout << "error: no path given" << std::endl;
    print_help_rmdir(program_name);
    return 1;
  }
  for (const std::string &path : args)
  {
    if (verbose) {
      std::cout << "rmdir: " << path << std::endl;
    }
    lfs_info info;
    int ret = fs.Stat(path.c_str(), &info);
    if (ret) {
      std::cout << "fs.Stat returned error code: " << ret  << " " << lfs_error_to_string(ret) << std::endl;
      return ret;
    }
    if (info.type == LFS_TYPE_REG) {
      std::cout << "error: provided path '" << path << "is a file" << std::endl;
      return 1;
    }
    // assume non-files are directories
    ret = fs.FileDelete(path.c_str());
    if (ret < 0) {
      std::cout << "fs.FileDelete returned error code: " << ret  << " " << lfs_error_to_string(ret) << std::endl;
      return ret;
    }
  }
  return 0;
}

int command_rm(const std::string &program_name, const std::vector<std::string> &args, bool verbose)
{
  if (verbose) {
    std::cout << "running command 'rm'" << std::endl;
  }
  // argv: littlefs-do rm path
  // check for help flag first
  for (const std::string &arg : args)
  {
    if (arg == "-h" || arg == "--help")
    {
      print_help_rm(program_name);
      return 0;
    }
  }
  if (args.empty()) {
    std::cout << "error: no path given" << std::endl;
    print_help_rm(program_name);
    return 1;
  }
  for (const std::string &path : args)
  {
    if (verbose) {
      std::cout << "rm: " << path << std::endl;
    }
    // assume non-files are directories
    int ret = fs.FileDelete(path.c_str());
    if (ret < 0) {
      std::cout << "fs.FileDelete returned error code: " << ret  << " " << lfs_error_to_string(ret) << std::endl;
      return ret;
    }
  }
  return 0;
}

int command_cp(const std::string &program_name, const std::vector<std::string> &args, bool verbose)
{
  if (verbose) {
    std::cout << "running 'cp'" << std::endl;
  }
  for (const std::string &arg : args)
  {
    if (arg == "-h" || arg == "--help")
    {
      print_help_cp(program_name);
      return 0;
    }
  }
  if (args.size() < 2) {
    std::cout << "error: no destination given, need source and destination" << std::endl;
    print_help_cp(program_name);
    return 1;
  }
  const std::string &destination = args.back();
  static constexpr size_t memorySize {0x400000};
  std::array<uint8_t, memorySize> buffer;
  if (destination[0] == '/') {
    if (verbose) {
      std::cout << "destination starts with '/', copying files into image" << std::endl;
    }
    {
      lfs_info info;
      int ret = fs.Stat(destination.c_str(), &info);
      if (ret) {
        std::cout << "fs.Stat for destination path returned error code: " << ret  << " " << lfs_error_to_string(ret) << std::endl;
        return ret;
      }
      if (info.type == LFS_TYPE_REG) {
        std::cout << "destination is file, assumed directory" << std::endl;
        return 1;
      }
    }
    for (size_t i=0; i<args.size()-1; i++)
    {
      const std::string &source = args.at(i);
      if (!std::filesystem::exists(source))
      {
        std::cout << "error: source file not found: '" << source << "'" << std::endl;
        return 1;
      }
      const std::string dest_path = (std::filesystem::path{destination} / std::filesystem::path{source}.filename()).generic_string();
      if (verbose) {
        std::cout << "copy file: " << source << " to " << dest_path << std::endl;
      }
      lfs_file_t file_p;
      int ret = fs.FileOpen(&file_p, dest_path.c_str(), LFS_O_WRONLY | LFS_O_CREAT);
      if (ret) {
        std::cout << "fs.FileOpen returned error code: " << ret  << " " << lfs_error_to_string(ret) << std::endl;
        return ret;
      }
      const size_t f_size = std::filesystem::file_size(source);
      std::ifstream ifs(source, std::ios::binary);
      ifs.read((char*)(buffer.data()), f_size);
      ret = fs.FileWrite(&file_p, buffer.data(), f_size);
      if (ret < 0) {
        std::cout << "fs.FileWrite returned error code: " << ret  << " " << lfs_error_to_string(ret) << std::endl;
        fs.FileClose(&file_p);
        return ret;
      }
      fs.FileClose(&file_p);
    }
  } // end cp from host to raw image
  else
  {
    if (verbose) {
      std::cout << "destination not starting with '/', copying files from image to host" << std::endl;
    }
    if (!std::filesystem::is_directory(destination))
    {
      std::cout << "error: destination is expected to be a directory: '" << destination << "'" << std::endl;
      return 1;
    }
    for (size_t i=0; i<args.size()-1; i++)
    {
      const std::string &source = args.at(i);
      const std::string dest_path = (std::filesystem::path{destination} / std::filesystem::path{source}.filename()).generic_string();
      if (verbose) {
        std::cout << "copy file: " << source << " to " << dest_path << std::endl;
      }
      lfs_info info;
      int ret = fs.Stat(source.c_str(), &info);
      if (ret) {
        std::cout << "fs.Stat for source path returned error code: " << ret  << " " << lfs_error_to_string(ret) << std::endl;
        return ret;
      }
      if (info.type == LFS_TYPE_DIR) {
        std::cout << "source is directory, assumed a file" << std::endl;
        return 1;
      }
      lfs_file_t file_p;
      ret = fs.FileOpen(&file_p, source.c_str(), LFS_O_RDONLY);
      if (ret) {
        std::cout << "fs.FileOpen returned error code: " << ret  << " " << lfs_error_to_string(ret) << std::endl;
        return ret;
      }
      ret = fs.FileRead(&file_p, buffer.data(), info.size);
      if (ret < 0) {
        std::cout << "fs.FileRead returned error code: " << ret  << " " << lfs_error_to_string(ret) << std::endl;
        fs.FileClose(&file_p);
        return ret;
      }
      fs.FileClose(&file_p);
      std::ofstream ofs(dest_path, std::ios::binary);
      ofs.write((char*)(buffer.data()), info.size);
    }
  } // end cp from raw image to host
  
  return 0;
}

int command_settings(const std::string &program_name, const std::vector<std::string> &args, bool verbose)
{
  if (verbose) {
    std::cout << "running 'settings'" << std::endl;
  }
  for (const std::string &arg : args)
  {
    if (arg == "-h" || arg == "--help")
    {
      print_help_settings(program_name);
      return 0;
    }
  }

  if (verbose) {
    std::cout << "calling Settings::Init()" << std::endl;
  }
  settingsController.Init();
  using namespace Pinetime::Controllers;
  {
    auto clockface = settingsController.GetClockFace();
    auto clockface_str = [](auto val) {
      if (val == 0) return "Digital";
      if (val == 1) return "Analog";
      if (val == 2) return "PineTimeStyle";
      if (val == 3) return "Terminal";
      return "unknown";
    }(clockface);
    std::cout << "ClockFace: " << static_cast<int>(clockface) << " " << clockface_str << std::endl;
  }
  {
    auto chimes = settingsController.GetChimeOption();
    auto chimes_str = [](auto val) {
      if (val == Settings::ChimesOption::None) return "None";
      if (val == Settings::ChimesOption::Hours) return "Hours";
      if (val == Settings::ChimesOption::HalfHours) return "HalfHours";
      return "unknown";
    }(chimes);
    std::cout << "Chimes: " << static_cast<int>(chimes) << " " << chimes_str << std::endl;
  }
  auto color_str = [](auto c) {
    if (c == Settings::Colors::White) return "White";
    if (c == Settings::Colors::Silver) return "Silver";
    if (c == Settings::Colors::Gray) return "Gray";
    if (c == Settings::Colors::Black) return "Black";
    if (c == Settings::Colors::Red) return "Red";
    if (c == Settings::Colors::Maroon) return "Maroon";
    if (c == Settings::Colors::Yellow) return "Yellow";
    if (c == Settings::Colors::Olive) return "Olive";
    if (c == Settings::Colors::Lime) return "Lime";
    if (c == Settings::Colors::Green) return "Cyan";
    if (c == Settings::Colors::Teal) return "Teal";
    if (c == Settings::Colors::Blue) return "Blue";
    if (c == Settings::Colors::Navy) return "Navy";
    if (c == Settings::Colors::Magenta) return "Magenta";
    if (c == Settings::Colors::Purple) return "Purple";
    if (c == Settings::Colors::Orange) return "Orange";
    return "unknown";
  };
  std::cout << "PTSColorTime: " << color_str(settingsController.GetPTSColorTime()) << std::endl;
  std::cout << "PTSColorBar: " << color_str(settingsController.GetPTSColorBar()) << std::endl;
  std::cout << "PTSColorBG: " << color_str(settingsController.GetPTSColorBG()) << std::endl;
  std::cout << "AppMenu: " << static_cast<int>(settingsController.GetAppMenu()) << std::endl;
  std::cout << "SettingsMenu: " << static_cast<int>(settingsController.GetSettingsMenu()) << std::endl;
  std::cout << "ClockType: " << (settingsController.GetClockType() == Settings::ClockType::H24 ? "H24" : "H12") << std::endl;
  {
    auto notif = settingsController.GetNotificationStatus();
    auto notif_str = [](auto val) {
      if (val == Settings::Notification::On) return "On";
      if (val == Settings::Notification::Off) return "Off";
      if (val == Settings::Notification::Sleep) return "Sleep";
      return "unknown";
    }(notif);
    std::cout << "NotificationStatus: " << static_cast<int>(notif) << " " << notif_str << std::endl;
  }
  std::cout << "ScreenTimeOut: " << settingsController.GetScreenTimeOut() << " ms" << std::endl;
  std::cout << "ShakeThreshold: " << settingsController.GetShakeThreshold() << std::endl;
  {
    std::cout << "WakeUpModes: " << std::endl;
    std::cout << "- SingleTap:  " << (settingsController.isWakeUpModeOn(Settings::WakeUpMode::SingleTap) ? "ON" : "OFF") << std::endl;
    std::cout << "- DoubleTap:  " << (settingsController.isWakeUpModeOn(Settings::WakeUpMode::DoubleTap) ? "ON" : "OFF") << std::endl;
    std::cout << "- RaiseWrist: " << (settingsController.isWakeUpModeOn(Settings::WakeUpMode::RaiseWrist) ? "ON" : "OFF") << std::endl;
    std::cout << "- Shake:      " << (settingsController.isWakeUpModeOn(Settings::WakeUpMode::Shake) ? "ON" : "OFF") << std::endl;
  }
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
  }
  std::cout << "StepsGoal: " << settingsController.GetStepsGoal() << std::endl;
  std::cout << "BleRadioEnabled: " << (settingsController.GetBleRadioEnabled() ? "true" : "false") << std::endl;
  return 0;
}

void mkdir_path(const std::filesystem::path &path) {
  if (!path.is_absolute()) {
    // for absolute paths parent path converges at '/', then parent_path == path
    mkdir_path(std::filesystem::path{"/"} / path);
    return;
  }
  std::filesystem::path parent = path.parent_path();
  if (path == parent) {
    return;
  }
  lfs_info info;
  int ret = fs.Stat(path.generic_string().c_str(), &info);
  if (ret == 0) {
    // directory exists, nothing to do
    return;
  }
  // try to create parent dir first
  mkdir_path(parent);
  // then create current dir
  ret = fs.DirCreate(path.generic_string().c_str());
  if (ret < 0) {
    std::cout << "mkdir_path: fs.DirCreate returned error code: " << ret  << " " << lfs_error_to_string(ret) << std::endl;
    assert(false);
    return;
  }
}
int command_res(const std::string &program_name, const std::vector<std::string> &args, bool verbose)
{
  if (verbose) {
    std::cout << "running 'res'" << std::endl;
  }
  for (const std::string &arg : args)
  {
    if (arg == "-h" || arg == "--help")
    {
      print_help_res(program_name);
      return 0;
    }
  }
  if (args.size() < 1) {
    std::cout << "error: no action specified" << std::endl;
    print_help_res(program_name);
    return 1;
  }
  if (args.at(0) == "load") {
    if (verbose) {
      std::cout << "running 'res load'" << std::endl;
    }
    if (args.size() < 2) {
      std::cout << "error: res load needs at least one path to a ressource bundle to load" << std::endl;
      print_help_res(program_name);
      return 1;
    }
    for (size_t i=1; i<args.size(); i++) {
      const std::filesystem::path path = args.at(i);
      if (verbose) {
        std::cout << "loading resource file: " << path << std::endl;
      }
      if (path.extension() == ".zip") {
        const std::string &zip_filename = args.at(i);
        const size_t f_size = std::filesystem::file_size(path);
        std::vector<uint8_t> buffer_compressed(f_size);
        std::ifstream ifs(path, std::ios::binary);
        ifs.read((char*)(buffer_compressed.data()), f_size);

        mz_zip_archive zip_archive {};
        int mz_status = mz_zip_reader_init_file(&zip_archive, zip_filename.c_str(), 0);
        if (!mz_status) {
          std::cout << "error: mz_zip_reader_init_file() failed!" << std::endl;
          return 1;
        }

        mz_uint zip_num_files = mz_zip_reader_get_num_files(&zip_archive);
        if (verbose) {
          std::cout << "zip: num of files in zip: " << zip_num_files << std::endl;
        }

        size_t uncomp_size = 0;
        void *p = nullptr;
        // extract resources.json file to heap, create a string and parse it
        p = mz_zip_reader_extract_file_to_heap(&zip_archive, "resources.json", &uncomp_size, 0);
        if (!p)
        {
          std::cout << "mz_zip_reader_extract_file_to_heap() failed to extract resources.json file" << std::endl;
          mz_zip_reader_end(&zip_archive);
          return 1;
        }
        std::string_view json_data(static_cast<const char *>(p), uncomp_size);
        nlohmann::json doc = nlohmann::json::parse(json_data);
        mz_free(p); // free json data, already converted into json document
        if (!doc.contains("resources")) {
          std::cout << "resources.json is missing 'resources' entry" << std::endl;
          mz_zip_reader_end(&zip_archive);
          return 1;
        }
        // copy all listed resources to SPI raw file
        for (const auto &res : doc["resources"]) {
          const auto filename = res["filename"].get<std::string>();
          const auto dest_path = res["path"].get<std::string>();
          if (verbose) {
            std::cout << "copy file " << std::left << std::setw(25) << filename
              << " from zip to SPI path '" << dest_path << "'" << std::endl;
          }
          // make sure destination directory exists before copy
          const std::filesystem::path dest_dir = std::filesystem::path{dest_path}.parent_path();
          mkdir_path(dest_dir);
          // extract from zip to heap to then copy to SPI raw file
          void *p = mz_zip_reader_extract_file_to_heap(&zip_archive, filename.c_str(), &uncomp_size, 0);
          if (!p) {
            std::cout << "mz_zip_reader_extract_file_to_heap() failed to extract file: " << filename << std::endl;
            mz_zip_reader_end(&zip_archive);
            return 1;
          }
          lfs_file_t file_p;
          int ret = fs.FileOpen(&file_p, dest_path.c_str(), LFS_O_WRONLY | LFS_O_CREAT);
          if (ret) {
            std::cout << "fs.FileOpen returned error code: " << ret  << " " << lfs_error_to_string(ret) << std::endl;
            return ret;
          }
          ret = fs.FileWrite(&file_p, static_cast<uint8_t *>(p), uncomp_size);
          if (ret < 0) {
            std::cout << "fs.FileWrite returned error code: " << ret  << " " << lfs_error_to_string(ret) << std::endl;
            fs.FileClose(&file_p);
            return ret;
          }
          // file copy complete, close destination file and free data
          fs.FileClose(&file_p);
          mz_free(p);
        }
        // all done, close archive
        mz_zip_reader_end(&zip_archive);
        if (verbose) {
          std::cout << "finished: zip file fully loaded into SPI memory: " << zip_filename << std::endl;
        }

      } else {
        std::cout << "error: resource has unknown extension: " << args.at(i) << std::endl;
        print_help_res(program_name);
        return 1;
      }
    }
  } else {
    std::cout << "error: unknown res action '" << args.at(0) << "'" << std::endl;
    print_help_res(program_name);
    return 1;
  }
  return 0;
}


int main(int argc, char **argv)
{
  // parse arguments
  if (argc <= 1) {
    print_help_generic(argv[0]);
    return 1;
  }
  bool verbose = false;
  std::vector<std::string> args;
  for (int i=1; i<argc; i++)
  {
    const std::string arg(argv[i]);
    if (arg == "-v" || arg == "--verbose") {
      verbose = true;
    } else {
      args.push_back(arg);
    }
  }
  if (args.empty()) {
    print_help_generic(argv[0]);
    return 1;
  }
  if (verbose) {
    std::cout << "Calling FS::Init()" << std::endl;
  }
  fs.Init();

  const std::string command = args.front();
  args.erase(args.begin()); // pop_front

  if (command == "-h" || command == "--help") {
    print_help_generic(argv[0]);
    return 0;
  } else if (command == "stat") {
    return command_stat(argv[0], args, verbose);
  } else if (command == "ls") {
    return command_ls(argv[0], args, verbose);
  } else if (command == "mkdir") {
    return command_mkdir(argv[0], args, verbose);
  } else if (command == "rmdir") {
    return command_rmdir(argv[0], args, verbose);
  } else if (command == "rm") {
    return command_rm(argv[0], args, verbose);
  } else if (command == "cp") {
    return command_cp(argv[0], args, verbose);
  } else if (command == "settings") {
    return command_settings(argv[0], args, verbose);
  } else if (command == "res") {
    return command_res(argv[0], args, verbose);
  } else
  {
    std::cout << "unknown argument '" << command << "'" << std::endl;
    return 1;
  }
  return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
