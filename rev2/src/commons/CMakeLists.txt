

file(GLOB commons_HPP third_party/stb_image.h *.hpp)
file(GLOB commons_CPP *.cpp)

add_library(commons STATIC ${commons_CPP})

target_include_directories(commons PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})