

file(GLOB AGL3_HPP *.hpp)
file(GLOB AGL3_CPP *.cpp)

add_executable(AGL3-example ${AGL3_CPP})

target_include_directories(AGL3-example PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(AGL3-example epoxy)