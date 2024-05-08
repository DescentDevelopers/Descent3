set(HEADERS md5.h)
set(CPPS)

add_library(md5 INTERFACE ${HEADERS} ${CPPS})

target_link_libraries(md5 INTERFACE OpenSSL::Crypto)
