#ifndef LIBSDL_ERROR_CHECKER_WRAPPER_HH
#define LIBSDL_ERROR_CHECKER_WRAPPER_HH

#include <utility>
#include "function-wrapper.hh"

namespace libsdl {
    template <typename T, typename ValueType>
    concept ErrorChecker = requires(T t, ValueType v) { t(v); };

    /**
     * Wrapper to check if the function returns an error
     *
     * @tparam Function The function to wrap
     * @tparam Checker The function that checks the value
     */
    template <FunctionWrapperConcept Function,
              ErrorChecker<typename Function::out_t> Checker>
    requires(!std::is_same_v<typename Function::out_t, void>)
    class ErrorCheckerWrapper {
      public:
        using out_t = Function::out_t;

        constexpr out_t operator()(auto&&... arguments) const {
            auto res =
                Function{}(std::forward<decltype(arguments)>(arguments)...);

            Checker{}(res);

            return res;
        }
    };
} // namespace libsdl

#endif // LIBSDL_ERROR_CHECKER_WRAPPER_HH
