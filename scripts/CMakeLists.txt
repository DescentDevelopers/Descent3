set(CMAKE_FOLDER "scripts")

set(HEADERS
  AIGame3_External.h
  linux_lib.h
  osiris_common.h
  osiris_import.h
  osiris_vector.h)
set(CPPS
  $<$<PLATFORM_ID:Darwin,Linux,OpenBSD,Android>:
    linux_lib.cpp
  >
)

set(SCRIPTS
  aigame2
  AIGame3
  aigame4
  AIGame
  barney
  BatteriesIncluded
  BossCamera
  CanyonsCTF
  CellTestLevel
  ChrisTest
  clutter
  generic
  Geodomes
  HalfPipe
  InfernalBolt
  Inversion
  LEVEL0
  level10
  level11
  Level12
  level13
  level14
  LEVEL15
  Level16
  level17
  level1
  level2
  level3
  level4
  level5
  Level6
  level7
  level8
  Level9
  LevelS1
  levelS2
  Merc02
  Merc1
  Merc3
  Merc4
  merc5
  Merc6
  Merc7
  myPowerHouse
  Mysterious_Isle
  orbital
  Paranoia
  PiccuStation
  Polaris
  Quadsomniac
  RudeAwakening
  SewerRat
  testscript
  TrainingMission
  Y2K
)

add_library(dallas STATIC DallasFuncs.cpp osiris_import.cpp osiris_vector.cpp)
set_target_properties(dallas PROPERTIES PREFIX "")
set_target_properties(dallas PROPERTIES CXX_VISIBILITY_PRESET "hidden")
target_link_libraries(dallas fix misc)

foreach(SCRIPT ${SCRIPTS})
  add_library(${SCRIPT} MODULE ${CPPS} ${HEADERS} "${SCRIPT}.cpp")
  target_link_libraries(${SCRIPT}
    dallas
    fix
    misc
    module
  )
  set_target_properties(${SCRIPT} PROPERTIES PREFIX "")
  set_target_properties(${SCRIPT} PROPERTIES CXX_VISIBILITY_PRESET "hidden")
  if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set_target_properties(${SCRIPT} PROPERTIES SUFFIX ".dylib")
    macos_sign(${SCRIPT})
  endif()
endforeach()

include(HogMaker)
MakeHog(
  TARGET HogFull
  OUTPUT "${D3_GENERATED_FILES_OUTPUT_DIRECTORY}/d3-${HOG_NAME}.hog"
  INPUT_FILE "${CMAKE_SOURCE_DIR}/scripts/data/fullhog/d3-${HOG_NAME}-fullhog.txt"
  SEARCH_PATH "$<TARGET_FILE_DIR:AIGame>"
  DEPENDS ${SCRIPTS}
)

install(
  FILES "${D3_GENERATED_FILES_OUTPUT_DIRECTORY}/d3-${HOG_NAME}.hog"
  DESTINATION ${CMAKE_INSTALL_DATADIR}
)

# FIXME: there may be only one d3-linux.hog, need deal with demo somehow.
#	add_custom_target(HogLinuxDemo
#		COMMAND $<TARGET_FILE:HogMaker>
#     "${CMAKE_BINARY_DIR}/scripts/data/demohog/d3-${HOG_NAME}.hog"
#     "${CMAKE_BINARY_DIR}/scripts/data/demohog/d3-${HOG_NAME}-demohog.txt"
#     "${CMAKE_BINARY_DIR}/scripts/"
#		DEPENDS ${SCRIPTS} HogMaker data/fullhog/d3-${HOG_NAME}-demohog.txt
#		COMMENT "Generate linuxdemohog/d3-${HOG_NAME}.hog"
#	)
