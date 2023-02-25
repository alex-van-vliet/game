#ifndef LIBSDL_SDL_HH
#define LIBSDL_SDL_HH

#include <SDL.h>
#include <memory>
#include "function-wrapper.hh"
#include "type-wrapper.hh"
#include "wrapper.hh"

namespace {
    // Creates a deleter for unique_ptr from a function
    template<auto FUN>
    class Deleter {
    public:
        template<typename T>
        constexpr void operator()(T *arg) const {
            FUN(arg);
        }
    };

    // Creates a deleter for unique_ptr which does nothing
    class NoDeleter {
    public:
        template<typename T>
        constexpr void operator()(T *arg) const {}
    };
}

namespace libsdl {
    // The SDL Wrappers
    constexpr Wrapper<FunctionWrapper<SDL_Init>> init;
    using Window = std::unique_ptr<SDL_Window, Deleter<SDL_DestroyWindow>>;
    constexpr TypeWrapper<Wrapper<FunctionWrapper<SDL_CreateWindow>, true>, Window> create_window;
    using WindowSurface = std::unique_ptr<SDL_Surface, NoDeleter>;
    constexpr TypeWrapper<Wrapper<FunctionWrapper<SDL_GetWindowSurface>>, WindowSurface> get_window_surface;
    constexpr Wrapper<FunctionWrapper<SDL_FillRect>> fill_rect;
    constexpr Wrapper<FunctionWrapper<SDL_MapRGB>, false> map_rgb;
    constexpr Wrapper<FunctionWrapper<SDL_UpdateWindowSurface>> update_window_surface;
    constexpr Wrapper<FunctionWrapper<SDL_PollEvent>, false> poll_event;
    constexpr Wrapper<FunctionWrapper<SDL_Quit>> quit;

    // Just a simple Context class to use RAII for init and quit
    class Context {
    public:
        explicit Context(Uint32 flags) {
            init(flags);
        }

        ~Context() {
            quit();
        }
    };
}

#endif //LIBSDL_SDL_HH
