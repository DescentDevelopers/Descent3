add_executable(
	HogMaker
	HogMaker/HogFormat.cpp
	HogMaker/HogMaker.cpp
)
add_dependencies(HogMaker get_git_hash)
target_include_directories(HogMaker PRIVATE ${PROJECT_BINARY_DIR}/lib)
