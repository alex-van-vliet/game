#ifndef LIBSDL_SDL_HH
#define LIBSDL_SDL_HH

#include <SDL.h>
#include <memory>
#include "function-wrapper.hh"
#include "type-wrapper.hh"
#include "error-checker-wrapper.hh"
#include "smart-pointer-remover-wrapper.hh"

namespace {
    // Create a functor to get the SDL error
    using GetError = decltype([]() { return SDL_GetError(); });

    // Helper to create a wrapper that only supports automatic smart pointer to raw pointer conversion
    template<auto FUN>
    using UncheckedWrapper = libsdl::SmartPointerRemoverWrapper<libsdl::FunctionWrapper<FUN>>;

    /**
     * Helper to automatically determine the correct way to test a value and call the error functor
     * @tparam T The type of the checked value
     * @tparam ERROR A functor to get the error message
     */
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

    /**
     * Helper to create a wrapper that adds the error checker if the return type is not void
     * @tparam FUN The function to wrap
     * @tparam OUT The output type of the function
     * @tparam ERROR The error message functor
     */
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

    // Helper to create a wrapper that supports automatic error checking and smart pointer to raw pointer conversion
    template<auto FUN, typename ERROR=GetError>
    using Wrapper = typename WrapperHelper<FUN, typename libsdl::FunctionWrapper<FUN>::out_t, ERROR>::type;

    // Helper to create a wrapper that supports type conversion, automatic error checking and
    // smart pointer to raw pointer conversion
    template<auto FUN, typename OUT, typename ERROR=GetError>
    using TypedWrapper = libsdl::TypeWrapper<Wrapper<FUN, ERROR>, OUT>;
}

namespace libsdl {
    class DefaultDeleter {
    public:
        auto operator()(SDL_Window *p) const noexcept {
            SDL_DestroyWindow(p);
        }
    };

    template<typename T>
    using unique_ptr = std::unique_ptr<T, DefaultDeleter>;

    // The SDL Wrappers
    constexpr Wrapper<SDL_Init> init;
    constexpr TypedWrapper<SDL_CreateWindow, unique_ptr<SDL_Window>> create_window;
    constexpr Wrapper<SDL_GetWindowSurface> get_window_surface;
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
