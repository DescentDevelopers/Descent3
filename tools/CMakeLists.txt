cmake_minimum_required(VERSION 3.20)  # For using CMAKE_<LANG>_BYTE_ORDER

project(HogMaker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
	HogMaker
	HogMaker/HogFormat.cpp
	HogMaker/HogMaker.cpp
)

target_include_directories(HogMaker PRIVATE ${PROJECT_SOURCE_DIR}/../lib ${PROJECT_BINARY_DIR}/lib)
