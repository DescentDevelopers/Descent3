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

# OSIRIS sandbox bridge (Descent3/osiris_sandbox.cpp), tested against a recording
# fake engine that gen_fakes.py generates from the engine's own headers, with the
# original level1 script built as a Win32 DLL.
find_package(Python3 REQUIRED COMPONENTS Interpreter)
set(OSB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/osiris_sandbox)
set(OSB_HOST ${OSB_SRC}/support/host)
set(OSB_GUEST ${OSB_SRC}/support/guest)
set(OSB_OUT ${CMAKE_CURRENT_BINARY_DIR}/osiris_sandbox)

add_custom_command(
  OUTPUT ${OSB_OUT}/fake_engine_gen.cpp
  COMMAND ${CMAKE_COMMAND} -E make_directory ${OSB_OUT}
  COMMAND Python3::Interpreter ${OSB_HOST}/gen_fakes.py ${PROJECT_SOURCE_DIR} ${OSB_OUT}/fake_engine_gen.cpp
  DEPENDS ${OSB_HOST}/gen_fakes.py ${PROJECT_SOURCE_DIR}/Descent3/osiris_predefs.h
          ${PROJECT_SOURCE_DIR}/Descent3/OsirisLoadandBind.cpp
  COMMENT "Generating the fake engine for osiris_sandbox_tests"
)

add_executable(osiris_sandbox_tests
  ${OSB_SRC}/osiris_sandbox_tests.cpp
  ${OSB_HOST}/fake_engine.cpp
  ${OSB_OUT}/fake_engine_gen.cpp
  ${PROJECT_SOURCE_DIR}/Descent3/osiris_sandbox.cpp
)
target_link_libraries(osiris_sandbox_tests GTest::gtest_main x86sandbox logger)
# Engine headers only: the engine functions themselves are the fakes.
target_include_directories(osiris_sandbox_tests PRIVATE
  ${OSB_HOST}
  ${PROJECT_BINARY_DIR}/lib
  ${PROJECT_SOURCE_DIR}/2dlib
  ${PROJECT_SOURCE_DIR}/bitmap
  ${PROJECT_SOURCE_DIR}/cfile
  ${PROJECT_SOURCE_DIR}/ddebug
  ${PROJECT_SOURCE_DIR}/ddio
  ${PROJECT_SOURCE_DIR}/Descent3
  ${PROJECT_SOURCE_DIR}/fix
  ${PROJECT_SOURCE_DIR}/grtext
  ${PROJECT_SOURCE_DIR}/lib
  ${PROJECT_SOURCE_DIR}/linux
  ${PROJECT_SOURCE_DIR}/mem
  ${PROJECT_SOURCE_DIR}/misc
  ${PROJECT_SOURCE_DIR}/model
  ${PROJECT_SOURCE_DIR}/module
  ${PROJECT_SOURCE_DIR}/physics
  ${PROJECT_SOURCE_DIR}/sndlib
  ${PROJECT_SOURCE_DIR}/ui
)
# The platform definitions (POSIX, WIN32 and so on) that Descent3 is compiled with.
target_compile_definitions(osiris_sandbox_tests PRIVATE $<TARGET_PROPERTY:Descent3,COMPILE_DEFINITIONS>)

# data/level1 holds original-release sources used as test data; the build stops
# if they are edited (whitespace aside). See data/level1/README.md.
set(L1_SRC ${OSB_SRC}/data/level1)
file(GLOB L1_FILES CONFIGURE_DEPENDS ${L1_SRC}/*)
add_custom_command(
  OUTPUT ${OSB_OUT}/level1_verified.stamp
  COMMAND ${CMAKE_COMMAND} -E make_directory ${OSB_OUT}
  COMMAND ${CMAKE_COMMAND} -DDIR=${L1_SRC} -DMANIFEST=${L1_SRC}/SHA256SUMS
          -DSTAMP=${OSB_OUT}/level1_verified.stamp -P ${OSB_SRC}/support/verify_data.cmake
  DEPENDS ${L1_FILES} ${OSB_SRC}/support/verify_data.cmake
  COMMENT "Checking that tests/osiris_sandbox/data/level1 is unchanged"
)
add_custom_target(osiris_sandbox_level1_check DEPENDS ${OSB_OUT}/level1_verified.stamp)
add_dependencies(osiris_sandbox_tests osiris_sandbox_level1_check)

if(X86SB_CLANG AND X86SB_LLD_LINK AND X86SB_DLLTOOL)
  # CHECKSUM is the 32-bit value from the top-level CMakeLists.txt.
  set(L1_CFLAGS --target=i686-pc-windows-msvc -O2 -fms-extensions -fms-compatibility -fno-exceptions -fno-rtti
                -march=i686 -mno-sse -nostdlibinc -I${OSB_GUEST}/crt -I${L1_SRC} -DCHECKSUM=2273873307UL -D_WIN32 -w)
  add_custom_command(
    OUTPUT ${OSB_OUT}/level1.dll
    COMMAND ${CMAKE_COMMAND} -E make_directory ${OSB_OUT}
    COMMAND ${X86SB_DLLTOOL} -m i386 -d ${OSB_GUEST}/msvcrt.def -l ${OSB_OUT}/level1_msvcrt.lib
    COMMAND ${X86SB_CLANG} ${L1_CFLAGS} -c ${L1_SRC}/level1.cpp -o ${OSB_OUT}/level1.obj
    COMMAND ${X86SB_CLANG} ${L1_CFLAGS} -c ${OSB_GUEST}/crt0.cpp -o ${OSB_OUT}/crt0.obj
    COMMAND ${X86SB_LLD_LINK} /dll /nodefaultlib /entry:_DllMainCRTStartup
            /def:${PROJECT_SOURCE_DIR}/scripts/MS-LVL.DEF /out:${OSB_OUT}/level1.dll
            ${OSB_OUT}/crt0.obj ${OSB_OUT}/level1.obj ${OSB_OUT}/level1_msvcrt.lib
    DEPENDS ${OSB_OUT}/level1_verified.stamp ${L1_FILES} ${OSB_GUEST}/crt0.cpp ${OSB_GUEST}/msvcrt.def
            ${PROJECT_SOURCE_DIR}/scripts/MS-LVL.DEF
    COMMENT "Building the original level1 script as a Win32 DLL for osiris_sandbox_tests"
  )
  add_custom_target(osiris_sandbox_level1_dll DEPENDS ${OSB_OUT}/level1.dll)
  add_dependencies(osiris_sandbox_tests osiris_sandbox_level1_dll)
  target_compile_definitions(osiris_sandbox_tests PRIVATE
    LEVEL1_DLL="${OSB_OUT}/level1.dll"
    LEVEL1_SOURCE="${L1_SRC}/level1.cpp"
    LEVEL1_MSG="${L1_SRC}/level1.msg"
  )
endif()

gtest_discover_tests(osiris_sandbox_tests)
