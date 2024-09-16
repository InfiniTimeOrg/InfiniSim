# [InfiniSim](https://github.com/InfiniTimeOrg/InfiniSim)

[![Build InfiniSim LVGL Simulator](https://github.com/InfiniTimeOrg/InfiniSim/actions/workflows/lv_sim.yml/badge.svg)](https://github.com/InfiniTimeOrg/InfiniSim/actions/workflows/lv_sim.yml)

Simulator for [InfiniTime](https://github.com/InfiniTimeOrg/InfiniTime) project.

Experience the `InfiniTime` user interface directly on your PC, to shorten the time until you get your hands on a real [PineTime smartwatch](https://www.pine64.org/pinetime/).
Or use it to develop new Watchfaces, new Screens, or quickly iterate on the user interface.

For a history on how this simulator started and the challenges on its way visit the [original PR](https://github.com/InfiniTimeOrg/InfiniTime/pull/743).

## Get the Sources

Clone this repository and tell `git` to recursively download the submodules as well

```sh
git clone --recursive https://github.com/InfiniTimeOrg/InfiniSim.git
```

If you've already cloned the repository without the submodules (or you want to update them to the latest checked in version) run the following command:

```sh
git submodule update --init --recursive
```

## Build dependencies

- CMake
- SDL2 (provides the simulator window, handles mouse and keyboard input)
- Compiler (g++ or clang++)
- [lv_font_conv](https://github.com/lvgl/lv_font_conv#install-the-script) (for `font.c` generation since [InfiniTime#1097](https://github.com/InfiniTimeOrg/InfiniTime/pull/1097))
  - Note: requires Node.js v14.0.0 or later
- [Pillow](https://python-pillow.org/) (for `resource.zip` generation when `BUILD_RESOURCES=ON`, which is the default)
- optional: `libpng`, see `-DWITH_PNG=ON` cmake setting below for more info

On Ubuntu/Debian install the following packages:

```sh
sudo apt install -y cmake libsdl2-dev g++ npm libpng-dev
```

On Arch Linux the following packages are needed:

```sh
sudo pacman -S cmake sdl2 gcc npm libpng
```

On Fedora the following packages are needed:

```sh
sudo dnf install cmake SDL2-devel g++ npm patch perl libpng-devel
```

On OpenSUSE (Tumbleweed) the following packages are needed:

```sh
sudo zypper install cmake libSDL2-devel gcc-c++ gcc npm libpng16-devel patch
```

Then install the `lv_font_conv` executable to the InfiniSim source directory (will be installed at `node_modules/.bin/lv_font_conv`)

```sh
npm install lv_font_conv@1.5.2
```

When you want to create a `resource.zip` file then install the `pillow` Python library to the InfiniSim source directory (will be installed in `.venv/`)

```sh
python3 -m venv .venv
source .venv/bin/activate
pip install wheel Pillow
```

Optionally, depending on your distro, it may also serve the Pip package as official native installation packages:

On Ubuntu/Debian 

```sh
sudo apt install python3-pil
```

On OpenSUSE (Tumbleweed) 

```sh
sudo zypper install python311-Pillow
```

## Configure and Build

In the most basic configuration tell cmake to configure the project and build it with the following two commands:

```sh
cmake -S . -B build
cmake --build build -j4
```

The following configuration settings can be added to the first `cmake -S . -B build` call

- `-DInfiniTime_DIR=InfiniTime`: a full path to an existing InfiniTime repository checked out.
  Inside that directory the `src/libs/lvgl` submodule must be checked out as well.
  The default value points to the InfiniTime submodule in this repository.
- `-DMONITOR_ZOOM=1`: scale simulator window by this factor
- `-DBUILD_RESOURCES=ON`: enable/disable `resource.zip` creation, will be created in the `<build-dir>/resources` folder
- `-DWITH_PNG=ON`: enable/disable the screenshot to `PNG` support.
  Per default InfiniSim tries to use `libpng` to create screenshots in PNG format.
  This requires `libpng` development libraries as build and runtime dependency.
  Can be disabled with cmake config setting `-DWITH_PNG=OFF`.
- `-DENABLE_USERAPPS`: ordered list of user applications to build into InfiniTime.
  Values must be fields from the enumeration `Pinetime::Applications::Apps` and must be separated by a comma.
  Ex: `-DENABLE_USERAPPS="Apps::Timer, Apps::Alarm"`.
  The default list of user applications will be selected if this variable is not set.

## Run Simulator

When the build was successful the simulator binary can be started with

```sh
./build/infinisim
```

![Running Simulator](https://user-images.githubusercontent.com/9076163/151057090-66fa6b10-eb4f-4b62-88e6-f9f307a57e40.gif)

To hide the second simulator-status-window start the binary with the `--hide-status` option

```sh
./build/infinisim --hide-status
```

- Left mouse button: simulates your finger, just click to tap, click and drag to swipe
- Right mouse button: simulates the hardware button (for example turn the screen off or on again)

Using the keyboard the following events can be triggered:

- `r` ... enable ringing
- `R` ... disable ringing
- `m` ... let motor run for 100 ms
- `M` ... let motor run for 255 ms
- `n` ... send notification
- `N` ... clear new notification flag
- `b` ... connect Bluetooth
- `B` ... disconnect Bluetooth
- `v` ... increase battery voltage and percentage
- `V` ... decrease battery voltage and percentage
- `c` ... charging,
- `C` ... not charging
- `l` ... increase brightness level
- `L` ... lower brightness level
- `p` ... enable print lvgl memory usage to terminal
- `P` ... disable print memory usage
- `s` ... increase step count by 500 steps
- `S` ... decrease step count by 500 steps
- `h` ... set heartrate running, and on further presses increase by 10 bpm
- `H` ... stop heartrate
- `i` ... take screenshot
- `I` ... start/stop Gif screen capture
- `w` ... generate weather data
- `W` ... clear weather data

Additionally using the arrow keys the respective swipe gesture can be triggered.
For example pressing the UP key triggers a `SwipeUp` gesture.

## Littlefs-do helper

To help working with the SPI-raw file the tool `littlefs-do` is provided.
The SPI-raw file emulates the persistent 4MB storage available over the SPI bus on the PineTime.

```sh
$ ./littlefs-do --help
Usage: ./littlefs-do <command> [options]
Commands:
  -h, --help           show this help message for the selected command and exit
  -v, --verbose        print status messages to the console
  stat                 show information of specified file or directory
  ls                   list available files in 'spiNorFlash.raw' file
  mkdir                create directory
  rmdir                remove directory
  rm                   remove directory or file
  cp                   copy files into or out of flash file
  settings             list settings from 'settings.h'
  res                  resource.zip handling
```

### Resource loading

To load resource zip files into the SPI raw file for the simulator to use the `res load` command can be used.

```sh
$ ./littlefs-do res --help
Usage: ./littlefs-do res <action> [options]
actions:
  load res.zip         load zip file into SPI memory
Options:
  -h, --help           show this help message for the selected command and exit
```

## Licenses

This project is released under the GNU General Public License version 3 or, at your option, any later version.
The same license as [InfiniTime](https://github.com/InfiniTimeOrg/InfiniTime).

The simulator is based on [lv_sim_eclipse_sdl](https://github.com/lvgl/lv_sim_eclipse_sdl) project under the MIT license.
