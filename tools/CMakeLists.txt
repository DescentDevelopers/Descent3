add_executable(
        hogedit
	HogEdit/HogFormat.cpp
	HogEdit/HogEdit.cpp
)
add_dependencies(hogedit get_git_hash)
target_include_directories(hogedit PRIVATE ${PROJECT_BINARY_DIR}/lib)
