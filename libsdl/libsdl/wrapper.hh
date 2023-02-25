#ifndef LIBSDL_WRAPPER_HH
#define LIBSDL_WRAPPER_HH

#include <stdexcept>
#include <memory>

namespace {
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
    template<FunctionWrapperConcept FUN, bool CHECK = true>
    class Wrapper {
    public:
        using out_t = FUN::out_t;

        constexpr void operator()(auto &&... arguments) const requires(std::is_void_v<out_t>) {
            FUN{}(std::forward<decltype(as_raw(arguments))>(as_raw(arguments))...);
        }

        constexpr out_t operator()(auto &&... arguments) const requires (!CHECK &&
                                                                         !std::is_void_v<out_t>) {
            return FUN{}(std::forward<decltype(as_raw(arguments))>(as_raw(arguments))...);
        }

        constexpr out_t operator()(auto &&... arguments) const requires (CHECK &&
                                                                         std::is_pointer_v<out_t>) {
            auto res = FUN{}(std::forward<decltype(as_raw(arguments))>(as_raw(arguments))...);

            if (res == nullptr) {
                throw std::runtime_error(SDL_GetError());
            }

            return res;
        }

        constexpr out_t operator()(auto &&... arguments) const requires (CHECK &&
                                                                         std::is_integral_v<out_t> &&
                                                                         std::is_signed_v<out_t>) {
            auto res = FUN{}(std::forward<decltype(as_raw(arguments))>(as_raw(arguments))...);

            if (res < 0) {
                throw std::runtime_error(SDL_GetError());
            }

            return res;
        }
    };
}

#endif //LIBSDL_WRAPPER_HH
