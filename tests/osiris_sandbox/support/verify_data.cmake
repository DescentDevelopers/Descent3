# Descent 3
# Copyright (C) 2024 Parallax Software
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

# Checks that the files in a test data directory are unchanged, ignoring whitespace.
#
# Each file is hashed after every run of whitespace (space, tab, CR, LF) is
# replaced by one space and leading and trailing whitespace is removed, so line
# endings, indentation, trailing spaces and final newlines do not matter. Any
# other change does. The manifest also fixes the set of files: a missing file
# or a file not listed (other than README.md and the manifest) is an error.
#
# Verify:
#   cmake -DDIR=<data dir> -DMANIFEST=<manifest> [-DSTAMP=<file>] -P verify_data.cmake
# Print the hashes of the files in a directory, in manifest form:
#   cmake -DDIR=<dir> -DPRINT=1 -P verify_data.cmake
#
# Manifest lines are "<sha256> <file name> <path in the source commit>";
# lines starting with # are comments.

cmake_minimum_required(VERSION 3.20)

function(normalized_sha256 path out)
  file(READ "${path}" content)
  string(REGEX REPLACE "[ \t\r\n]+" " " content "${content}")
  string(STRIP "${content}" content)
  string(SHA256 hash "${content}")
  set(${out} ${hash} PARENT_SCOPE)
endfunction()

if(NOT DIR OR NOT IS_DIRECTORY "${DIR}")
  message(FATAL_ERROR "verify_data.cmake: DIR must name a directory")
endif()
get_filename_component(DIR "${DIR}" ABSOLUTE)

if(PRINT)
  file(GLOB names LIST_DIRECTORIES false RELATIVE "${DIR}" "${DIR}/*")
  list(SORT names)
  foreach(name IN LISTS names)
    normalized_sha256("${DIR}/${name}" hash)
    message("${hash} ${name}")
  endforeach()
  return()
endif()

if(NOT MANIFEST OR NOT EXISTS "${MANIFEST}")
  message(FATAL_ERROR "verify_data.cmake: MANIFEST must name the manifest file")
endif()

file(STRINGS "${MANIFEST}" lines)
get_filename_component(manifest_name "${MANIFEST}" NAME)
set(expected_names README.md ${manifest_name})
set(problems "")
foreach(line IN LISTS lines)
  if(line MATCHES "^#" OR line STREQUAL "")
    continue()
  endif()
  if(NOT line MATCHES "^([0-9a-f]+) +([^ ]+) +([^ ]+)$")
    message(FATAL_ERROR "verify_data.cmake: bad manifest line: ${line}")
  endif()
  set(want "${CMAKE_MATCH_1}")
  set(name "${CMAKE_MATCH_2}")
  set(origin "${CMAKE_MATCH_3}")
  list(APPEND expected_names "${name}")
  if(NOT EXISTS "${DIR}/${name}")
    string(APPEND problems "  missing: ${name}\n")
    continue()
  endif()
  normalized_sha256("${DIR}/${name}" got)
  if(NOT got STREQUAL want)
    string(APPEND problems "  changed: ${name} (originally ${origin})\n")
  endif()
endforeach()

file(GLOB present LIST_DIRECTORIES true RELATIVE "${DIR}" "${DIR}/*")
foreach(name IN LISTS present)
  if(NOT name IN_LIST expected_names)
    string(APPEND problems "  not in the manifest: ${name}\n")
  endif()
endforeach()

if(problems)
  message(FATAL_ERROR
    "The files in ${DIR} must stay exactly as released (whitespace aside):\n"
    "${problems}"
    "They are the original Windows script sources that the OSIRIS sandbox tests run. "
    "Restore them from git; do not reformat, modernize or otherwise edit them. "
    "See ${DIR}/README.md.")
endif()

if(STAMP)
  file(TOUCH "${STAMP}")
endif()
