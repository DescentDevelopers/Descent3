
ADD_DEFINITIONS (-DCHECKSUM=2273873307UL)
SET (HEADERS )
SET (CPPS )
IF(UNIX)
SET (CPPS
	linux_lib.cpp)
ENDIF()

SET (SCRIPTS
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

FOREACH(SCRIPT ${SCRIPTS})
 
	ADD_LIBRARY(${SCRIPT} SHARED ${CPPS} "${SCRIPT}.cpp")
	set_target_properties(${SCRIPT} PROPERTIES PREFIX "")
	IF(UNIX)
		ADD_CUSTOM_COMMAND(
			TARGET ${SCRIPT}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${SCRIPT}> "${CMAKE_SOURCE_DIR}/scripts/data/linuxfullhog/"
			COMMENT "Copying file ${SCRIPT} to hogfile directory..."
		)
		ADD_CUSTOM_COMMAND(
			TARGET ${SCRIPT}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${SCRIPT}> "${CMAKE_SOURCE_DIR}/scripts/data/linuxdemohog/"
		)
	ENDIF()
ENDFOREACH()

IF(UNIX AND NOT APPLE)
  SET(HOGBUILDCMD "${CMAKE_SOURCE_DIR}/scripts/hogutils/hogUtils-i686")
  SET(HOGARCH "linux")


  ADD_CUSTOM_TARGET("FullLinuxHog"
    ALL
    COMMAND ${HOGBUILDCMD} "--makehog" "d3linuxfullhog.txt" 
    COMMAND ${CMAKE_COMMAND} -E remove "${CMAKE_SOURCE_DIR}/scripts/data/linuxfullhog/d3-${HOGARCH}.hog"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/scripts/data/linuxfullhog/new.hog" "${CMAKE_SOURCE_DIR}/scripts/data/linuxfullhog/d3-${HOGARCH}.hog"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/scripts/data/linuxfullhog/d3-${HOGARCH}.hog" "${D3_GAMEDIR}"
    COMMAND ${CMAKE_COMMAND} -E remove "${CMAKE_SOURCE_DIR}/scripts/data/linuxfullhog/new.hog"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/scripts/data/linuxfullhog/"
    DEPENDS ${SCRIPTS}
    COMMENT "Building platform specific hog."
    )
  ADD_CUSTOM_TARGET("DemoLinuxHog"
    ALL
    COMMAND ${HOGBUILDCMD} "--makehog" "d3linuxdemohog.txt" 
    COMMAND ${CMAKE_COMMAND} -E remove "${CMAKE_SOURCE_DIR}/scripts/data/linuxdemohog/d3-${HOGARCH}.hog"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/scripts/data/linuxdemohog/new.hog" "${CMAKE_SOURCE_DIR}/scripts/data/linuxdemohog/d3-${HOGARCH}.hog"
    COMMAND ${CMAKE_COMMAND} -E remove "${CMAKE_SOURCE_DIR}/scripts/data/linuxdemohog/new.hog"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/scripts/data/linuxdemohog/"
    DEPENDS ${SCRIPTS}
    COMMENT "Building platform specific hog."
    )
ENDIF()