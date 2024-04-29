# In case Git is not available, default is empty string
set(GIT_HASH "")

find_package(Git QUIET)
if(GIT_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --always --dirty
    OUTPUT_VARIABLE REPO_GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )
  file(WRITE ${SOURCE_DIR}/git-hash.txt ${REPO_GIT_HASH})
else()
  # Try to read pregenerated file with commit hash on it (archive version of repository)
  if(EXISTS ${SOURCE_DIR}/git-hash.txt)
    file(READ ${SOURCE_DIR}/git-hash.txt D3_GIT_HASH)
  endif()
endif()

message(STATUS "Git hash is ${REPO_GIT_HASH}")
configure_file(
  ${SOURCE_DIR}/${TEMPLATE_HEADER}
  ${TARGET_DIR}/${TARGET_HEADER}
  @ONLY
)
