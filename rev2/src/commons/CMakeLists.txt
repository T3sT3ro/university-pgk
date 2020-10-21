
file(GLOB T3Commons_HPP third_party/stb_image.h *.hpp)

file(GLOB T3Commons_CPP *.cpp)

add_library(T3Commons STATIC ${T3Commons_CPP})

install(TARGETS T3Commons DESTINATION lib)
install(FILES ${T3Commons_HPP} DESTINATION include)