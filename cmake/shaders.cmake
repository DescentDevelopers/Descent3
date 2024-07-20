file(WRITE ${shaders_h} "#pragma once
#include <string_view>
namespace shaders {
")

set(idx 0)
set(idx_is_path FALSE)
while(${idx} LESS ${CMAKE_ARGC})
    if(idx_is_path)
        set(shader_path ${CMAKE_ARGV${idx}})
        cmake_path(GET shader_path STEM shader_name)
        file(READ ${shader_path} content)
        file(APPEND ${shaders_h} "
inline constexpr std::string_view ${shader_name} = R\"shader(${content}
)shader\";
")
    else()
        if(${CMAKE_ARGV${idx}} STREQUAL "--")
            set(idx_is_path TRUE)
        endif()
    endif()
    math(EXPR idx "${idx} + 1")
endwhile()

foreach(idx RANGE ${CMAKE_ARGC})
endforeach()

file(APPEND ${shaders_h} "
} // namespace shaders
")

