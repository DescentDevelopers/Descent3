set(CPPS
  $<$<PLATFORM_ID:Darwin,Linux>:
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
  GameGauge
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
  Y2K)

add_custom_target(HogFull-copy
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/scripts/data/fullhog ${CMAKE_BINARY_DIR}/scripts/data/fullhog/
  COMMENT "Copy script/data/fullhog directory"
)

#add_custom_target(HogDemo-copy
#	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/scripts/data/demohog ${CMAKE_BINARY_DIR}/scripts/data/demohog/
#	COMMENT "Copy script/data/demohog directory"
#)

foreach(SCRIPT ${SCRIPTS})
  add_library(${SCRIPT} MODULE ${CPPS} "${SCRIPT}.cpp")
  target_link_libraries(${SCRIPT} fix)
  add_dependencies(${SCRIPT} HogFull-copy) #  HogDemo-copy
  set_target_properties(${SCRIPT} PROPERTIES PREFIX "")
  if (APPLE)
    set_target_properties(${SCRIPT} PROPERTIES SUFFIX ".dylib")
  endif ()
  if (UNIX)
    add_custom_command(
      TARGET ${SCRIPT}
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${SCRIPT}> "${CMAKE_BINARY_DIR}/scripts/data/fullhog/"
      COMMENT "Copying file ${SCRIPT} to scripts/data/fullhog/..."
    )
    #		add_custom_command(
    #			TARGET ${SCRIPT}
    #			POST_BUILD
    #			COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${SCRIPT}> "${CMAKE_BINARY_DIR}/scripts/data/demohog/"
    #			COMMENT "Copying file ${SCRIPT} to scripts/data/demohog/..."
    #		)
  endif ()
endforeach ()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(HOG_NAME "linux")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set(HOG_NAME "osx")
endif()

add_custom_target(HogFull
  COMMAND $<TARGET_FILE:HogMaker> "$<TARGET_FILE_DIR:Descent3>/d3-${HOG_NAME}.hog" "${CMAKE_BINARY_DIR}/scripts/data/fullhog/d3-${HOG_NAME}-fullhog.txt"
  DEPENDS ${SCRIPTS} HogMaker data/fullhog/d3-linux-fullhog.txt data/fullhog/d3-osx-fullhog.txt
  COMMENT "Generate fullhog/d3-${HOG_NAME}.hog"
)

# FIXME: there may be only one d3-linux.hog, need deal with demo somehow.
#	add_custom_target(HogLinuxDemo
#		COMMAND $<TARGET_FILE:HogMaker> "${CMAKE_BINARY_DIR}/scripts/data/demohog/d3-${HOG_NAME}.hog" "${CMAKE_BINARY_DIR}/scripts/data/demohog/d3-${HOG_NAME}-demohog.txt"
#		DEPENDS ${SCRIPTS} HogMaker
#		COMMENT "Generate linuxdemohog/d3-${HOG_NAME}.hog"
#	)
