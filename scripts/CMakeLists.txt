set(HEADERS)
set(CPPS)
if (UNIX)
  set(CPPS
    linux_lib.cpp)
endif ()

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

add_custom_target(HogLinuxFull-copy
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/scripts/data/linuxfullhog ${CMAKE_BINARY_DIR}/scripts/data/linuxfullhog/
  COMMENT "Copy script/data/linuxfullhog directory"
)

#add_custom_target(HogLinuxDemo-copy
#	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/scripts/data/linuxdemohog ${CMAKE_BINARY_DIR}/scripts/data/linuxdemohog/
#	COMMENT "Copy script/data/linuxdemohog directory"
#)

foreach(SCRIPT ${SCRIPTS})
  add_library(${SCRIPT} SHARED ${CPPS} "${SCRIPT}.cpp")
  target_link_libraries(${SCRIPT} fix)
  add_dependencies(${SCRIPT} HogLinuxFull-copy) #  HogLinuxDemo-copy
  set_target_properties(${SCRIPT} PROPERTIES PREFIX "")
  if (UNIX)
    add_custom_command(
      TARGET ${SCRIPT}
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${SCRIPT}> "${CMAKE_BINARY_DIR}/scripts/data/linuxfullhog/"
      COMMENT "Copying file ${SCRIPT} to scripts/data/linuxfullhog/..."
    )
    #		add_custom_command(
    #			TARGET ${SCRIPT}
    #			POST_BUILD
    #			COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${SCRIPT}> "${CMAKE_BINARY_DIR}/scripts/data/linuxdemohog/"
    #			COMMENT "Copying file ${SCRIPT} to scripts/data/linuxdemohog/..."
    #		)
  endif ()
endforeach ()

if (UNIX)
  add_custom_target(HogLinuxFull
    COMMAND $<TARGET_FILE:HogMaker> "$<TARGET_FILE_DIR:Descent3>/d3-linux.hog" "${CMAKE_BINARY_DIR}/scripts/data/linuxfullhog/d3linuxfullhog.txt"
    DEPENDS ${SCRIPTS} HogMaker
    COMMENT "Generate linuxfullhog/d3-linux.hog"
  )

  # FIXME: there may be only one d3-linux.hog, need deal with demo somehow.
  #	add_custom_target(HogLinuxDemo
  #		COMMAND $<TARGET_FILE:HogMaker> "${CMAKE_BINARY_DIR}/scripts/data/linuxdemohog/d3-linux.hog" "${CMAKE_BINARY_DIR}/scripts/data/linuxdemohog/d3linuxdemohog.txt"
  #		DEPENDS ${SCRIPTS} HogMaker
  #		COMMENT "Generate linuxdemohog/d3-linux.hog"
  #	)

endif ()
