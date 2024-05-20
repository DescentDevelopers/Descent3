add_subdirectory(libacm)
add_subdirectory(md5)
add_subdirectory(stb)

add_custom_target(LicenseFiles ALL
  COMMENT "Copying license files for third-party dependencies"
)

# Function to add third-party license
function(add_third_party_license license_name)
  add_custom_command(TARGET LicenseFiles POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:Descent3>/licenses"
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_SOURCE_DIR}/third_party/${license_name}"
    "$<TARGET_FILE_DIR:Descent3>/licenses/${license_name}"
    COMMENT "Copying ${license_name}"
  )
endfunction()

add_custom_command(TARGET LicenseFiles POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  "${CMAKE_SOURCE_DIR}/LICENSE"
  "$<TARGET_FILE_DIR:Descent3>/GPL-3.txt"
  COMMENT "Copying Descent3 license"
)

add_third_party_license(libacm_LICENSE.txt)
add_third_party_license(md5_LICENSE.txt)
add_third_party_license(stb_LICENSE.txt)