#ifndef LIBSDL_SDL_HH
#define LIBSDL_SDL_HH

#include <SDL.h>
#include <memory>
#include "function-wrapper.hh"
#include "type-wrapper.hh"
#include "error-checker-wrapper.hh"
#include "smart-pointer-remover-wrapper.hh"

namespace {
    // Creates a deleter for unique_ptr from a function
    template<auto FUN>
    using Deleter = decltype([](auto *arg) { FUN(arg); });
    // Creates a deleter for unique_ptr which does nothing
    using NoDeleter = decltype([](auto *arg) {});

    // Create a functor to get the SDL error
    using GetError = decltype([]() { return SDL_GetError(); });

    template<auto FUN>
    using UncheckedWrapper = libsdl::SmartPointerRemoverWrapper<libsdl::FunctionWrapper<FUN>>;

    template<typename T, typename ERROR>
    class Checker {
    };

    template<std::signed_integral T, typename ERROR>
    class Checker<T, ERROR> {
    public:
        void operator()(auto res) {
            if (res < 0) {
                throw std::runtime_error(ERROR{}());
            }
        }
    };

    template<typename T, typename ERROR>
    class Checker<T *, ERROR> {
    public:
        void operator()(const T *res) {
            if (res == nullptr) {
                throw std::runtime_error(ERROR{}());
            }
        }
    };

    template<auto FUN, typename OUT, typename ERROR>
    class WrapperHelper {
    public:
        using type = libsdl::ErrorCheckerWrapper<UncheckedWrapper<FUN>, Checker<OUT, ERROR>>;
    };

    template<auto FUN, typename ERROR>
    class WrapperHelper<FUN, void, ERROR> {
    public:
        using type = UncheckedWrapper<FUN>;
    };

    template<auto FUN, typename ERROR=GetError>
    using Wrapper = typename WrapperHelper<FUN, typename libsdl::FunctionWrapper<FUN>::out_t, ERROR>::type;

    template<auto FUN, typename OUT, typename ERROR=GetError>
    using TypedWrapper = libsdl::TypeWrapper<Wrapper<FUN, ERROR>, OUT>;
}

namespace libsdl {
    // The SDL Wrappers
    constexpr Wrapper<SDL_Init> init;
    using Window = std::unique_ptr<SDL_Window, Deleter<SDL_DestroyWindow>>;
    constexpr TypedWrapper<SDL_CreateWindow, Window> create_window;
    using WindowSurface = std::unique_ptr<SDL_Surface, NoDeleter>;
    constexpr TypedWrapper<SDL_GetWindowSurface, WindowSurface> get_window_surface;
    constexpr Wrapper<SDL_FillRect> fill_rect;
    constexpr UncheckedWrapper<SDL_MapRGB> map_rgb;
    constexpr Wrapper<SDL_UpdateWindowSurface> update_window_surface;
    constexpr UncheckedWrapper<SDL_PollEvent> poll_event;
    constexpr Wrapper<SDL_Quit> quit;

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
