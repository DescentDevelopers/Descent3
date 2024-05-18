set(HEADERS)
set(CPPS
  video_lnx.cpp)

add_library(ddvid_lnx STATIC ${HEADERS} ${CPPS})
target_link_libraries(ddvid_lnx PRIVATE
  linux
)
