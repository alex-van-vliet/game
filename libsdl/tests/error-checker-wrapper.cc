#include <catch2/catch_all.hpp>
#include <libsdl/error-checker-wrapper.hh>

using namespace libsdl;

namespace {
    int my_function(int value) { return value; }

    constexpr ErrorCheckerWrapper<FunctionWrapper<my_function>,
                                  decltype([](int value) {
                                      if (value < 0)
                                          throw std::runtime_error(
                                              "Value should be non-negative.");
                                  })>
        my_wrapped_function;

    constexpr ErrorCheckerWrapper<FunctionWrapper<my_function>,
                                  decltype([](const int& value) {
                                      if (value < 0)
                                          throw std::runtime_error(
                                              "Value should be non-negative.");
                                  })>
        my_wrapped_function2;
} // namespace

TEST_CASE(
    "error checker wrapped functions should have the return value checked",
    "[libsdl]") {
    REQUIRE(std::is_same_v<decltype(my_wrapped_function)::out_t, int>);

    auto res = my_wrapped_function(2);

    REQUIRE(res == 2);

    REQUIRE_THROWS_MATCHES(
        my_wrapped_function(-1), std::runtime_error,
        Catch::Matchers::Message("Value should be non-negative."));

    REQUIRE_THROWS_MATCHES(
        my_wrapped_function2(-2), std::runtime_error,
        Catch::Matchers::Message("Value should be non-negative."));
}
