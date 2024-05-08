# Custom triplet for vcpkg that builds some packages statically

set(VCPKG_TARGET_ARCHITECTURE x86)

# Link statically selected ports
if(${PORT} MATCHES "zlib")
  set(VCPKG_CRT_LINKAGE static)
  set(VCPKG_LIBRARY_LINKAGE static)
else()
  set(VCPKG_CRT_LINKAGE dynamic)
  set(VCPKG_LIBRARY_LINKAGE dynamic)
endif()