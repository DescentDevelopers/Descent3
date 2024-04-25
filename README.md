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
cmake --preset win -D LOGGER=[ON|OFF]
cmake --build --preset win --config [Debug|Release]
```

#### Building - MacOS
```sh
brew bundle install
cmake --preset mac -D LOGGER=[ON|OFF]
cmake --build --preset mac --config [Debug|Release]
```

#### Building - Linux
```sh
sudo apt update
sudo apt install -y --no-install-recommends ninja-build cmake g++ libgtest-dev \
    libsdl1.2-dev libsdl-image1.2-dev libncurses-dev libxext-dev zlib1g-dev
cmake --preset linux -D LOGGER=[ON|OFF]
cmake --build --preset linux --config [Debug|Release]
```

<details>
<summary>32-bit Linux & other distributions</summary>

**Ubuntu / Debian 32-bit build:**

Note: `libsdl1.2-dev` can not be installed for 64 and 32 bit at the same time, but on Ubuntu 24.04 and Debian 12
it can be replaced by `libsdl1.2-compat-dev:amd64` and `libsdl1.2-compat-dev:i386`.

```sh
sudo dpkg --add-architecture i386
sudo apt update
sudo apt install -y --no-install-recommends ninja-build cmake g++ g++-multilib libgtest-dev:i386 \
    libsdl1.2-dev:i386 libsdl-image1.2-dev:i386 libncurses-dev:i386 libxext6:i386 zlib1g-dev:i386

cmake --preset linux-32 -D LOGGER=[ON|OFF]
cmake --build --preset linux-32 --config [Debug|Release]

# when not using a preset:
cmake .. -GNinja -DCMAKE_TOOLCHAIN_FILE=../tools/toolchain-32-bit.cmake
ninja
```

**Fedora dependencies:**
```sh
dnf install -y --setopt=install_weak_deps=False \
    ninja-build sdl12-compat-devel SDL_image-devel ncurses-devel libXext-devel zlib-devel
# packages for 32-bit builds:
    sdl12-compat-devel.i686 SDL_image-devel.i686 ncurses-devel.i686 libXext-devel.i686 glibc-devel.i686 \
    mesa-libGLU-devel.i686 libglvnd-devel.i686 zlib-devel.i686
# for cmake calls, see above
```

**openSUSE Tumbleweed dependencies:**
```sh
zypper install -y cmake ninja gcc gcc-c++ sdl12_compat-devel libSDL_image-devel libXext-devel ncurses-devel zlib-devel
# (no 32bit build possible, missing sdl1.2 devel package)
# for cmake calls, see above
```
</details>

## Contributing
Anyone can contribute! We have an active Discord presence at [Descent Developer Network](https://discord.gg/GNy5CUQ). If you are interested in maintaining the project on a regular basis, please contact Kevin Bentley.
