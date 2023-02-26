#ifndef LIBSDL_FUNCTION_WRAPPER_HH
#define LIBSDL_FUNCTION_WRAPPER_HH

#include <utility>

namespace {
    // Forward-declaration only, used for decltype to get the return type of a
    // function pointer
    template <typename Out, typename... INS>
    Out return_type(Out (*)(INS...));
} // namespace

namespace libsdl {
    /**
     * Concept representing any function wrapper
     * @tparam T The wrapper
     */
    template <typename T>
    // clang-format off
    // Disabling clang format here as the requires alignment is only supported on clang-format 16+
    concept FunctionWrapperConcept = requires(const T t) {
        typename T::out_t;
        // Currently not possible to check that the operator() exists as it is templated
        // &T::operator();
    };
    // clang-format on

    /**
     * Wraps a function into an object (base wrapper)
     * @tparam Function The function
     */
    template <auto Function>
    class FunctionWrapper {
      public:
        using out_t = decltype(return_type(Function));

        constexpr out_t operator()(auto&&... arguments) const {
            return Function(std::forward<decltype(arguments)>(arguments)...);
        }
    };
} // namespace libsdl

#endif // LIBSDL_FUNCTION_WRAPPER_HH
