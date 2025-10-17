#include <doctest/doctest.h>

#include <poc.core/utility/tagged.hpp>

#include <poc.core/types.hpp>

namespace poc { namespace {

struct int_tag;
struct integer_tag;

struct integer
{
    integer() noexcept = default;
    integer(int v) noexcept
        : value(v)
    {
    }

    explicit operator int() const noexcept
    {
        return value;
    }

    auto operator<=>(const integer& other) const noexcept -> std::strong_ordering = default;
    int value{0};
};

template<typename T>
struct tag_of
{};

template<>
struct tag_of<int>
{
    using type = int_tag;
};

template<>
struct tag_of<integer>
{
    using type = integer_tag;
};

template<typename T>
using tag_of_t = typename tag_of<T>::type;

TEST_CASE_TEMPLATE("tagged: default ctor", T, int, integer)
{
    using int_t = tagged<T, tag_of_t<T>>;

    int_t value;

    CHECK_EQ(static_cast<T>(value), T{});
    CHECK_EQ(value.underlying(), T{});
}

TEST_CASE_TEMPLATE("tagged: compare", T, int, integer)
{
    using int_t = tagged<T, tag_of_t<T>>;

    CHECK_GT(int_t{42}, int_t{41});
    CHECK_GE(int_t{42}, int_t{41});

    CHECK_LT(int_t{42}, int_t{43});
    CHECK_LE(int_t{42}, int_t{43});
}

}} // namespace poc
