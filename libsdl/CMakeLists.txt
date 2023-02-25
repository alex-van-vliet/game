add_library(libsdl INTERFACE libsdl/sdl.hh libsdl/function-wrapper.hh libsdl/type-wrapper.hh libsdl/wrapper.hh
        libsdl/error-checker-wrapper.hh)
target_include_directories(libsdl INTERFACE .)

find_package(SDL2 REQUIRED)
target_link_libraries(libsdl INTERFACE SDL2::SDL2)

add_executable(libsdl_tests tests/function-wrapper.cc tests/type-wrapper.cc tests/error-checker-wrapper.cc)
target_link_libraries(libsdl_tests PRIVATE Catch2::Catch2WithMain libsdl)
catch_discover_tests(libsdl_tests)
