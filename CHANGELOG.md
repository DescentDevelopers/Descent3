# Descent 3 Open Source - Changelog

## 1.6.0 - TBD

## 1.5.0 - August 2024

This is the first release of the community port of Descent 3, release as open source software under the GPLv3 license on April 16th 2024.

This release is the work of 28 individual contributors over 4 months. It fills in the gaps left in the never-released 1.5 patch of the game, and makes it more accessible and convinient to run on modern devices.

Our focus for this release has been compatibility with modern CPUs and developer experience improvement, while keeping features mostly identical to the code initially released. The next releases will be given more latitude to improve in-game player experience.

Descent 3 now runs on 64-bit Linux, Windows and MacOS using SDL2, on AMD64 as well as ARM64 CPUs. 32-bit support has been dropped.


### Added 
 - New HogMaker utility script to build platform-specific HOG files containing level scripts (#122, #143, #153, #275, #277, #298)
 - Set up gtest as a unit testing framework (#171, #251)
 - Show git hash in main menu (#224, #440, #451)


### Changed
 - Use SDL2 on all platforms (#241, #457). Upon code release, the Windows was using an old version of DirectX for input and window management, and Linux and MacOS were using SDL1.2 from the Loki port of the game. All operating systems now use a common SDL2 back-end.
 - Use a configuration file instead of the registry to store game settings on Windows, to be consistent with the other OSes. 
 - Support 64-bit builds on all platforms (#59, #158, #178, #179, #192, #233, #272, #354, #356, #357, #385, #397). 32bit compatibility for existing savefiles and demos is not functional.
 - Eliminate assembly code specific to x86 (#72).
 - Add icons to Windows and MacOS bundles (#498)
 - Replace OS-specific filesystem functions by C++17 std::filesystem in the CFile and DDIO modules (#383, #443, #475, #484, #486, #499).
 - Use standard temporary directory for cache files instead of game data directory (#500)
 - Implement new byteswap functions (#85, #165, #172)
 - Build the internal level editor using MFC UI framework. The executable is not included in this release because it is still very unstable and not usable, and can only run on Windows (32-bit) (#397, #476)
 - Render the game to an OpenGL Frame Buffer Object at the game's resolution (#437)
 - Format code-base using clang-format (#13, #16, #19, #24, #51, #438)
 - Use upstream libacm 1.3 for ACM audio decoding (#135, #191, #221) 
 - Implement a new MVE video decoder based on the open source implementation used in D2X (#289). MVE movies for intro and cutscenes can now be played on all platforms. OGV movies shipped with the Linux game cannot be played yet.
 - Use the "push" model of filling sound buffer for MVE playback (#491)
 - Write a new implementation of the md5 algorithm used to compute checksums. (#316)
 - Use VCPKG to build third-party dependencies not included in the code tree, such as SDL2 and zlib (#137, #430, #469, #512).
 - Set up automated build and testing using Github Actions for Linux (clang/gcc), Windows (MSVC) and MacOS (apple-clang, x64 & arm64) (#14, #28, #29, #45, #50, #60, #123, #133, #139, #155, #166, #177, #182)
 - Modernize build system: require CMake 3.20, use present-day CMake practices (#20, #63, #77, #86, #99, #104, #127, #131, #144, #220, #225, #227, #235, #256, #318, #340, #400, #407, #422, #460, #463, #482, #504)
 - Ensure compatibility with popular compilers, convert misc. pieces of code to modern C++ (#17, #20, #23, #40, #54, #58, #70, #108, #304, #328, #344, #358, #388, #405, #416)
 - Mark relevant variables and functions as static (#185)
 - Update the module system: isolate code for independant sub-modules in separate directories and CMake targets (#76, #89, #91, #195, #367, #374, #448, #470, #502)
 - Document build procedure and usage on all platforms (#116, #118, #119, #124, #147, #154, #170, #174, #175, #184, #190, #204, #210, #211, #222, #259, #261, #270, #285, #297, #384, #413, #435)
 - Unifiy Windows & Unix command line arguments: --foo, -foo and +foo is accepted on any OS (#199)
 - Refactor CleanupStr in misc/pstring.cpp (#207)
 - Change non-specific types (e.g. unsigned int) to their stdint specified counterparts (e.g. `uint32_t`) (#268, #464)
 - Support case-insensitive loading of content files (#294)
 - Build TCP/IP and PXO multiplayer modules (#296, #302, #420, #424, #432)
 - Refactor renderer: move non-OpenGL specific code to a base file, abstract triangle rendering code (#315, #323, #336)
 - Save screenshots in PNG format instead of the legacy TGA format (#325, #348)
 - Increment max cells value to increase render distance for outdoor terrain (#338)
 - Use chrono C++11 steady_clock for time measurement (#394)
 - Enable real-time library memory functions by default (#436, #445, #456)
 - Change `__LINUX__` defines to `POSIX` for general OS checks (#492)


### Fixed
 - Cleanup warnings issued by modern compilers (#57, #74, #82, #87, #89, #109, #117, #218, #349, #387, #428, #434, #478, #507)
 - Replace sprintf by snprintf(#78, #252, #266)
 - Add object unlink sanity check (#140)
 - Use a static background image in the main menu if the video file is not present (#152)
 - Add virtual default constructors on abstract classes (#219)
 - Fix cursor not being grabbed on macOS (#223)
 - Fix detection of mercenary installation, where hogfiles are not the same for CD and digital installation (#260)
 - Fix some out-of-bounds array access (#263)
 - Fix for Level 5 script;  Make CustomText2 centered on Y axis (#307)
 - Fix a crash when calling `quit` in the server console in dedicated server mode (#370)
 - Fix a game crash when launching with `-useexedir` argument (#372)
 - `-nomousegrab` command-line argument now works correctly (#401)
 - Only close SDL audio if it's open (#409)
 - Do not segfault when critical game files are missing (#414)
 - Avoid server crash during client connection (#431)
 - Fix `-aspect` aspect ratio argument for ship selection screen (#454)
 - Remove usage of non-glibc `GLOB_PERIOD` (#462)
 - Fix endian issues with iff importer (#481)
 - Fix a crash with `-nointro` command-line argument
 - Support `-nosound` option for movie playbacks
 - Use UTF-8 in more places on Windows using a manifest file (#494) 


### Removed
 - Remove unused supporting modules for obsolete hardware or unused features: Aureal 3D sound cards (#345), GameGauge performance benchmark (#238, #337), CD-ROM asset loading (#452, #455), Intellivibe interface for VRF Tactile Feedback System (#453), AngelScript (#15), legacy Macintosh (#188), IPX/SPX network protocol support (#237), Rock'n Ride (#292)
 - Unused library dynamic loading headers (#283)
 - References to libGLU (#246)
 - References to X11 and glX (#247, #244)
 - Unused mono debug code (#410)
 - Unused software renderer and d3m files (#305, #314, #321)
 - DDAccess.h header and access ring definitions (#342)
 - DMFC_DLL definition (#441)
 - Misc. removal of unfinished or unused code (#21, #55, #189, #197, #234, #245, #299, #303, #334, #396, #408)
 