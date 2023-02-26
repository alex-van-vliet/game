#ifndef LIBSDL_ERROR_CHECKER_WRAPPER_HH
#define LIBSDL_ERROR_CHECKER_WRAPPER_HH

#include <utility>
#include "function-wrapper.hh"

namespace libsdl {
    template <typename T, typename VALUE_TYPE>
    concept ErrorChecker = requires(T t, VALUE_TYPE v) { t(v); };

    /**
     * Wrapper to check if the function returns an error
     *
     * @tparam FUN The function to wrap
     * @tparam CHECKER The function that checks the value
     */
    template <FunctionWrapperConcept FUN,
              ErrorChecker<typename FUN::out_t> CHECKER>
    requires(!std::is_same_v<typename FUN::out_t, void>)
    class ErrorCheckerWrapper {
      public:
        using out_t = FUN::out_t;

        constexpr out_t operator()(auto&&... arguments) const {
            auto res = FUN{}(std::forward<decltype(arguments)>(arguments)...);

            CHECKER{}(res);

            return res;
        }
    };
} // namespace libsdl

#endif // LIBSDL_ERROR_CHECKER_WRAPPER_HH
