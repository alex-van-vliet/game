#include <catch2/catch_all.hpp>
#include <libsdl/function-wrapper.hh>

using namespace libsdl;

int my_function(int value) {
    return value;
}

void my_void_function(int value, int *res) {
    *res = value;
}

constexpr FunctionWrapper<my_function> my_wrapped_function;
constexpr FunctionWrapper<my_void_function> my_wrapped_void_function;

TEST_CASE("type wrapped functions should be callable like the inner function", "[libsdl]") {
    REQUIRE(std::is_same_v<decltype(my_wrapped_function)::out_t, int>);

    auto ptr = my_wrapped_function(2);

    REQUIRE(ptr == 2);

    REQUIRE(std::is_same_v<decltype(my_wrapped_void_function)::out_t, void>);

    int res;
    my_wrapped_void_function(3, &res);

    REQUIRE(res == 3);
}
