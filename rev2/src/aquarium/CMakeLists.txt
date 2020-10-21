file(GLOB aquarium_HPP *.hpp)
file(GLOB aquarium_CPP *.cpp)

add_executable(aquarium ${aquarium_CPP})

link_libraries(commons)
include_directories(${PROJECT_SOURCE_DIR}/src/commons)

target_include_directories(aquarium PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})