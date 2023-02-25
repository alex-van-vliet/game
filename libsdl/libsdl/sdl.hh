#ifndef LIBSDL_SDL_HH
#define LIBSDL_SDL_HH

#include <SDL.h>
#include <stdexcept>
#include <memory>

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

    // Helper class to get a T* from a unique_ptr<T>, and itself from everything else
    template<typename T>
    class AsRaw {
    public:
        static constexpr auto call(auto &&t) {
            return t;
        }
    };

    template<typename T, typename D>
    class AsRaw<std::unique_ptr<T, D> &> {
    public:
        static constexpr auto call(std::unique_ptr<T, D> &t) {
            return t.get();
        }
    };

    template<typename T, typename D>
    class AsRaw<const std::unique_ptr<T, D> &> {
    public:
        static constexpr auto call(const std::unique_ptr<T, D> &t) {
            return t.get();
        }
    };

    template<typename T>
    auto as_raw(T &&t) {
        return AsRaw<T>::call(std::forward<T>(t));
    }

    // Forward-declaration only, used for decltype to get the return type of a function pointer
    template<typename OUT, typename ...INS>
    OUT return_type(OUT (*)(INS...));
}

namespace libsdl {
    /**
     * Wrapper creator for SDL functions that:
     * - handles error checking,
     * - automatically transforms unique_ptr to raw pointers,
     * - handles return values: void, signed integral types, and pointers.
     *
     * @tparam FUN The function to wrap
     * @tparam CHECK Whether to check the return value for errors (only used if the return type is not void)
     */
    template<auto FUN, bool CHECK = true>
    class Wrapper {
        using out_t = decltype(return_type(FUN));
    public:
        constexpr void operator()(auto &&... arguments) const requires(std::is_void_v<out_t>) {
            FUN(std::forward<decltype(as_raw(arguments))>(as_raw(arguments))...);
        }

        constexpr out_t operator()(auto &&... arguments) const requires (!CHECK &&
                                                                         !std::is_void_v<out_t>) {
            return FUN(std::forward<decltype(as_raw(arguments))>(as_raw(arguments))...);
        }

        constexpr out_t operator()(auto &&... arguments) const requires (CHECK &&
                                                                         std::is_pointer_v<out_t>) {
            auto res = FUN(std::forward<decltype(as_raw(arguments))>(as_raw(arguments))...);

            if (res == nullptr) {
                throw std::runtime_error(SDL_GetError());
            }

            return res;
        }

        constexpr out_t operator()(auto &&... arguments) const requires (CHECK &&
                                                                         std::is_integral_v<out_t> &&
                                                                         std::is_signed_v<out_t>) {
            auto res = FUN(std::forward<decltype(as_raw(arguments))>(as_raw(arguments))...);

            if (res < 0) {
                throw std::runtime_error(SDL_GetError());
            }

            return res;
        }
    };

    /**
     * Secondary wrapper to change the return type of the function
     *
     * @tparam WRAPPER Wrapped function
     * @tparam OUT The new return type
     */
    template<typename WRAPPER, typename OUT>
    class TypeWrapper {
    public:
        constexpr OUT operator()(auto &&... arguments) const {
            return OUT{WRAPPER{}(std::forward<decltype(arguments)>(arguments)...)};
        }
    };

    // The SDL Wrappers
    constexpr Wrapper<SDL_Init> init;
    using Window = std::unique_ptr<SDL_Window, Deleter<SDL_DestroyWindow>>;
    constexpr TypeWrapper<Wrapper<SDL_CreateWindow, true>, Window> create_window;
    using WindowSurface = std::unique_ptr<SDL_Surface, NoDeleter>;
    constexpr TypeWrapper<Wrapper<SDL_GetWindowSurface>, WindowSurface> get_window_surface;
    constexpr Wrapper<SDL_FillRect> fill_rect;
    constexpr Wrapper<SDL_MapRGB, false> map_rgb;
    constexpr Wrapper<SDL_UpdateWindowSurface> update_window_surface;
    constexpr Wrapper<SDL_PollEvent, false> poll_event;
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
