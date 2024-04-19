CMAKE_MINIMUM_REQUIRED(VERSION 3.19)

# allow more human readable "if then else" constructs
SET( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE )

PROJECT(Descent3 VERSION 1.5.500)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

IF (UNIX)
  SET (D3_GAMEDIR "~/Descent3/")

  IF (UNIX AND NOT APPLE)
    SET(EXTRA_CXX_FLAGS "-fpermissive ")
  ENDIF()

  STRING(APPEND CMAKE_CXX_FLAGS " -Wno-write-strings -Wno-multichar -Wno-address-of-temporary ${BITS} ${EXTRA_CXX_FLAGS}")
  STRING(APPEND CMAKE_C_FLAGS " ${BITS}")

  FIND_PACKAGE(SDL REQUIRED)
  IF (APPLE)
    # Provide FIND_PACKAGE( SDL_image ) below with an include dir and library that work with brew-installed sdl2_image
    FIND_PATH(SDL_IMAGE_INCLUDE_DIR SDL_image.h PATH_SUFFIXES include/SDL2)
    FIND_LIBRARY(SDL_IMAGE_LIBRARY SDL2_image)
  ENDIF()
  FIND_PACKAGE(SDL_image REQUIRED)
  FIND_PACKAGE(Curses REQUIRED)
  FIND_PACKAGE(OpenGL REQUIRED)
MESSAGE( "SDL Include Dir is " ${SDL_INCLUDE_DIR} )
ENDIF()

IF (UNIX AND NOT APPLE)
  MESSAGE("Building for Linux")

  ADD_DEFINITIONS( -D_DEBUG -D__LINUX__ -DLINUX -D_MAX_PATH=260 -D_MAX_FNAME=256 -D_REENRANT -D__32BIT__ -DHAVEALLOCA_H -D_USE_OGL_ACTIVE_TEXTURES)
  SET(PLATFORM_INCLUDES "lib/linux" ${SDL_INCLUDE_DIR} )
ENDIF()

IF (APPLE)
	SET(D3_GAMEDIR "~/Descent3X")	
  MESSAGE("Building for MAC OSX")
  ADD_DEFINITIONS(-D_DEBUG -D__LINUX__ -DLINUX -D_MAX_PATH=260 -D_MAX_FNAME=256 -D_REENRANT -DMACOSX=1 -D_USE_OGL_ACTIVE_TEXTURES)
  SET(PLATFORM_INCLUDES "lib/linux" ${SDL_INCLUDE_DIR} "/usr/X11/include" )
ENDIF()

IF (WIN32)
	SET(D3_GAMEDIR "c:/games/Descent3/")	
	set (CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "lib/win" "lib/win/directx")
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /EHsc /RTC1 /W3 /nologo /c /Zi /TP /errorReport:prompt")
	SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL /FD /EHsc /W3 /nologo /c /Zi /TP /errorReport:prompt")
	
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO /SUBSYSTEM:WINDOWS /NODEFAULTLIB:LIBC")
	SET(CMAKE_MODULE_LINKER_FLAGS "/SAFESEH:NO /SUBSYSTEM:WINDOWS /NODEFAULTLIB:LIBC")
	ADD_DEFINITIONS (-DIS_WINDOWS -D_CRT_SECURE_NO_WARNINGS -DWIN32 -D_CRT_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -DNOMINMAX)

	SET(PLATFORM_INCLUDES "lib/win/directx" "lib/win")
	
	SET(CMAKE_FIND_LIBRARY_PREFIXES "")
	SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
	
	FIND_LIBRARY(DSOUND_LIBRARY NAMES dsound "${CMAKE_SOURCE_DIR}/lib/win" "${CMAKE_SOURCE_DIR}/lib/win/directx")
	FIND_LIBRARY(DINPUT_LIBRARY NAMES dinput "${CMAKE_SOURCE_DIR}/lib/win" "${CMAKE_SOURCE_DIR}/lib/win/directx")
	FIND_LIBRARY(DXGUID_LIBRARY NAMES dxguid "${CMAKE_SOURCE_DIR}/lib/win" "${CMAKE_SOURCE_DIR}/lib/win/directx")
	FIND_LIBRARY(DDRAW_LIBRARY NAMES ddraw "${CMAKE_SOURCE_DIR}/lib/win" "${CMAKE_SOURCE_DIR}/lib/win/directx")

	if (MSVC AND CMAKE_CXX_SIMULATE_ID STREQUAL "")
		add_compile_options("/we4150")  # deletion of pointer to incomplete type 'type'; no destructor called
#		add_compile_options("/we4305")  # truncation from 'X' to 'Y'
		add_compile_options("/we4474")  # too many arguments passed for format string
		add_compile_options("/we4700")  # uninitialized local variable 'X' used
		add_compile_options("/we4804")  # unsafe use of type 'bool' in operation
		add_compile_options("/we4806")  # unsafe operation: no value of type 'bool' promoted to type 'int' can equal the given constant
		add_compile_options("/we4473")  # not enough arguments passed for format string
		add_compile_options("/we4477")  # format string requires argument of type X but variadic argument Y has type Z
		add_compile_options("/we4715")  # 'function' : not all control paths return a value
		add_compile_options("/we4834")  # discarding return value of function with [[nodiscard]] attribute
	endif()
ENDIF ()

MESSAGE("Install will copy files to ${D3_GAMEDIR}")

include_directories("${CMAKE_SOURCE_DIR}" "lib" "Descent3" ${PLATFORM_INCLUDES})

# file(GLOB_RECURSE INCS "*.h")

ADD_SUBDIRECTORY (2dlib)
ADD_SUBDIRECTORY (AudioEncode)
ADD_SUBDIRECTORY (bitmap)
ADD_SUBDIRECTORY (cfile)
ADD_SUBDIRECTORY (czip)
ADD_SUBDIRECTORY (d3music)

IF (WIN32)
ADD_SUBDIRECTORY (dd_grwin32)
ADD_SUBDIRECTORY (dd_vidwin32)
ADD_SUBDIRECTORY (ddio_win)
ADD_SUBDIRECTORY (win32)
ADD_SUBDIRECTORY (dd_sndlib)
ENDIF()

IF (UNIX)
ADD_SUBDIRECTORY(linux)
ADD_SUBDIRECTORY(dd_lnxsound)
ADD_SUBDIRECTORY(ddvid_lnx)
ADD_SUBDIRECTORY(lnxcontroller)
ADD_SUBDIRECTORY(ddio_lnx)
ENDIF()

ADD_SUBDIRECTORY (ddio_common)
ADD_SUBDIRECTORY (fix)
ADD_SUBDIRECTORY (grtext)
ADD_SUBDIRECTORY (manage)
ADD_SUBDIRECTORY (mem)
ADD_SUBDIRECTORY (misc)
ADD_SUBDIRECTORY (model)
ADD_SUBDIRECTORY (module)
ADD_SUBDIRECTORY (movie)
ADD_SUBDIRECTORY (music)
ADD_SUBDIRECTORY (networking)
ADD_SUBDIRECTORY (physics)
ADD_SUBDIRECTORY (renderer)
ADD_SUBDIRECTORY (rtperformance)
ADD_SUBDIRECTORY (sndlib)
ADD_SUBDIRECTORY (stream_audio)
ADD_SUBDIRECTORY (ui)
ADD_SUBDIRECTORY (unzip)
ADD_SUBDIRECTORY (vecmat)
ADD_SUBDIRECTORY (libmve)
ADD_SUBDIRECTORY (md5)
ADD_SUBDIRECTORY (libacm)
ADD_SUBDIRECTORY (Descent3)

# For now we don't need to build the scripts under windows, so we'll only include
# the directory when building for linux/osx. In the future we may want to to fix bugs, etc.
IF(UNIX)
ADD_SUBDIRECTORY (netgames)
ADD_SUBDIRECTORY (netcon)
ADD_SUBDIRECTORY (scripts)
ENDIF()

# set default cmake build type to Debug (None Debug Release RelWithDebInfo MinSizeRel)
IF( NOT CMAKE_BUILD_TYPE )
SET( CMAKE_BUILD_TYPE "Debug" )
ENDIF()
