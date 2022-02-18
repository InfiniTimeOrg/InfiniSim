# [InfiniSim](https://github.com/InfiniTimeOrg/InfiniSim)

[![Build InfiniSim LVGL Simulator](https://github.com/InfiniTimeOrg/InfiniSim/actions/workflows/lv_sim.yml/badge.svg)](https://github.com/InfiniTimeOrg/InfiniSim/actions/workflows/lv_sim.yml)

Simulator for [InfiniTime](https://github.com/InfiniTimeOrg/InfiniTime) project.

Experience the `InfiniTime` user interface directly on your PC, to shorten the time until you get your hands on a real [PineTime smartwatch](https://www.pine64.org/pinetime/).
Or use it to develop new Watchfaces, new Screens, or quickly iterate on the user interface.

For a history on how this simulator started and the challenges on its way visit the [original PR](https://github.com/InfiniTimeOrg/InfiniTime/pull/743).

## Build dependencies

- CMake
- SDL2 (provides the simulator window, handles mouse and keyboard input)
- Compiler (g++ or clang++)

On Ubuntu/Debian install the following packages:

```sh
sudo apt install -y cmake libsdl2-dev g++
```

On Arch Linux the following packages are needed:

```sh
sudo pacman -S cmake sdl2 gcc
```

## Get the Sources

Clone this repository and tell `git` to recursively download the submodules as well

```sh
git clone --recursive https://github.com/InfiniTimeOrg/InfiniSim.git
```

If you've already cloned the repository without the submodules (or you want to update them to the latest checked in version) run the following command:

```sh
git submodule update --init --recursive
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
- `N` ... clear all notifications
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

## Licenses

This project is released under the GNU General Public License version 3 or, at your option, any later version.
The same license as [InfiniTime](https://github.com/InfiniTimeOrg/InfiniTime).

The simulator is based on [lv_sim_eclipse_sdl](https://github.com/lvgl/lv_sim_eclipse_sdl) project under the MIT license.
