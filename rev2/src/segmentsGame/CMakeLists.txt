

file(GLOB HPP *.hpp)
file(GLOB CPP *.cpp)
set(RESOURCES shaders)

add_executable(segments ${HPP} ${CPP})

target_include_directories(segments PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(segments epoxy)

file(COPY ${RESOURCES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )