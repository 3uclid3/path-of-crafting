#include <doctest/doctest.h>

#include <poc/algorithm.hpp>
#include <poc/types.hpp>

#include <vector>

namespace poc { namespace {

TEST_CASE("count_until: stops when predicate satisfied")
{
    constexpr string_view input = "line1\nline2\nline3\nline4\n";

    const auto count = count_until(input.begin(), input.end(), '\n', [](auto seen) {
        return seen >= 4;
    });

    CHECK_EQ(count, 4);
}

TEST_CASE("count_until: returns occurrences when predicate never satisfied")
{
    constexpr string_view input = "a,b,c";

    const auto count = count_until(input.begin(), input.end(), ',', [](auto seen) {
        return seen >= 5;
    });

    CHECK_EQ(count, 2);
}

TEST_CASE("count_until: returns zero when value absent")
{
    const std::vector<int> numbers{1, 2, 3};

    const auto count = count_until(numbers.begin(), numbers.end(), 4, [](auto) {
        return true;
    });

    CHECK_EQ(count, 0);
}

TEST_CASE("count_until: ranges overload forwards to iterator version")
{
    const std::vector<char> letters{'a', 'b', 'a', 'c'};

    const auto count = ranges::count_until(letters, 'a', [](auto seen) {
        return seen >= 2;
    });

    CHECK_EQ(count, 2);
}

}} // namespace poc
