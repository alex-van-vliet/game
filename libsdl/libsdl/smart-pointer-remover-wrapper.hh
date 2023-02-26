#ifndef LIBSDL_SMART_POINTER_REMOVER_WRAPPER_HH
#define LIBSDL_SMART_POINTER_REMOVER_WRAPPER_HH

#include <memory>
#include "function-wrapper.hh"

namespace libsdl {
    namespace detail {
        // Helper class to get a T* from a unique_ptr<T>, and itself from
        // everything else
        template <typename T>
        class AsRaw {
          public:
            static constexpr bool implemented = false;
        };

        template <typename T, typename D>
        class AsRaw<std::unique_ptr<T, D>> {
          public:
            static constexpr bool implemented = true;

            using in_t = std::unique_ptr<T, D>;
            using out_t = typename in_t::pointer;

            static constexpr out_t call(in_t& t) { return t.get(); }

            static constexpr out_t call(const in_t& t) { return t.get(); }

            static constexpr out_t call(in_t&& t) { return t.get(); }
        };

        auto as_raw(auto&& in) {
            using as_raw_t = AsRaw<std::remove_cvref_t<decltype(in)>>;
            if constexpr (as_raw_t::implemented) {
                return AsRaw<std::remove_cvref_t<decltype(in)>>::call(
                    std::forward<decltype(in)>(in));
            } else {
                return in;
            }
        }
    } // namespace detail

    /**
     * Wrapper to remove the smart pointers in the arguments
     *
     * @tparam FUN The function to wrap
     * @tparam OUT The new return type
     */
    template <FunctionWrapperConcept FUN>
    class SmartPointerRemoverWrapper {
      public:
        using out_t = FUN::out_t;

        constexpr out_t operator()(auto&&... arguments) const {
            return FUN{}(detail::as_raw(
                std::forward<decltype(arguments)>(arguments))...);
        }
    };
} // namespace libsdl

#endif // LIBSDL_SMART_POINTER_REMOVER_WRAPPER_HH
