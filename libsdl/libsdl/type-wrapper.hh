#ifndef LIBSDL_TYPE_WRAPPER_HH
#define LIBSDL_TYPE_WRAPPER_HH

#include <utility>
#include "function-wrapper.hh"

namespace libsdl {
    /**
     * Wrapper to change the return type of the function
     *
     * @tparam Function The function to wrap
     * @tparam Out The new return type
     */
    template <FunctionWrapperConcept Function, typename Out>
    class TypeWrapper {
      public:
        using out_t = Out;

        constexpr Out operator()(auto&&... arguments) const {
            return Out{
                Function{}(std::forward<decltype(arguments)>(arguments)...)};
        }
    };
} // namespace libsdl

#endif // LIBSDL_TYPE_WRAPPER_HH
