set(CMAKE_FOLDER "tests")

add_executable(
  byteswap_tests
  byteswap_tests.cpp
)
target_link_libraries(
  byteswap_tests
  GTest::gtest_main
)
target_include_directories(byteswap_tests PRIVATE ${PROJECT_SOURCE_DIR}/lib)

gtest_discover_tests(byteswap_tests)

# x86 sandbox. The Win32 test DLL is built with clang and lld-link when they are
# available; otherwise the tests that need it are skipped.
add_executable(x86sandbox_tests x86sandbox/x86sandbox_tests.cpp)
target_link_libraries(x86sandbox_tests GTest::gtest_main x86sandbox)

find_program(X86SB_CLANG NAMES clang)
find_program(X86SB_LLD_LINK NAMES lld-link)
find_program(X86SB_DLLTOOL NAMES llvm-dlltool)
if(X86SB_CLANG AND X86SB_LLD_LINK AND X86SB_DLLTOOL)
  set(X86SB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/x86sandbox)
  set(X86SB_OUT ${CMAKE_CURRENT_BINARY_DIR}/x86sandbox)
  add_custom_command(
    OUTPUT ${X86SB_OUT}/test_dll.dll
    COMMAND ${CMAKE_COMMAND} -E make_directory ${X86SB_OUT}
    COMMAND ${X86SB_DLLTOOL} -m i386 -d ${X86SB_SRC}/msvcrt.def -l ${X86SB_OUT}/msvcrt.lib
    COMMAND ${X86SB_DLLTOOL} -m i386 -k -d ${X86SB_SRC}/kernel32.def -l ${X86SB_OUT}/kernel32.lib
    COMMAND ${X86SB_CLANG} --target=i686-pc-windows-msvc -O2 -ffreestanding -fno-builtin -march=i686 -mno-sse
            -c ${X86SB_SRC}/test_dll.c -o ${X86SB_OUT}/test_dll.obj
    COMMAND ${X86SB_LLD_LINK} /dll /nodefaultlib /entry:DllMain /out:${X86SB_OUT}/test_dll.dll
            ${X86SB_OUT}/test_dll.obj ${X86SB_OUT}/msvcrt.lib ${X86SB_OUT}/kernel32.lib
    DEPENDS ${X86SB_SRC}/test_dll.c ${X86SB_SRC}/msvcrt.def ${X86SB_SRC}/kernel32.def
    COMMENT "Building the Win32 test DLL for x86sandbox_tests"
  )
  add_custom_target(x86sandbox_test_dll DEPENDS ${X86SB_OUT}/test_dll.dll)
  add_dependencies(x86sandbox_tests x86sandbox_test_dll)
  target_compile_definitions(x86sandbox_tests PRIVATE X86SANDBOX_TEST_DLL="${X86SB_OUT}/test_dll.dll")
else()
  message(STATUS "x86sandbox_tests: clang, lld-link or llvm-dlltool not found; DLL tests will be skipped")
endif()

gtest_discover_tests(x86sandbox_tests)
