set(HEADERS)
set(CPPS
  networking.cpp
  $<$<PLATFORM_ID:Windows>:
    directplay.cpp
  >
)

if(WIN32)
  set(PLATFORM_LIBS ${DXGUID_LIBRARY})
endif()

add_library(networking STATIC ${HEADERS} ${CPPS})
target_link_libraries(networking PRIVATE
  ddebug
  ddio
  logger
  mem
  module
  SDL3::SDL3
  ${PLATFORM_LIBS}
)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
