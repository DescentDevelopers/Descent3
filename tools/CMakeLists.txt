if(WIN32)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE")
endif()

add_executable(
	HogMaker
	HogMaker/HogFormat.cpp
	HogMaker/HogMaker.cpp
)
add_dependencies(HogMaker get_git_hash)
target_include_directories(HogMaker PRIVATE ${PROJECT_BINARY_DIR}/lib)
