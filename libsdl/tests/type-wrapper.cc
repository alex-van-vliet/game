#include <catch2/catch_all.hpp>
#include <libsdl/type-wrapper.hh>

using namespace libsdl;

namespace {
    int* my_function(int value) { return new int(value); }

    constexpr TypeWrapper<FunctionWrapper<my_function>, std::unique_ptr<int>>
        my_wrapped_function;
} // namespace

TEST_CASE("type wrapped functions should have the requested return type",
          "[libsdl]") {
    REQUIRE(std::is_same_v<decltype(my_wrapped_function)::out_t,
                           std::unique_ptr<int>>);

    auto ptr = my_wrapped_function(2);

    REQUIRE(ptr != nullptr);
    REQUIRE(*ptr == 2);
}
