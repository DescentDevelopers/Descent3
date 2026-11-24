# x86 sandbox: runs 32-bit Windows DLLs in tiny386, compiled to Wasm and back to C
# with wasm2c. The generated C in generated/ is rebuilt by regenerate.sh.

set(WASM2C_DIR ${PROJECT_SOURCE_DIR}/third_party/wasm2c)

add_library(x86sandbox STATIC
  host/x86sandbox.cpp
  host/x86sandbox.h
  host/x86sandbox_rt.c
  host/x86sandbox_rt.h
  generated/x86sandbox_wasm.c
  generated/x86sandbox_wasm.h
  ${WASM2C_DIR}/wasm-rt-impl.c
  ${WASM2C_DIR}/wasm-rt-mem-impl.c
  ${WASM2C_DIR}/wasm-rt-exceptions-impl.c
)

target_include_directories(x86sandbox
  PUBLIC host
  PRIVATE generated ${WASM2C_DIR}
)

# Explicit bounds checks and call-depth counting: no guard pages, no signal
# handlers and no large address-space reservation, on every platform.
target_compile_definitions(x86sandbox PRIVATE
  WASM_RT_USE_MMAP=0
  WASM_RT_MEMCHECK_BOUNDS_CHECK=1
  WASM_RT_STACK_DEPTH_COUNT=1
  WASM_RT_SKIP_SIGNAL_RECOVERY=1
)

# wasm2c output relies on these to stay conformant (and so to keep its checks)
# when optimized; see the wasm2c README.
set(WASM2C_SOURCES generated/x86sandbox_wasm.c ${WASM2C_DIR}/wasm-rt-impl.c ${WASM2C_DIR}/wasm-rt-mem-impl.c
                   ${WASM2C_DIR}/wasm-rt-exceptions-impl.c host/x86sandbox_rt.c)
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  set_source_files_properties(${WASM2C_SOURCES} PROPERTIES
    COMPILE_OPTIONS "-fno-optimize-sibling-calls;-frounding-math;-fsignaling-nans;-w")
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
  set_source_files_properties(${WASM2C_SOURCES} PROPERTIES
    COMPILE_OPTIONS "-fno-optimize-sibling-calls;-frounding-math;-w")
elseif(MSVC)
  set_source_files_properties(${WASM2C_SOURCES} PROPERTIES COMPILE_OPTIONS "/fp:strict;/w")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_link_libraries(x86sandbox PRIVATE m)
endif()
