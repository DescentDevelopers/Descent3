cmake_minimum_required(VERSION 3.20)  # For using CMAKE_<LANG>_BYTE_ORDER

# set default cmake build type to Debug (None Debug Release RelWithDebInfo MinSizeRel)
if(NOT CMAKE_BUILD_TYPE AND NOT DEFINED ENV{CMAKE_BUILD_TYPE})
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "default build type")
endif()

# toolchain setup for vcpkg must be done before the 'project' call
set(USE_VCPKG "DEFAULT" CACHE STRING "Use vcpkg for dependency management. DEFAULT defers to existence of $VCPKG_ROOT environment variable.")
set_property(CACHE USE_VCPKG PROPERTY STRINGS "DEFAULT" "ON" "OFF")

if(NOT USE_VCPKG STREQUAL "OFF")
  if(DEFINED ENV{VCPKG_ROOT})

    if (CMAKE_TOOLCHAIN_FILE)
      cmake_path(ABSOLUTE_PATH CMAKE_TOOLCHAIN_FILE NORMALIZE OUTPUT_VARIABLE VCPKG_CHAINLOAD_TOOLCHAIN_FILE)
    endif()

    set(VCPKG_TOOLCHAIN_FILE "scripts/buildsystems/vcpkg.cmake")
    cmake_path(ABSOLUTE_PATH VCPKG_TOOLCHAIN_FILE BASE_DIRECTORY $ENV{VCPKG_ROOT} NORMALIZE OUTPUT_VARIABLE CMAKE_TOOLCHAIN_FILE)

    if(CMAKE_TOOLCHAIN_FILE STREQUAL VCPKG_CHAINLOAD_TOOLCHAIN_FILE)
      # prevent endless recursion
      unset(VCPKG_CHAINLOAD_TOOLCHAIN_FILE)
    endif()
  elseif(NOT USE_VCPKG STREQUAL "DEFAULT")
    message(WARNING "USE_VCPKG=${USE_VCPKG} but ENV{VCPKG_ROOT} not set; will use system-provided libraries. Did you forget to set VCPKG_ROOT in your environment?")
  endif()
endif()

project(Descent3
  LANGUAGES C CXX
  VERSION 1.6.0
)

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "In-source builds are not supported. Please run CMake with a different build path.")
endif()

if(POLICY CMP0177) # install() DESTINATION path are normalized
  cmake_policy(SET CMP0177 NEW)
endif()

option(BUILD_TESTING "Enable testing. Requires GTest." OFF)
option(ENABLE_LOGGER "Enable logging to the terminal" OFF)
option(ENABLE_MEM_RTL "Enable Real-time library memory management functions (disable to verbose memory allocations)" ON)
option(FATAL_GL_ERRORS "Check OpenGL calls and raise exceptions on errors." OFF)
option(FORCE_COLORED_OUTPUT "Always produce ANSI-colored compiler warnings/errors (GCC/Clang only; esp. useful with Ninja)." OFF)
option(FORCE_PORTABLE_INSTALL "Install all files into local directory defined by CMAKE_INSTALL_PREFIX" ON)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  option(BUILD_EDITOR "Build internal editor" OFF)
endif()

set(CODESIGN_IDENTITY "" CACHE STRING "Sets the macOS code signing identity. If set to something besides the empty string, then the dynamic libraries put into the hog files will be signed using this identity.")
function (macos_sign target)
  if(DEFINED CODESIGN_IDENTITY AND NOT "${CODESIGN_IDENTITY}" STREQUAL "")
    message(STATUS "Code signing ${target}")
    add_custom_command(TARGET ${target} POST_BUILD
            COMMAND codesign --verbose --sign "${CODESIGN_IDENTITY}" --force --timestamp --deep -o runtime $<TARGET_FILE:${target}>)
  endif()
endfunction()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(MSVC)
  add_compile_options(/source-charset:UTF-8 /execution-charset:UTF-8)
else()
  add_compile_options(-finput-charset=UTF-8)
  # Unfortunately, Clang doesn’t support -fexec-charset yet so this next part
  # is GCC only. Luckily, Clang defaults to using UTF-8 for the execution
  # character set [1], so we’re fine. Once Clang gets support for
  # -fexec-charset, we should probably start using it.
  #
  # [1]: <https://discourse.llvm.org/t/rfc-enabling-fexec-charset-support-to-llvm-and-clang-reposting/71512>
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-fexec-charset=UTF-8)
  endif()
endif()

if(FORCE_COLORED_OUTPUT)
  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.24)
    set(CMAKE_COLOR_DIAGNOSTICS ON)
  else()
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      add_compile_options(-fdiagnostics-color=always)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      add_compile_options(-fcolor-diagnostics)
    endif()
  endif()
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(GNUInstallDirs)
if(FORCE_PORTABLE_INSTALL)
  set(CMAKE_INSTALL_BINDIR ".")
  set(CMAKE_INSTALL_LIBDIR ".")
  set(CMAKE_INSTALL_DATADIR ".")
  set(CMAKE_INSTALL_DOCDIR ".")

  set(D3_DATADIR "${CMAKE_INSTALL_DATADIR}")
else()
  set(CMAKE_INSTALL_DATADIR "${CMAKE_INSTALL_DATADIR}/descent3")
  # On system install files will go into /usr/share/descent3
  set(D3_DATADIR "${CMAKE_INSTALL_FULL_DATADIR}/descent3")
endif()

# Get info about multi-config environment
get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
# Properly determine output directory for generated files
if(IS_MULTI_CONFIG)
  set(D3_GENERATED_FILES_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/$<CONFIG>")
else ()
  set(D3_GENERATED_FILES_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")
endif()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/installed" CACHE PATH "Default install path" FORCE)
endif()

if(CMAKE_CXX_BYTE_ORDER STREQUAL "BIG_ENDIAN")
  message(STATUS "Big Endian system detected.")
  add_compile_definitions("OUTRAGE_BIG_ENDIAN")
endif()


# 64 bit machines have a different game checksum than 32 bit machines
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
  add_definitions(-DCHECKSUM=2273889835UL)
else()
  add_definitions(-DCHECKSUM=2273873307UL)
endif()

if(BUILD_TESTING)
  find_package(GTest REQUIRED)
  enable_testing()
  include(GoogleTest)
  add_subdirectory(tests)
endif()

# Define names for HOG files
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "OpenBSD")
  set(HOG_NAME "linux")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set(HOG_NAME "osx")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(HOG_NAME "win")
endif()

# rebuild d3_version.h every time
add_custom_target(get_git_hash ALL)

add_custom_command(
  POST_BUILD
  TARGET get_git_hash
  COMMAND ${CMAKE_COMMAND}
    -D SOURCE_DIR=${PROJECT_SOURCE_DIR}
    -D TARGET_DIR=${PROJECT_BINARY_DIR}
    -D PROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    -D PROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    -D PROJECT_VERSION_PATCH=${PROJECT_VERSION_PATCH}
    -P ${PROJECT_SOURCE_DIR}/cmake/CheckGit.cmake
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

install(
  FILES CHANGELOG.md USAGE.md README.md BUILD.md LICENSE THIRD_PARTY.md
  DESTINATION ${CMAKE_INSTALL_DOCDIR}
)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options("-Wno-multichar;-Wall")
endif()

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)
find_package(glm REQUIRED)
find_package(httplib QUIET)
if (TARGET httplib::httplib)
  add_library(httplib ALIAS httplib::httplib)
else()
  # Some Linux distributions do not package httplibConfig.cmake, so find the header manually
  find_file(CPP_HTTPLIB_HEADER httplib.h REQUIRED)
  find_library(CPP_HTTPLIB_LIB cpp-httplib QUIET)

  # The target httplib::httplib cannot be created manually, so rename it
  add_library(httplib INTERFACE ${CPP_HTTPLIB_HEADER})

  # And some Linux distributions build httplib as header-only,
  # and some as header+library. What a mess!
  if (NOT ${CPP_HTTPLIB_LIB} STREQUAL "CPP_HTTPLIB_LIB-NOTFOUND")
    target_link_libraries(httplib INTERFACE ${CPP_HTTPLIB_LIB})
  endif ()
endif()
find_package(ZLIB REQUIRED)
find_package(plog REQUIRED)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "OpenBSD")
  message("Building for Linux")
  add_compile_definitions(POSIX __LINUX__ _USE_OGL_ACTIVE_TEXTURES PRIMARY_HOG=\"d3-linux.hog\")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  message("Building for macOS")
  add_compile_definitions(POSIX MACOSX=1 _USE_OGL_ACTIVE_TEXTURES PRIMARY_HOG=\"d3-osx.hog\")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  # Windows.h defines to avoid as many issues as possible.
  add_compile_definitions(WIN32_LEAN_AND_MEAN NOMINMAX NODRAWTEXT NOBITMAP NOMCX NOSERVICE PRIMARY_HOG=\"d3-win.hog\"
          #[[NOGDI]] # We need GDI for now, enable when GDI is actually not needed (or when all windows.h mentions are gone)
  )

  list(APPEND CMAKE_LIBRARY_PATH "lib/win" "lib/win/DirectX/${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}")
  add_compile_options("$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:/EHsc;/RTC1;/W3;/nologo;/c;/Zi;/TP;/errorReport:prompt>")
  add_compile_options("$<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:/GL;/FD;/EHsc;/W3;/nologo;/c;/Zi;/TP;/errorReport:prompt>")

  add_link_options("/SAFESEH:NO;/NODEFAULTLIB:LIBC")
  add_link_options("$<$<CONFIG:Release>:/LTCG>")

  add_compile_definitions(WIN32 _CRT_SECURE_NO_WARNINGS _CRT_SECURE_NO_DEPRECATE _CRT_NONSTDC_NO_DEPRECATE)

  set(CMAKE_FIND_LIBRARY_PREFIXES "")
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")

  add_compile_definitions(DIRECTINPUT_VERSION=0x0500)
  find_library(DSOUND_LIBRARY NAMES dsound REQUIRED)
  find_library(DINPUT_LIBRARY NAMES dinput REQUIRED)
  find_library(DXGUID_LIBRARY NAMES dxguid REQUIRED)
  find_library(DDRAW_LIBRARY NAMES ddraw REQUIRED)

  if(MSVC AND CMAKE_CXX_SIMULATE_ID STREQUAL "")
    add_compile_options("/we4150")  # deletion of pointer to incomplete type 'type'; no destructor called
#    add_compile_options("/we4305")  # truncation from 'X' to 'Y'
    add_compile_options("/we4474")  # too many arguments passed for format string
    add_compile_options("/we4700")  # uninitialized local variable 'X' used
    add_compile_options("/we4804")  # unsafe use of type 'bool' in operation
    add_compile_options("/we4806")  # unsafe operation: no value of type 'bool' promoted to type 'int' can equal the given constant
    add_compile_options("/we4473")  # not enough arguments passed for format string
    add_compile_options("/we4477")  # format string requires argument of type X but variadic argument Y has type Z
    add_compile_options("/we4715")  # 'function' : not all control paths return a value
    add_compile_options("/we4834")  # discarding return value of function with [[nodiscard]] attribute
  endif()

  add_compile_options("/MP")  # so msbuild builds with multiple processes
endif()

add_compile_definitions($<$<CONFIG:Release>:RELEASE>)
add_compile_definitions($<$<CONFIG:Debug>:_DEBUG>)

if(ENABLE_LOGGER)
  message("Enabling Logging")
  add_compile_definitions(LOGGER)
endif()

if(FATAL_GL_ERRORS)
  message("Raising exceptions on OpenGL errors")
  add_compile_definitions(FATAL_GL_ERRORS)
endif()

include_directories(
  "cfile"   # TODO: Remove after untying all modules
  "ddebug"  # -*-
  "fix"     # -*-
  "lib"     # TODO: Remove after untying all modules
  "linux"   # -*-
  "Descent3"
  $<$<PLATFORM_ID:Windows>:
    "lib/win/DirectX"
    "lib/win"
  >
)

add_subdirectory(third_party)

add_subdirectory(2dlib)
add_subdirectory(AudioEncode)
add_subdirectory(bitmap)
add_subdirectory(cfile)
add_subdirectory(ddebug)
add_subdirectory(linux)
add_subdirectory(ddio)
add_subdirectory(fix)
add_subdirectory(logger)
add_subdirectory(manage)
add_subdirectory(grtext)
add_subdirectory(mem)
add_subdirectory(misc)
add_subdirectory(model)
add_subdirectory(module)
add_subdirectory(music)
add_subdirectory(networking)
add_subdirectory(physics)
add_subdirectory(renderer)
add_subdirectory(rtperformance)
add_subdirectory(sndlib)
add_subdirectory(stream_audio)
add_subdirectory(ui)
add_subdirectory(unzip)
add_subdirectory(vecmat)
add_subdirectory(libmve)
add_subdirectory(md5)

if(BUILD_EDITOR AND CMAKE_SYSTEM_NAME STREQUAL "Windows")
  add_subdirectory(dd_grwin32)
  add_subdirectory(win32)
  add_subdirectory(editor)
endif()

add_subdirectory(Descent3)

if (CMAKE_CROSSCOMPILING)
  find_package(HogMaker REQUIRED)
else()
  add_subdirectory(tools)
endif()

add_subdirectory(netcon)
add_subdirectory(netgames)
add_subdirectory(scripts)

# Packaging stuff
set(CPACK_SOURCE_GENERATOR "TXZ")
set(CPACK_SOURCE_IGNORE_FILES
  ".git/"
  ".github/"
  ".idea"
  build[s]/
  cmake-build-*
  ".*~$"
)
set(CPACK_VERBATIM_VARIABLES YES)
include(CPack)
