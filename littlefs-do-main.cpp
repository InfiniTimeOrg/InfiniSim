
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
#include <typeinfo>
#include <algorithm>
#include <filesystem>
#include <cmath> // std::pow

#include "components/fs/FS.h"
#include "components/settings/Settings.h"
#include "drivers/SpiNorFlash.h"

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
  std::cout << "  stat                 show information of specified file or directory" << std::endl;
  std::cout << "  ls                   list available files in 'spiNorFlash.raw' file" << std::endl;
  std::cout << "  mkdir                create directory" << std::endl;
  std::cout << "  rmdir                remove directory" << std::endl;
  std::cout << "  rm                   remove directory or file" << std::endl;
  std::cout << "  cp                   copy files into or out of flash file" << std::endl;
  std::cout << "  settings             list settings from 'settings.h'" << std::endl;
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

int command_stat(int argc, char **argv)
{
  std::cout << "running command 'stat'" << std::endl;
  // argv: littlefs-do stat [args]
  std::string path = "/";
  // check for help flag first
  for (int i=2; i<argc; i++)
  {
    const std::string arg(argv[i]);
    if (arg == "-h" || arg == "--help")
    {
      print_help_stat(argv[0]);
      return 0;
    }
  }
  if (argc == 2) {
    std::cout << "no path given, showing '/'" << std::endl;
  } if (argc == 3) {
    path = argv[2];
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

int command_ls(int argc, char **argv)
{
  std::cout << "running command 'ls'" << std::endl;
  // argv: littlefs-do ls [args]
  std::string path = "/";
  // check for help flag first
  for (int i=2; i<argc; i++)
  {
    const std::string arg(argv[i]);
    if (arg == "-h" || arg == "--help")
    {
      print_help_ls(argv[0]);
      return 0;
    }
  }
  if (argc == 2) {
    std::cout << "no path given, showing '/'" << std::endl;
  } if (argc == 3) {
    path = argv[2];
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
int command_mkdir(int argc, char **argv)
{
  std::cout << "running command 'mkdir'" << std::endl;
  // argv: littlefs-do mkdir path
  // check for help flag first
  for (int i=2; i<argc; i++)
  {
    const std::string arg(argv[i]);
    if (arg == "-h" || arg == "--help")
    {
      print_help_mkdir(argv[0]);
      return 0;
    }
  }
  if (argc == 2) {
    std::cout << "error: no path given" << std::endl;
    print_help_mkdir(argv[0]);
    return 1;
  }
  for (int i=2; i<argc; i++)
  {
    const std::string path(argv[i]);
    std::cout << "mkdir: " << path << std::endl;
    int ret = fs.DirCreate(path.c_str());
    if (ret < 0) {
      std::cout << "fs.DirCreate returned error code: " << ret  << " " << lfs_error_to_string(ret) << std::endl;
      return ret;
    }
  }
  return 0;
}

int command_rmdir(int argc, char **argv)
{
  std::cout << "running command 'rmdir'" << std::endl;
  // argv: littlefs-do rmdir path
  // check for help flag first
  for (int i=2; i<argc; i++)
  {
    const std::string arg(argv[i]);
    if (arg == "-h" || arg == "--help")
    {
      print_help_rmdir(argv[0]);
      return 0;
    }
  }
  if (argc == 2) {
    std::cout << "error: no path given" << std::endl;
    print_help_rmdir(argv[0]);
    return 1;
  }
  for (int i=2; i<argc; i++)
  {
    const std::string path(argv[i]);
    std::cout << "rmdir: " << path << std::endl;
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

int command_rm(int argc, char **argv)
{
  std::cout << "running command 'rm'" << std::endl;
  // argv: littlefs-do rm path
  // check for help flag first
  for (int i=2; i<argc; i++)
  {
    const std::string arg(argv[i]);
    if (arg == "-h" || arg == "--help")
    {
      print_help_rm(argv[0]);
      return 0;
    }
  }
  if (argc == 2) {
    std::cout << "error: no path given" << std::endl;
    print_help_rm(argv[0]);
    return 1;
  }
  for (int i=2; i<argc; i++)
  {
    const std::string path(argv[i]);
    std::cout << "rm: " << path << std::endl;
    // assume non-files are directories
    int ret = fs.FileDelete(path.c_str());
    if (ret < 0) {
      std::cout << "fs.FileDelete returned error code: " << ret  << " " << lfs_error_to_string(ret) << std::endl;
      return ret;
    }
  }
  return 0;
}

int command_cp(int argc, char **argv)
{
  std::cout << "running 'cp'" << std::endl;
  for (int i=2; i<argc; i++)
  {
    const std::string arg(argv[i]);
    if (arg == "-h" || arg == "--help")
    {
      print_help_cp(argv[0]);
      return 0;
    }
  }
  if (argc < 4) {
    std::cout << "error: no destination given, need source and destination" << std::endl;
    print_help_cp(argv[0]);
    return 1;
  }
  std::string destination = argv[argc-1];
  static constexpr size_t memorySize {0x400000};
  std::array<uint8_t, memorySize> buffer;
  if (destination[0] == '/') {
    std::cout << "destination starts with '/', copying files into image" << std::endl;
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
    for (int i=2; i<argc-1; i++)
    {
      const std::string source(argv[i]);
      if (!std::filesystem::exists(source))
      {
        std::cout << "error: source file not found: '" << source << "'" << std::endl;
        return 1;
      }
      const std::string dest_path = (std::filesystem::path{destination} / std::filesystem::path{source}.filename()).generic_string();
      std::cout << "copy file: " << source << " to " << dest_path << std::endl;
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
    std::cout << "destination not starting with '/', copying files from image to host" << std::endl;
    if (!std::filesystem::is_directory(destination))
    {
      std::cout << "error: destination is expected to be a directory: '" << destination << "'" << std::endl;
      return 1;
    }
    for (int i=2; i<argc-1; i++)
    {
      const std::string source(argv[i]);
      const std::string dest_path = (std::filesystem::path{destination} / std::filesystem::path{source}.filename()).generic_string();
      std::cout << "copy file: " << source << " to " << dest_path << std::endl;
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

int command_settings(int argc, char **argv)
{
  std::cout << "running 'settings'" << std::endl;
  for (int i=2; i<argc; i++)
  {
    const std::string arg(argv[i]);
    if (arg == "-h" || arg == "--help")
    {
      print_help_settings(argv[0]);
      return 0;
    }
  }

  std::cout << "calling Settings::Init()" << std::endl;
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
      if (val == Settings::Notification::ON) return "ON";
      if (val == Settings::Notification::OFF) return "OFF";
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

int main(int argc, char **argv)
{
  // parse arguments
  if (argc <= 1) {
    print_help_generic(argv[0]);
    return 1;
  }
  std::cout << "Calling FS::Init()" << std::endl;
  fs.Init();

  for (int i=1; i<argc; i++)
  {
    const std::string arg(argv[i]);
    if (arg == "-h" || arg == "--help")
    {
      print_help_generic(argv[0]);
      return 0;
    } else if (arg == "stat") {
      return command_stat(argc, argv);
    } else if (arg == "ls") {
      return command_ls(argc, argv);
    } else if (arg == "mkdir") {
      return command_mkdir(argc, argv);
    } else if (arg == "rmdir") {
      return command_rmdir(argc, argv);
    } else if (arg == "rm") {
      return command_rm(argc, argv);
    } else if (arg == "cp") {
      return command_cp(argc, argv);
    } else if (arg == "settings") {
      return command_settings(argc, argv);
    } else
    {
      std::cout << "unknown argument '" << arg << "'" << std::endl;
      return 1;
    }
  }
  return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
