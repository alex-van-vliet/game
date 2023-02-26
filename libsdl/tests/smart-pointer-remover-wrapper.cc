#include <catch2/catch_all.hpp>
#include <libsdl/smart-pointer-remover-wrapper.hh>

using namespace libsdl;

namespace {
    int my_function(const int* value) { return *value; }

    constexpr SmartPointerRemoverWrapper<FunctionWrapper<my_function>>
        my_wrapped_function;
} // namespace

TEST_CASE("smart pointer remover wrapped functions should accept unique "
          "pointers as arguments",
          "[libsdl]") {
    REQUIRE(std::is_same_v<decltype(my_wrapped_function)::out_t, int>);

    auto res = my_wrapped_function(std::make_unique<int>(2));

    REQUIRE(res == 2);

    auto ptr = std::make_unique<int>(3);
    res = my_wrapped_function(ptr);

    REQUIRE(res == 3);

    ptr = std::make_unique<int>(4);
    res = my_wrapped_function(std::move(ptr));

    REQUIRE(res == 4);

    const auto cptr = std::make_unique<int>(5);
    res = my_wrapped_function(cptr);

    REQUIRE(res == 5);
}

TEST_CASE(
    "smart pointer remover wrapped functions should still accept raw pointers",
    "[libsdl]") {
    REQUIRE(std::is_same_v<decltype(my_wrapped_function)::out_t, int>);

    int value = 6;

    auto res = my_wrapped_function(&value);

    REQUIRE(res == 6);
}
