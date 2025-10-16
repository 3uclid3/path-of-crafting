#include <doctest/doctest.h>

#include <poc/string/string_slice.hpp>

#include <poc/string/test_string.hpp>

namespace poc { namespace {

TEST_CASE_TEMPLATE("basic_string_slice: default constructor", T, char, char8_t, wchar_t)
{
    using string_slice = basic_string_slice<T>;

    const string_slice slice;

    CHECK_EQ(slice.offset(), 0);
    CHECK_EQ(slice.size(), 0);
}

TEST_CASE_TEMPLATE("basic_string_slice: constructor with offset and size", T, char, char8_t, wchar_t)
{
    using string_slice = basic_string_slice<T>;

    const string_slice slice{5, 10};

    CHECK_EQ(slice.offset(), 5);
    CHECK_EQ(slice.size(), 10);
}

TEST_CASE_TEMPLATE("basic_string_slice: view", T, char, char8_t, wchar_t)
{
    using string_slice = basic_string_slice<T>;
    using string_view = std::basic_string_view<T>;

    const string_view str{TEST_STRING("Hello, World!")};
    const string_view expected_view{TEST_STRING("World")};

    const string_slice slice{7, 5};

    CHECK_EQ(slice.view(str), expected_view);
}

TEST_CASE_TEMPLATE("basic_string_slice: view when empty slice", T, char, char8_t, wchar_t)
{
    using string_slice = basic_string_slice<T>;
    using string_view = std::basic_string_view<T>;

    const string_view str{TEST_STRING("Hello, World!")};

    const string_slice slice;

    CHECK_EQ(slice.view(str), string_view{});
}

TEST_CASE_TEMPLATE("basic_string_slice: view when empty string", T, char, char8_t, wchar_t)
{
    using string_slice = basic_string_slice<T>;
    using string_view = std::basic_string_view<T>;

    const string_view str;

    const string_slice slice;

    CHECK_EQ(slice.view(str), string_view{});
}

}} // namespace poc
