#ifndef LIBSDL_SDL_HH
#define LIBSDL_SDL_HH

#include <memory>
#include <SDL.h>
#include "error-checker-wrapper.hh"
#include "function-wrapper.hh"
#include "smart-pointer-remover-wrapper.hh"
#include "type-wrapper.hh"

namespace {
    // Create a functor to get the SDL error
    using GetError = decltype([]() { return SDL_GetError(); });

    // Helper to create a wrapper that only supports automatic smart pointer to
    // raw pointer conversion
    template <auto Function>
    using UncheckedWrapper =
        libsdl::SmartPointerRemoverWrapper<libsdl::FunctionWrapper<Function>>;

    /**
     * Helper to automatically determine the correct way to test a value and
     * call the error functor
     * @tparam T The type of the checked value
     * @tparam Error A functor to get the error message
     */
    template <typename T, typename Error>
    class Checker {};

    template <std::signed_integral T, typename Error>
    class Checker<T, Error> {
      public:
        void operator()(auto res) {
            if (res < 0) {
                throw std::runtime_error(Error{}());
            }
        }
    };

    template <typename T, typename Error>
    class Checker<T*, Error> {
      public:
        void operator()(const T* res) {
            if (res == nullptr) {
                throw std::runtime_error(Error{}());
            }
        }
    };

    /**
     * Helper to create a wrapper that adds the error checker if the return type
     * is not void
     * @tparam Function The function to wrap
     * @tparam Out The output type of the function
     * @tparam Error The error message functor
     */
    template <auto Function, typename Out, typename Error>
    class WrapperHelper {
      public:
        using type = libsdl::ErrorCheckerWrapper<UncheckedWrapper<Function>,
                                                 Checker<Out, Error>>;
    };

    template <auto Function, typename Error>
    class WrapperHelper<Function, void, Error> {
      public:
        using type = UncheckedWrapper<Function>;
    };

    // Helper to create a wrapper that supports automatic error checking and
    // smart pointer to raw pointer conversion
    template <auto Function, typename Error = GetError>
    using Wrapper = typename WrapperHelper<
        Function, typename libsdl::FunctionWrapper<Function>::out_t,
        Error>::type;

    // Helper to create a wrapper that supports type conversion, automatic error
    // checking and smart pointer to raw pointer conversion
    template <auto Function, typename Out, typename Error = GetError>
    using TypedWrapper = libsdl::TypeWrapper<Wrapper<Function, Error>, Out>;
} // namespace

namespace libsdl {
    class DefaultDeleter {
      public:
        auto operator()(SDL_Window* p) const noexcept { SDL_DestroyWindow(p); }
    };

    template <typename T>
    using unique_ptr = std::unique_ptr<T, DefaultDeleter>;

    // The SDL Wrappers
    constexpr Wrapper<SDL_Init> init;
    constexpr TypedWrapper<SDL_CreateWindow, unique_ptr<SDL_Window>>
        create_window;
    constexpr Wrapper<SDL_GetWindowSurface> get_window_surface;
    constexpr Wrapper<SDL_FillRect> fill_rect;
    constexpr UncheckedWrapper<SDL_MapRGB> map_rgb;
    constexpr Wrapper<SDL_UpdateWindowSurface> update_window_surface;
    constexpr UncheckedWrapper<SDL_PollEvent> poll_event;
    constexpr Wrapper<SDL_Quit> quit;

    // Just a simple Context class to use RAII for init and quit
    class Context {
        bool should_delete;

      public:
        explicit Context(Uint32 flags) : should_delete{true} { init(flags); }

        ~Context() {
            if (should_delete)
                quit();
        }

        Context(const Context&) = delete;

        Context& operator=(const Context&) = delete;

        Context(Context&& other) noexcept : should_delete{false} {
            swap(*this, other);
        }

        Context& operator=(Context&& other) noexcept {
            swap(*this, other);

            return *this;
        }

        friend void swap(Context& first, Context& second) {
            using std::swap;
            swap(first.should_delete, second.should_delete);
        }
    };
} // namespace libsdl

#endif // LIBSDL_SDL_HH
