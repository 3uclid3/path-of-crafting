#include <doctest/doctest.h>

#include <poc.core/utility/bitmask.hpp>

#include <poc.core/types.hpp>

namespace poc {

namespace {

template<typename T>
struct bitmask_enum
{
    enum class enum_type : T
    {
        none = 0,

        flag_0 = 1 << 0,
        flag_1 = 1 << 1,
        flag_2 = 1 << 2
    };
};

template<typename T>
using bitmask_enum_t = typename bitmask_enum<T>::enum_type;

} // namespace

template<>
struct is_bitmask_enum<bitmask_enum_t<std::uint8_t>> : std::true_type
{};

template<>
struct is_bitmask_enum<bitmask_enum_t<std::uint16_t>> : std::true_type
{};

template<>
struct is_bitmask_enum<bitmask_enum_t<std::uint32_t>> : std::true_type
{};

template<>
struct is_bitmask_enum<bitmask_enum_t<std::uint64_t>> : std::true_type
{};

namespace {

TEST_CASE_TEMPLATE("bitmask_enum: operator|", IntegralType, std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t)
{
    using T = bitmask_enum_t<IntegralType>;

    CHECK_EQ((T::flag_0 | T::flag_1), static_cast<T>(0b011));
    CHECK_EQ((T::flag_1 | T::flag_2), static_cast<T>(0b110));
    CHECK_EQ((T::flag_0 | T::flag_2), static_cast<T>(0b101));
}

TEST_CASE_TEMPLATE("bitmask_enum: operator&", IntegralType, std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t)
{
    using T = bitmask_enum_t<IntegralType>;

    CHECK_EQ((T::flag_0 & T::flag_1), T::none);
    CHECK_EQ((T::flag_1 & T::flag_1), T::flag_1);
    CHECK_EQ((T::flag_2 & T::flag_2), T::flag_2);
}

TEST_CASE_TEMPLATE("bitmask_enum: operator^", IntegralType, std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t)
{
    using T = bitmask_enum_t<IntegralType>;

    CHECK_EQ((T::flag_0 ^ T::flag_1), static_cast<T>(0b011));
    CHECK_EQ((T::flag_1 ^ T::flag_1), T::none);
    CHECK_EQ((T::flag_2 ^ T::flag_0), static_cast<T>(0b101));
}

TEST_CASE_TEMPLATE("bitmask_enum: operator~", IntegralType, std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t)
{
    using T = bitmask_enum_t<IntegralType>;

    CHECK_EQ(~T::flag_0, static_cast<T>(~0b001));
    CHECK_EQ(~T::flag_1, static_cast<T>(~0b010));
    CHECK_EQ(~T::flag_2, static_cast<T>(~0b100));
}

TEST_CASE_TEMPLATE("bitmask_enum: operator|=", IntegralType, std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t)
{
    using T = bitmask_enum_t<IntegralType>;

    T v = T::flag_0;
    v |= T::flag_1;
    CHECK_EQ(v, static_cast<T>(0b011));

    v |= T::flag_2;
    CHECK_EQ(v, static_cast<T>(0b111));
}

TEST_CASE_TEMPLATE("bitmask_enum: operator&=", IntegralType, std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t)
{
    using T = bitmask_enum_t<IntegralType>;

    T v = static_cast<T>(0b111);
    v &= T::flag_1;
    CHECK_EQ(v, T::flag_1);

    v &= T::flag_2;
    CHECK_EQ(v, T::none);
}

TEST_CASE_TEMPLATE("bitmask_enum: operator^=", IntegralType, std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t)
{
    using T = bitmask_enum_t<IntegralType>;

    T v = T::flag_0;
    v ^= T::flag_1;
    CHECK_EQ(v, static_cast<T>(0b011));

    v ^= T::flag_0;
    CHECK_EQ(v, T::flag_1);
}

} // namespace
} // namespace poc
