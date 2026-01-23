
# Building Descent 3 Open Source 

## Dependencies
The build process uses [**CMake**](https://cmake.org/) and, by default, [**Ninja**](https://ninja-build.org/). You must install these; the project cannot locate them for you. The source code also depends on third-party libraries that are not provided as part of the repository:
- [**SDL3**](https://wiki.libsdl.org/SDL3/FrontPage) which is used as the base to handle video, audio and input.
- [**cpp-httplib**](https://github.com/yhirose/cpp-httplib) as a HTTP client to download levels.
- [**glm**](https://github.com/g-truc/glm) providing useful additions to OpenGL.
- [**plog**](https://github.com/SergiusTheBest/plog) for logging
- [**zlib**](https://www.zlib.net/) as a compression utility
- [**gtest**](https://github.com/google/googletest) (optional) for testing.

You can supply these dependencies yourself via your system's library management tools, or the build system can locate the dependencies for you using [vcpkg](https://github.com/microsoft/vcpkg), a cross-platform dependency-management system developed by Microsoft. The official builds source their dependencies from vcpkg.

## Installing and using vcpkg
* When building for Windows, vcpkg is already installed and configured when using any of the Visual Studio command prompts (either actual Command Prompt, or PowerShell).
* For Android, Linux or Mac (or non-standard-Windows) configurations, you will need to install vcpkg locally by cloning https://github.com/microsoft/vcpkg and setting your `VCPKG_ROOT` env var to the repository location. With this environment variable set, the build will automatically locate its dependencies.

## Building - Windows
1. **Install the prerequisite build tools.**

    Make sure that you have Git and Visual Studio 2022 with the “Desktop development with C++” workload and the “C++ MFC for latest v143 build tools (x86 & x64)” component. If you don’t already have those installed or you aren’t sure, then open an elevated Command Prompt and run:
    <!--
    The following code block specifies the full path to the Visual Studio Installer because the Visual Studio Installer doesn’t add itself to the user’s Path. The installer is guaranteed to be in a specific location on 64-bit systems [1]. The installer will be in a different location on 32-bit systems [2], but Visual Studio 2022 doesn’t support 32-bit systems [3] so we can ignore that detail.

    [1]: <https://learn.microsoft.com/en-us/visualstudio/install/use-command-line-parameters-to-install-visual-studio?view=vs-2022>
    [2]: <https://github.com/microsoft/vswhere/wiki#installing>
    [3]: <https://learn.microsoft.com/en-us/answers/questions/1689898/does-visual-studio-build-tools-2022-support-32-bit>
    -->
    ```batch
    winget install Git.Git Microsoft.VisualStudio.2022.Community

    "%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\setup.exe" modify^
        --passive^
        --channelId VisualStudio.17.Release^
        --productId Microsoft.VisualStudio.Product.Community^
        --add Microsoft.VisualStudio.Workload.NativeDesktop;includeRecommended^
        --add Microsoft.VisualStudio.Component.VC.ATLMFC
    ```

    **NOTE:**
    Builds _must_ be performed in one of the Visual Studio-provided **x64 Native Tools** command prompts ("x64 Native Tools Command Prompt" or "x64 Native Tools PowerShell"), _not_ a standard Command Prompt or PowerShell. The VS prompts will already have vcpkg installed and configured for use, so no dependency management is needed.

2. **Clone the Descent3 source code.**

    Open a "x64 Native Tools Command Prompt" or "x64 Native Tools PowerShell" and run:
    ```batch
    git clone https://github.com/DescentDevelopers/Descent3
    ```

3. **Build Descent3.**

    ```batch
    cd Descent3
    cmake --preset win
    cmake --build --preset win --config [Debug|Release]
    ```
    See [Build Options](#build-options) below for more information on `Debug` vs `Release`.

Once CMake finishes, the built files will be put in `builds\win\build\Debug` or `builds\win\build\Release`.

## Building - macOS

1. **Install the prerequisite build tools.**

    * Make sure that [Xcode](https://developer.apple.com/xcode) is installed.
    * Make sure that [Homebrew](https://brew.sh) is installed.

2. **Acquire the library dependencies.**

    * To install the library dependencies:
        ```sh
        git clone https://github.com/microsoft/vcpkg
        cd vcpkg
        ./bootstrap-vcpkg.sh
        cd -
        export VCPKG_ROOT="$PWD/vcpkg"
        ```
        **NOTE:**
        You will need `$VCPKG_ROOT` defined in the environment for all build runs. It is a good idea to set this in your `.bashrc` or equivalent.
    * Create a custom VCPKG triplet that allows the mac build to run and create a universal binary.
      ```sh
      cd "$VCPKG_ROOT"/triplets
      cp ./arm64-osx.cmake ./community/universal-osx.cmake
      sed -i '' 's/^set(VCPKG_OSX_ARCHITECTURES.*$/set(VCPKG_OSX_ARCHITECTURES "arm64;x86_64")/' ./community/universal-osx.cmake
      ```

3. **Clone the Descent3 source code.**

    ```sh
    git clone https://github.com/DescentDevelopers/Descent3
    ```

4. **Build Descent3.**

    * Install the build tools.
      ```sh
      brew bundle install
      ```
    * Compile Descent 3.
      ```sh
      cd Descent3
      cmake --preset mac
      cmake --build --preset mac --config [Debug|Release]
      ```
      See [Build Options](#build-options) below for more information on `Debug` vs `Release`.

Once CMake finishes, the built files will be put in `builds/mac/build/Debug` or `builds/mac/build/Release`.

## Building - Linux
1. **Install the prerequisite build tools.**

    * APT users (Debian, Ubuntu)
        ```sh
        sudo apt update
        sudo apt install -y --no-install-recommends git ninja-build cmake g++ ca-certificates curl zip unzip tar
        ```
    * DNF users (Red Hat, Fedora)
        ```sh
        sudo dnf update --refresh
        sudo dnf install -y git ninja-build cmake gcc-c++
        ```

2. **Acquire the library dependencies.**

    * We recommend using VCPKG to manage library dependencies.
        1. Clone vcpkg:
            ```sh
            git clone https://github.com/microsoft/vcpkg
            cd vcpkg
            ./bootstrap-vcpkg.sh
            cd -
            export VCPKG_ROOT="$PWD/vcpkg"
            ```
            **NOTE:**
            You will need `$VCPKG_ROOT` defined in the environment for all build runs. It is a good idea to set this in your `.bashrc` or equivalent.
        2. Install vcpkg-needed build tools and dependencies:
            * APT users
                ```sh
                sudo apt install -y --no-install-recommends curl pkg-config autoconf automake libtool libltdl-dev make python3-jinja2 libx11-dev libxft-dev libxext-dev libwayland-dev libxkbcommon-dev libegl1-mesa-dev libibus-1.0-dev libasound2-dev libpulse-dev libaudio-dev libjack-dev libsndio-dev libxcursor-dev libxfixes-dev libxi-dev libxrandr-dev libxss-dev
                ```
            * DNF users
                ```sh
                sudo dnf install -y autoconf automake libtool perl-open perl-FindBin python-jinja2 libX11-devel libXft-devel libXext-devel wayland-devel libxkbcommon-devel mesa-libEGL-devel ibus-devel alsa-lib-devel pulseaudio-libs-devel
                ```
    * If you would like to manage the code dependencies yourself, install libraries and headers for the dependencies listed [above](#dependencies). SDL3 still being new at the time of writing, it may not be packaged by all popular non rolling-release Linux distributions yet.

3. **Clone the Descent3 source code.**

    ```sh
    git clone https://github.com/DescentDevelopers/Descent3
    ```

4. **Build Descent3.**

    ```sh
    cd Descent3
    cmake --preset linux
    cmake --build --preset linux --config [Debug|Release]
    ```

    See [Build Options](#build-options) below for more information on `Debug` vs `Release`.

Once CMake finishes, the built files will be put in `builds/linux/build/Debug` or `builds/linux/build/Release`.

## Cross-compilation

In order to cross-compile Descent3 to another platform (for example, Linux ARM64), you'll need to build auxiliary
tools natively which are needed to build data files. First create build-native directory and configure project in it:

```shell
mkdir build-native
cd build-native
cmake ..
cmake --build . --target HogMaker
cmake --install . --prefix . --component HostTools
```

Feel free to omit the `--target HogMaker` if you want to build the entire Descent3 project, or the `cmake --install` step if you don't mind finding the HogMaker build location yourself (it will vary between build generators like Ninja vs Make).

Once you have a HogMaker binary runnable on your build system, you are ready for cross-compilation. Create a new cross-compilation build directory and configure the project in it, but this time specify the location of the HogMaker native executable just built, as well as the [toolchain file](https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html#id14) location. This enables the cross-compilation environment. An example toolchain file is provided for a Linux ARM64 build at `cmake/toolchains/linux-aarch64-gcc-toolchain.cmake`. The custom toolchain system can be used in combination with VCPKG to build all dependencies for the target system.

```shell
mkdir build-cross
cd build-cross
cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/MyToolChain.cmake -DHOGMAKER_BINARY=../build-native/HogMaker ..
cmake --build .
```

## Build Options

The Descent3 build can be customized by [setting CMake variables on the command line](https://cmake.org/cmake/help/latest/manual/cmake.1.html#cmdoption-cmake-D) during its "Configuration" phase (the command without the `--build` option). To set a variable, you prepend the variable name with `-D` and then append the value, all as one single parameter. For example:
```sh
cmake --preset linux -DBUILD_TESTING=ON
```

**NOTE:** CMake variables, or more technically _CMake cache entries_, will persist in their values until they are explicitly cleared. So, if you set a variable and then run another CMake command _without_ that variable specified, the variable will still be set. Variables must be explicitly unset, or the `builds/` directory cleaned, in order to be cleared.

You can also use the [cmake-gui](https://cmake.org/cmake/help/latest/manual/cmake-gui.1.html) front-end to easily set and view CMake cache variable values, and generate build

| Option                    | Description                                                                                                                                                                                                                                                                                                                                                                                                                                                    | Default                                                                                      |
|---------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------|
| `CMAKE_BUILD_TYPE`        | `Debug` builds are generally larger, slower and contain extra correctness checks that will validate game data and interrupt gameplay when problems are detected.<br>`Release` builds are optimized for size and speed and do not include debugging information, which makes it harder to find problems. The build type can also be set using the `--config` argument with a preset.                                                                            | `Debug`                                                                                      |
| `BUILD_EDITOR`            | _(Windows-only)_ Build internal editor.                                                                                                                                                                                                                                                                                                                                                                                                                        | `OFF`                                                                                        |
| `BUILD_TESTING`           | Enable testing. Requires GTest.                                                                                                                                                                                                                                                                                                                                                                                                                                | `OFF`                                                                                        |
| `DEFAULT_ADDITIONAL_DIRS` | A semi-colon separated list of paths that Descent 3 will use as read-only base directories (see [USAGE.md’s Base directories section][1]). Each item in this list is placed between quotation marks in order to form a C++ expression. This will cause issues if the items in the list are not properly escaped. You can use C++ escape sequences in order to embed special characters (like `\`, `"` and `;`) in paths. Example: `C:\\Games\\Descent3\\;D:\\` | A list with one item in it. The item is an empty string.                                     |
| `ENABLE_LOGGER`           | Enable logging to the terminal.                                                                                                                                                                                                                                                                                                                                                                                                                                | `OFF`                                                                                        |
| `ENABLE_MEM_RTL`          | Enable Real-time library memory management functions (disable to verbose memory allocations).                                                                                                                                                                                                                                                                                                                                                                  | `ON`                                                                                         |
| `FORCE_COLORED_OUTPUT`    | Always produce ANSI-colored compiler warnings/errors (GCC/Clang only; esp. useful with Ninja).                                                                                                                                                                                                                                                                                                                                                                 | `OFF`                                                                                        |
| `FORCE_PORTABLE_INSTALL`  | Install all files into local directory defined by `CMAKE_INSTALL_PREFIX`.                                                                                                                                                                                                                                                                                                                                                                                      | `ON`                                                                                         |
      | `OFF`                                                                                        |
| `USE_VCPKG`               | Explicitly control whether or not to use vcpkg for dependency resolution. `ON` requires the environment variable `VCPKG_ROOT` to be set.                                                                                                                                                                                                                                                                                                                       | Determined by the existence of `VCPKG_ROOT` in the environment: If it exists, vcpkg is used. |
| `CODESIGN_IDENTITY`       | Sets the macOS code signing identity. If set to something besides the empty string, then the dynamic libraries put into the hog files will be signed using this identity.                                                                                                                                                                                                                                                                                      | The empty string, `""`.                                                                      |

[1]: ./USAGE.md#base-directories
