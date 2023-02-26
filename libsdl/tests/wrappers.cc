#include <catch2/catch_all.hpp>
#include <libsdl/wrappers.hh>

using namespace libsdl;

namespace {
    using TestGetError = decltype([]() { return "Correct error was thrown."; });

    int my_int(int v) { return v; }

    constexpr Wrapper<my_int, TestGetError> my_wrapped_int;

    int* my_ptr(int* ptr) { return ptr; }

    constexpr Wrapper<my_ptr, TestGetError> my_wrapped_ptr;

    void my_void() {}

    constexpr Wrapper<my_void, TestGetError> my_wrapped_void;
} // namespace

TEST_CASE("wrapped functions should automatically throw", "[libsdl]") {
    REQUIRE_THROWS_MATCHES(
        my_wrapped_ptr(nullptr), std::runtime_error,
        Catch::Matchers::Message("Correct error was thrown."));

    int i = 2;
    REQUIRE(my_wrapped_ptr(&i) == &i);

    REQUIRE_THROWS_MATCHES(
        my_wrapped_int(-1), std::runtime_error,
        Catch::Matchers::Message("Correct error was thrown."));

    REQUIRE(my_wrapped_int(3) == 3);

    REQUIRE_NOTHROW(my_wrapped_void());
}

namespace {
    int my_function(const int* i) { return *i; }

    constexpr Wrapper<my_function> my_wrapped_function;
} // namespace

TEST_CASE("wrapped functions can be called with a unique pointer", "[libsdl]") {
    REQUIRE(my_wrapped_function(std::make_unique<int>(2)) == 2);
}

namespace {
    int* my_ptr_generator(bool success, int value) {
        if (!success) {
            return nullptr;
        }

        return new int(value);
    }

    constexpr TypedWrapper<my_ptr_generator, std::unique_ptr<int>, TestGetError>
        my_wrapped_ptr_generator;
} // namespace

TEST_CASE("typed wrapped functions convert to the given type", "[libsdl]") {
    REQUIRE(std::is_same_v<decltype(my_wrapped_ptr_generator(true, 1)),
                           std::unique_ptr<int>>);
}

TEST_CASE(
    "typed wrapped functions are error checked before transforming the type",
    "[libsdl]") {
    REQUIRE_THROWS_MATCHES(
        my_wrapped_ptr_generator(false, 1), std::runtime_error,
        Catch::Matchers::Message("Correct error was thrown."));

    REQUIRE(*my_wrapped_ptr_generator(true, 2) == 2);
}
