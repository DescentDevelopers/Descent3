if (NOT CMAKE_CROSSCOMPILING)
  add_executable(
      HogMaker
      HogMaker/HogFormat.cpp
      HogMaker/HogMaker.cpp
  )
  target_include_directories(HogMaker PRIVATE ${PROJECT_BINARY_DIR}/lib)

  install(TARGETS HogMaker
      RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
      COMPONENT HostTools
  )
endif()