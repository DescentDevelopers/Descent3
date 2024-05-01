![d3 (1)](https://github.com/DescentDevelopers/Descent3/assets/47716344/82ba0911-ee32-4565-84ee-b432c215ab95)

This is the Descent 3 open source engine, licensed under [GPL-3.0](https://github.com/DescentDevelopers/Descent3?tab=GPL-3.0-1-ov-file). It includes the '1.5' patch written by Kevin Bentley and Jeff Slutter several years ago and brought to a stable condition by the Descent community.

In order to use this, you must provide your own game files. See the **Usage** section for details.

## Version 1.5 Notes
There is no "release" yet. The current milestone is "1.5 Stable", which is meant to more or less be Descent 3 as it might have been if the 1.5 patch had made it to retail years ago. Artifacts can be downloaded from the [Actions](https://github.com/DescentDevelopers/Descent3/actions) tab.
The milestone needs testing on all platforms. Please report issues when found.

## Usage
Purchase Descent 3 from a reputable source and install it, then replace the main binary with the newly built `Descent3` binary under `${CMAKE_BINARY_DIR}/Descent3/*/Descent3[.exe]`.
See your platform below:

#### Windows
In the install folder, rename `main.exe` to `main.old` and take your built `Descent3.exe`, name it `main.exe`, and add it to the install folder.

#### MacOS
Right-click Descent3.app, click Show Package Contents. Back up your `Descent3` binary and drop your built `Descent3` binary into the install (Contents/MacOS) folder.

#### Linux
Back up your `Descent3` binary and drop your built `Descent3` binary into the install folder.

## Building
Build steps below assume you have already cloned the repository and entered it locally.

#### Building - Windows
Requires Visual Studio 2022 and C++ Tools (cmake and vcpkg)

Install and configure vcpkg:
```sh
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg && bootstrap-vcpkg.bat
setx VCPKG_ROOT="C:\path\to\vcpkg"
setx PATH=%VCPKG_ROOT%;%PATH%
```

Build Descent 3:
```sh
cmake --preset win -D ENABLE_LOGGER=[ON|OFF]
cmake --build --preset win --config [Debug|Release]
```

#### Building - MacOS
```sh
brew bundle install
cmake --preset mac -D ENABLE_LOGGER=[ON|OFF]
cmake --build --preset mac --config [Debug|Release]
```

#### Building - Linux (Ubuntu)
```sh
sudo apt update
sudo apt install -y --no-install-recommends ninja-build cmake g++ libsdl1.2-dev libsdl-image1.2-dev libncurses-dev zlib1g-dev libspdlog-dev
cmake --preset linux -D LOGGER=[ON|OFF]
cmake --build --preset linux --config [Debug|Release]
```

#### Building - Linux (Fedora)
```sh
sudo dnf update --refresh
sudo dnf install -y ninja-build cmake gcc-c++ SDL-devel SDL_image-devel ncurses-devel zlib-devel spdlog-devel
cmake --preset linux -D LOGGER=[ON|OFF]
cmake --build --preset linux --config [Debug|Release]
```

## Contributing
Anyone can contribute! We have an active Discord presence at [Descent Developer Network](https://discord.gg/GNy5CUQ). If you are interested in maintaining the project on a regular basis, please contact Kevin Bentley.
