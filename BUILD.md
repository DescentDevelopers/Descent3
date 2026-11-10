
# Building Descent 3 Open Source 

## Dependencies
The build process uses [**CMake**](https://cmake.org/) and, by default, [**Ninja**](https://ninja-build.org/). You must install these; the project cannot locate them for you. The source code also depends on third-party libraries that are not provided as part of the repository:
- [**SDL3**](https://wiki.libsdl.org/SDL3/FrontPage) which is used as the base to handle video, audio and input.
- [**cpp-httplib**](https://github.com/yhirose/cpp-httplib) >= 0.23 as a HTTP client to download levels.
- [**glm**](https://github.com/g-truc/glm) providing useful additions to OpenGL.
- [**plog**](https://github.com/SergiusTheBest/plog) for logging
- [**zlib**](https://www.zlib.net/) as a compression utility
- [**gtest**](https://github.com/google/googletest) for the unit tests, which are built by default (see [Building without the tests](#building-without-the-tests)).

You can supply these dependencies yourself via your system's library management tools, or the build system can locate the dependencies for you using [vcpkg](https://github.com/microsoft/vcpkg), a cross-platform dependency-management system developed by Microsoft. The official builds source their dependencies from vcpkg.

## Platforms tested in CI
Every pull request to `main` and every push to `main` is built in Debug and Release on the following platforms. The unit tests run on all of them except the ARM64 cross-compile. See [Running the tests](#running-the-tests).

| Platform | CI runner | Compiler |
|---|---|---|
| Windows x64 | Windows Server 2025 (`windows-2025-vs2026`) | MSVC from Visual Studio 2026 |
| macOS, universal arm64 and x86_64 binary | macOS 15 on Apple Silicon (`macos-15`) | Apple Clang 17 from Xcode 16.4 |
| Linux x64 | Ubuntu 26.04 (`ubuntu-26.04`) | GCC 15 |
| Linux ARM64, cross-compiled on x64 | Ubuntu 26.04 (`ubuntu-26.04`) | GCC 15 (`aarch64-linux-gnu`) |

The Linux builds use CMake 3.30. The Windows and macOS builds use the CMake version installed on the runner image.

## Installing and using vcpkg
* When building for Windows, vcpkg is already installed and configured when using any of the Visual Studio command prompts (either actual Command Prompt, or PowerShell).
* For Android, Linux or Mac (or non-standard-Windows) configurations, you will need to install vcpkg locally by cloning https://github.com/microsoft/vcpkg and setting your `VCPKG_ROOT` env var to the repository location. With this environment variable set, the build will automatically locate its dependencies.

## Building - Windows
1. **Install the prerequisite build tools.**

    Make sure that you have Git and Visual Studio 2026 with the “Desktop development with C++” workload and the “C++ MFC for x64/x86 (Latest MSVC)” component. If you don’t already have those installed or you aren’t sure, then open an elevated Command Prompt and run:
    <!--
    The following code block specifies the full path to the Visual Studio Installer because the Visual Studio Installer doesn’t add itself to the user’s Path. The installer is guaranteed to be in a specific location on 64-bit systems [1]. The installer will be in a different location on 32-bit systems [2], but Visual Studio 2026 doesn’t support 32-bit systems [3] so we can ignore that detail.

    [1]: <https://learn.microsoft.com/en-us/visualstudio/install/use-command-line-parameters-to-install-visual-studio?view=visualstudio>
    [2]: <https://github.com/microsoft/vswhere/wiki#installing>
    [3]: <https://learn.microsoft.com/en-us/visualstudio/releases/2026/vs-system-requirements>
    -->
    ```batch
    winget install Git.Git Microsoft.VisualStudio.Community

    "%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\setup.exe" modify^
        --passive^
        --channelId VisualStudio.18.Stable^
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
```

Now, you are ready for cross-compilation. Create a new cross-compilation build directory and configure the project in it, but this time specify the location of the HogMaker native executable just built, as well as the [toolchain file](https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html#id14) location. This enables the cross-compilation environment. An example toolchain file is provided for a Linux ARM64 build at `cmake/toolchains/linux-aarch64-gcc-toolchain.cmake`. The custom toolchain system can be used in combination with VCPKG to build all dependencies for the target system.

```shell
mkdir build-cross
cd build-cross
cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/MyToolChain.cmake -DHogMaker_DIR=../build-native/ ..
cmake --build .
```

## Running the tests

The unit tests use [GoogleTest](https://github.com/google/googletest) and are run with [CTest](https://cmake.org/cmake/help/latest/manual/ctest.1.html). They are built by default, as part of the normal build. vcpkg installs GoogleTest along with the other dependencies; if you manage the dependencies yourself, install it as well.

1. **Configure**, using the preset for your platform (`win`, `mac` or `linux`):

    ```sh
    cmake --preset linux
    ```

2. **Build.**

    ```sh
    cmake --build --preset linux --config [Debug|Release]
    ```

3. **Run every test.**

    ```sh
    ctest --preset linux -C [Debug|Release]
    ```

Each GoogleTest case is registered as its own CTest test, named `Suite.Test`. Some useful `ctest` options:
- `--output-on-failure` prints the output of the tests that fail.
- `-j <N>` runs up to N tests in parallel.
- `-R <regex>` runs only the tests whose names match, for example `ctest --preset linux -C Debug -R CFile`.

Most of the tests are in one executable, `d3_tests` (for example `builds/linux/tests/Debug/d3_tests`). The others are in executables of their own, because something in them cannot share one with the rest; each says why next to its `d3_test_executable()` in CMake. They are the online lobby modules (`netcon_tests`), the game clock that `ENABLE_MOCK_CLOCK` did not pick (`clock_tests`), the netgame framework (`dmfc_tests`) and each netgame mode (`netgame_<mode>_tests`), the script runtime (`dallas_tests`) and each script module (`aiscripts_<module>_tests`), and on Windows the editor (`editor_tests`).

You can also run a test executable directly, and pass it GoogleTest options such as `--gtest_filter=<pattern>`. CTest runs every test case in a process of its own, so a test never sees what another one left behind; when you run an executable directly, its tests share one process.

### What the test executables guard against

Every test executable runs with the shared `main()` in `tests/support/test_main.cpp`, which keeps the game's own failure paths from hiding a failure or stalling an unattended run:
- `Error()` ends the program with `exit(0)`, which CTest would count as a pass. Exiting before the tests have finished fails the run instead.
- A failed `ASSERT()` or `Int3()` fails the current test. A test that triggers them on purpose declares a `d3_test::ExpectGameAssertions` while it does.
- The message boxes of `Error()`, `Int3MessageBox()`, `AssertionFailed()` and `OutrageMessageBox()` are never shown; each one fails the current test.

### Matching CI

CI configures with `-DBUILD_TESTING=ON -DENABLE_LOGGER=ON -DUSE_EXTERNAL_PLOG=ON -DBUILD_EDITOR=ON`, adds `-DENABLE_MOCK_CLOCK=ON` to Debug builds, and runs `ctest` on both the Debug and the Release build. To reproduce a CI failure locally, use the same options and configuration. Release builds compile out assertions, so a test can behave differently in Debug and in Release.

Tests of code that is only built on some platforms, or only with `BUILD_EDITOR=ON`, are only built and run there too. The editor is Windows-only, so its tests only run on Windows with `BUILD_EDITOR=ON`.

### Adding tests

Tests for a module live in its `tests` directory, which the module's `CMakeLists.txt` adds inside `if(BUILD_TESTING)`. That directory's `CMakeLists.txt` compiles them with `d3_test_sources()` from `cmake/D3Testing.cmake`, normally into `d3_tests`:

```cmake
d3_test_sources(cfile_tests TO d3_tests
  SOURCES cfile_tests.cpp cfile_io_tests.cpp
  LIBRARIES cfile
)
```

CTest finds every test case in the executable, and CI runs them with no further changes, because the workflow runs `ctest` over the whole build rather than a list of tests. A test that cannot share `d3_tests` gets an executable of its own from `d3_test_executable()`, with a comment saying why.

Put helpers that only one module's tests use in the directory's `support` subdirectory, and keep the test files themselves to tests. Helpers that the tests of several modules need are in `tests/support/d3_test/`: temporary directories and whole-file reads and writes (`files.h`), byte images in the game's file formats (`byte_writer.h`), expected game assertions (`game_assertions.h`) and the game clock (`game_clock.h`).

The game libraries reference parts of the `Descent3` executable, which tests cannot link, and of the OpenGL renderer, which needs a window. `d3_tests` links stand-ins for both: `d3_test_game` (`tests/support/game/`) provides the executable's globals and functions, and `d3_test_renderer` (`tests/support/renderer/`) runs the renderer's real device-independent 3D pipeline on a fake backend that draws nothing. Their controls are in `d3_test/game.h` and `d3_test/renderer.h`. When a library under test starts to use another part of the executable or the renderer, add it there rather than in the module's tests.

## Build Options

The Descent3 build can be customized by [setting CMake variables on the command line](https://cmake.org/cmake/help/latest/manual/cmake.1.html#cmdoption-cmake-D) during its "Configuration" phase (the command without the `--build` option). To set a variable, you prepend the variable name with `-D` and then append the value, all as one single parameter. For example:
```sh
cmake --preset linux -DENABLE_LOGGER=ON
```

**NOTE:** CMake variables, or more technically _CMake cache entries_, will persist in their values until they are explicitly cleared. So, if you set a variable and then run another CMake command _without_ that variable specified, the variable will still be set. Variables must be explicitly unset, or the `builds/` directory cleaned, in order to be cleared.

You can also use the [cmake-gui](https://cmake.org/cmake/help/latest/manual/cmake-gui.1.html) front-end to easily set and view CMake cache variable values, and generate build

| Option                    | Description                                                                                                                                                                                                                                                                                                                                                                                                                                                    | Default                                                                                      |
|---------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------|
| `CMAKE_BUILD_TYPE`        | `Debug` builds are generally larger, slower and contain extra correctness checks that will validate game data and interrupt gameplay when problems are detected.<br>`Release` builds are optimized for size and speed and do not include debugging information, which makes it harder to find problems. The build type can also be set using the `--config` argument with a preset.                                                                            | `Debug`                                                                                      |
| `BUILD_EDITOR`            | _(Windows-only)_ Build internal editor.                                                                                                                                                                                                                                                                                                                                                                                                                        | `OFF`                                                                                        |
| `BUILD_TESTING`           | Build the unit tests. Requires GTest; see [Building without the tests](#building-without-the-tests).                                                                                                                                                                                                                                                                                                                                                           | `ON`                                                                                         |
| `DEFAULT_ADDITIONAL_DIRS` | A semi-colon separated list of paths that Descent 3 will use as read-only base directories (see [USAGE.md’s Base directories section][1]). Each item in this list is placed between quotation marks in order to form a C++ expression. This will cause issues if the items in the list are not properly escaped. You can use C++ escape sequences in order to embed special characters (like `\`, `"` and `;`) in paths. Example: `C:\\Games\\Descent3\\;D:\\` | A list with one item in it. The item is an empty string.                                     |
| `ENABLE_LOGGER`           | Enable logging to the terminal.                                                                                                                                                                                                                                                                                                                                                                                                                                | `OFF`                                                                                        |
| `ENABLE_MEM_RTL`          | Enable Real-time library memory management functions (disable to verbose memory allocations).                                                                                                                                                                                                                                                                                                                                                                  | `ON`                                                                                         |
| `ENABLE_MOCK_CLOCK`       | Replace the real-time clock with a mock one for automated testing. Every game frame advances the clock by exactly 16ms (so `-framecap` is not allowed), while still waiting for real time to pass so the game plays at normal speed.                                                                                                                                                                                                                           | `OFF`                                                                                        |
| `FORCE_COLORED_OUTPUT`    | Always produce ANSI-colored compiler warnings/errors (GCC/Clang only; esp. useful with Ninja).                                                                                                                                                                                                                                                                                                                                                                 | `OFF`                                                                                        |
| `FORCE_PORTABLE_INSTALL`  | Install all files into local directory defined by `CMAKE_INSTALL_PREFIX`.                                                                                                                                                                                                                                                                                                                                                                                      | `ON`                                                                                         |
      | `OFF`                                                                                        |
| `USE_VCPKG`               | Explicitly control whether or not to use vcpkg for dependency resolution. `ON` requires the environment variable `VCPKG_ROOT` to be set.                                                                                                                                                                                                                                                                                                                       | Determined by the existence of `VCPKG_ROOT` in the environment: If it exists, vcpkg is used. |
| `CODESIGN_IDENTITY`       | Sets the macOS code signing identity. If set to something besides the empty string, then the dynamic libraries put into the hog files will be signed using this identity.                                                                                                                                                                                                                                                                                      | The empty string, `""`.                                                                      |

### Building without the tests

The unit tests are built by default, which is why GoogleTest is one of the [dependencies](#dependencies). If you really don't want to build them, configure with `-DBUILD_TESTING=OFF`, and you won't need GoogleTest; if you use vcpkg, it still installs GoogleTest along with the other dependencies, because the project's `vcpkg.json` lists it.

[1]: ./USAGE.md#base-directories
