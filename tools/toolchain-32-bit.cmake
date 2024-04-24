# CMake toolchain file to build 32-bit binaries on a 64-bit multiarch system.
# Call with -DCMAKE_TOOLCHAIN_FILE=...

function(add_m32_flag VAR_NAME)
  if(DEFINED ${VAR_NAME})
    if(NOT ${VAR_NAME} MATCHES "-m32")
      set(${VAR_NAME} "${${VAR_NAME}} -m32" PARENT_SCOPE)
    endif()
  else()
    set(${VAR_NAME} "-m32" PARENT_SCOPE)
  endif()
endfunction()


add_m32_flag(CMAKE_C_FLAGS)
add_m32_flag(CMAKE_CXX_FLAGS)
add_m32_flag(CMAKE_EXE_LINKER_FLAGS)
add_m32_flag(CMAKE_MODULE_LINKER_FLAGS)
add_m32_flag(CMAKE_SHARED_LINKER_FLAGS)
