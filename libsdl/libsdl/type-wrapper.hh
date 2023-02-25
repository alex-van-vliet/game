#ifndef LIBSDL_TYPE_WRAPPER_HH
#define LIBSDL_TYPE_WRAPPER_HH

#include <utility>
#include "function-wrapper.hh"

namespace libsdl {
    /**
     * Wrapper to change the return type of the function
     *
     * @tparam FUN The function to wrap
     * @tparam OUT The new return type
     */
    template<FunctionWrapperConcept FUN, typename OUT>
    class TypeWrapper {
    public:
        using out_t = OUT;

        constexpr OUT operator()(auto &&... arguments) const {
            return OUT{FUN{}(std::forward<decltype(arguments)>(arguments)...)};
        }
    };
}

#endif //LIBSDL_TYPE_WRAPPER_HH
