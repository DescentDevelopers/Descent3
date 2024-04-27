cmake_minimum_required(VERSION 3.20)  # For using CMAKE_<LANG>_BYTE_ORDER

project(Descent3 VERSION 1.5.500)

if(NOT MSVC) # GCC/clang or compatible, hopefully
  option(FORCE_COLORED_OUTPUT "Always produce ANSI-colored compiler warnings/errors (GCC/Clang only; esp. useful with ninja)." OFF)
endif()

option(LOGGER "Enable logging to the terminal" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CONFIGURATION_TYPES "Debug" "Release" "DebugAsan")

if(CMAKE_CXX_BYTE_ORDER STREQUAL "BIG_ENDIAN")
  message(STATUS "Big Endian system detected.")
  add_definitions("-DOUTRAGE_BIG_ENDIAN")
endif()

if(BUILD_TESTING)
  find_package(GTest REQUIRED)
  enable_testing()
  include(GoogleTest)
  add_subdirectory(tests)
endif()

if(NOT MSVC)
  # check if this is some kind of clang (Clang, AppleClang, whatever)
  # (convert compiler ID to lowercase so we match Clang, clang, AppleClang etc, regardless of case)
  string(TOLOWER ${CMAKE_CXX_COMPILER_ID} compiler_id_lower)
  if(compiler_id_lower MATCHES ".*clang.*")
    message(STATUS "Compiler \"${CMAKE_CXX_COMPILER_ID}\" detected as some kind of clang")
    set(D3_COMPILER_IS_CLANG TRUE)
    set(D3_COMPILER_IS_GCC_OR_CLANG TRUE)
  elseif(CMAKE_COMPILER_IS_GNUCC)
    set(D3_COMPILER_IS_GCC_OR_CLANG TRUE)
  endif()
  unset(compiler_id_lower)

  if(FORCE_COLORED_OUTPUT)
    if(CMAKE_COMPILER_IS_GNUCC)
      add_compile_options (-fdiagnostics-color=always)
    elseif(D3_COMPILER_IS_CLANG)
      add_compile_options (-fcolor-diagnostics)
    endif ()
  endif ()
endif()

if(UNIX)
  set(D3_GAMEDIR "~/Descent3/")

  string(APPEND CMAKE_CXX_FLAGS " -Wno-write-strings -Wno-multichar ${BITS} ${EXTRA_CXX_FLAGS}")
  string(APPEND CMAKE_C_FLAGS " ${BITS}")

  find_package(SDL REQUIRED)
  if(APPLE)
    # Provide FIND_PACKAGE( SDL_image ) below with an include dir and library that work with brew-installed sdl2_image
    find_path(SDL_IMAGE_INCLUDE_DIR SDL_image.h PATH_SUFFIXES include/SDL2)
    find_library(SDL_IMAGE_LIBRARY SDL2_image)
  endif()

  find_package(SDL_image REQUIRED)
  find_package(Curses REQUIRED)
  find_package(OpenGL REQUIRED)
  message("SDL Include Dir is " ${SDL_INCLUDE_DIR})
endif()

if(UNIX AND NOT APPLE)
  message("Building for Linux")
  add_definitions(-D__LINUX__ -DLINUX -D_MAX_PATH=260 -D_MAX_FNAME=256 -D_REENRANT -D__32BIT__ -DHAVEALLOCA_H -D_USE_OGL_ACTIVE_TEXTURES)
  set(PLATFORM_INCLUDES "lib/linux" ${SDL_INCLUDE_DIR})
endif()

if(APPLE)
  message("Building for MAC OSX")
  add_definitions(-D__LINUX__ -DLINUX -D_MAX_PATH=260 -D_MAX_FNAME=256 -D_REENRANT -DMACOSX=1 -D_USE_OGL_ACTIVE_TEXTURES)
  set(PLATFORM_INCLUDES "lib/linux" ${SDL_INCLUDE_DIR} "/usr/X11/include")
endif()

if(WIN32)
  set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "lib/win" "lib/win/directx")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /EHsc /RTC1 /W3 /nologo /c /Zi /TP /errorReport:prompt")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL /FD /EHsc /W3 /nologo /c /Zi /TP /errorReport:prompt")

  # default cmake options set -fsanitize and other clang/gcc-style parameters, even for windows. override them with normal debug + /fsanitize
  set(CMAKE_CXX_FLAGS_DEBUGASAN "${CMAKE_CXX_FLAGS_DEBUG} /fsanitize=address")

  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO /SUBSYSTEM:WINDOWS /NODEFAULTLIB:LIBC")
  set(CMAKE_MODULE_LINKER_FLAGS "/SAFESEH:NO /SUBSYSTEM:WINDOWS /NODEFAULTLIB:LIBC")
  add_definitions(-DIS_WINDOWS -D_CRT_SECURE_NO_WARNINGS -DWIN32 -D_CRT_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -DNOMINMAX)

  set(PLATFORM_INCLUDES "lib/win/directx" "lib/win")

  set(CMAKE_FIND_LIBRARY_PREFIXES "")
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")

  find_library(DSOUND_LIBRARY NAMES dsound "${CMAKE_SOURCE_DIR}/lib/win" "${CMAKE_SOURCE_DIR}/lib/win/directx")
  find_library(DINPUT_LIBRARY NAMES dinput "${CMAKE_SOURCE_DIR}/lib/win" "${CMAKE_SOURCE_DIR}/lib/win/directx")
  find_library(DXGUID_LIBRARY NAMES dxguid "${CMAKE_SOURCE_DIR}/lib/win" "${CMAKE_SOURCE_DIR}/lib/win/directx")
  find_library(DDRAW_LIBRARY NAMES ddraw "${CMAKE_SOURCE_DIR}/lib/win" "${CMAKE_SOURCE_DIR}/lib/win/directx")

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
endif()

add_compile_definitions($<$<CONFIG:Release>:RELEASE>)
add_compile_definitions($<$<CONFIG:Debug>:_DEBUG>)

find_package(ZLIB REQUIRED)

if(LOGGER)
  message("Enabling Logging")
  add_definitions(-DLOGGER)
endif()

include_directories(
  "cfile"   # TODO: Remove after untying all modules
  "lib"     # TODO: Remove after untying all modules
  "Descent3"
  ${PLATFORM_INCLUDES}
)

# file(GLOB_RECURSE INCS "*.h")

add_subdirectory(2dlib)
add_subdirectory(AudioEncode)
add_subdirectory(bitmap)
add_subdirectory(cfile)
add_subdirectory(czip)
add_subdirectory(d3music)

if(WIN32)
  add_subdirectory(dd_grwin32)
  add_subdirectory(dd_vidwin32)
  add_subdirectory(win32)
  add_subdirectory(dd_sndlib)
  add_subdirectory(ddio_win)
endif()

if(UNIX)
  add_subdirectory(linux)
  add_subdirectory(ddvid_lnx)
  add_subdirectory(dd_lnxsound)
  add_subdirectory(lnxcontroller)
  add_subdirectory(ddio_lnx)
endif()

add_subdirectory(ddio_common)
add_subdirectory(fix)
add_subdirectory(manage)
add_subdirectory(grtext)
add_subdirectory(mem)
add_subdirectory(misc)
add_subdirectory(model)
add_subdirectory(module)
add_subdirectory(movie)
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
add_subdirectory(libacm)
add_subdirectory(Descent3)

add_subdirectory(tools)

# For now we don't need to build the scripts under windows, so we'll only include
# the directory when building for linux/osx. In the future we may want to to fix bugs, etc.
if(UNIX)
  add_subdirectory(netgames)
  add_subdirectory(netcon)
  add_subdirectory(scripts)
endif()

# set default cmake build type to Debug (Debug Release DebugAsan)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()
