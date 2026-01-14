# Descent 3
# Copyright (C) 2024 Descent Developers
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


# - Function making a HOG file based on supplied filelist.
#
# This module provides function MakeHog() used for building HOG files.
# MakeHog() registers custom target TARGET which can be used as dependency.
#
#   MakeHog(
#     <TARGET target>
#     <OUTPUT d3-full-win32.hog>
#     <INPUT_FILE filelist.txt>
#     [SEARCH_PATH additional_dirs]
#     [DEPENDS additional_dependencies]
#   )
#

function(MakeHog)
  set(options)
  set(oneValueArgs INPUT_FILE OUTPUT TARGET)
  set(multiValueArgs DEPENDS SEARCH_PATH)

  cmake_parse_arguments(PARSE_ARGV 0
    "HOG"
    "${options}"
    "${oneValueArgs}"
    "${multiValueArgs}"
  )

  # Register custom target for tracking dependencies
  add_custom_target("${HOG_TARGET}"
    DEPENDS "${HOG_OUTPUT}"
  )

  if (NOT CMAKE_CROSSCOMPILING)
    set(HOGMAKE_BINARY $<TARGET_FILE:HogMaker>)
    set(HOGMAKE_DEPENDENCY HogMaker)
  elseif (HOGMAKE_BINARY)
    set(HOGMAKE_DEPENDENCY "${HOGMAKE_BINARY}")
  else()
    message(FATAL_ERROR "HOGMAKE_BINARY not set")
  endif()

  add_custom_command(OUTPUT "${HOG_OUTPUT}"
    COMMAND "${HOGMAKE_BINARY}"
      "${HOG_OUTPUT}"
      "${HOG_INPUT_FILE}"
      "${HOG_SEARCH_PATH}"
    DEPENDS ${HOGMAKE_DEPENDENCY} ${HOG_INPUT_FILE} ${HOG_DEPENDS}
    COMMENT "Generate ${HOG_OUTPUT}"
    COMMAND_EXPAND_LISTS
  )
endfunction()
