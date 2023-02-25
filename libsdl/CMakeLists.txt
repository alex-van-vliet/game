add_library(libsdl INTERFACE libsdl/sdl.hh)
target_include_directories(libsdl INTERFACE .)

find_package(SDL2 REQUIRED)
target_link_libraries(libsdl INTERFACE SDL2::SDL2)
