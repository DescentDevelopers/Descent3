add_executable(
	HogMaker
	HogMaker/HogFormat.cpp
	HogMaker/HogMaker.cpp
)
target_include_directories(HogMaker PRIVATE ${PROJECT_SOURCE_DIR}/lib)
