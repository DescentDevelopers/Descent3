set(HEADERS mem.h)
set(CPPS mem.cpp)

add_library(mem STATIC ${HEADERS} ${CPPS})
target_compile_definitions(mem PUBLIC
  $<$<BOOL:${ENABLE_MEM_RTL}>:MEM_USE_RTL>
)
target_link_libraries(mem PRIVATE
  logger
  misc
)
target_include_directories(mem PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/mem
  >
)
