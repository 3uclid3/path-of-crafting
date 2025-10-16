#include <doctest/doctest.h>

#include <poc/string/string_arena.hpp>

#include <poc/string/test_string.hpp>

namespace poc { namespace {

static constexpr std::size_t page_size = 1024 * 64;

TEST_CASE_TEMPLATE("basic_string_arena::lease: default constructor", T, char, char8_t, wchar_t)
{
    using string_arena = basic_string_arena<T, page_size>;
    using string_lease = typename string_arena::lease;
    using string_view = typename string_arena::view_type;

    const string_lease lease;

    CHECK_FALSE(lease.is_valid());
    CHECK_EQ(lease.view(), string_view{});
}

TEST_CASE_TEMPLATE("basic_string_arena: default constructor", T, char, char8_t, wchar_t)
{
    using string_arena = basic_string_arena<T, page_size>;

    const string_arena arena;

    const auto statistics = arena.statistics();

    CHECK_EQ(statistics.page_count, 1);
    CHECK_EQ(statistics.page_size, page_size);

    CHECK_EQ(statistics.allocated_bytes, 0);
    CHECK_EQ(statistics.allocation_count, 0);
}

TEST_CASE_TEMPLATE("basic_string_arena::allocate", T, char, char8_t, wchar_t)
{
    using string_arena = basic_string_arena<T, page_size>;
    using string_lease = typename string_arena::lease;
    using string_view = typename string_arena::view_type;

    string_arena arena;
    string_view str = TEST_STRING("Hello, World!");
    string_lease lease = arena.allocate(str);

    CHECK(lease.is_valid());
    CHECK_EQ(lease.view(), str);

    auto statistics = arena.statistics();

    CHECK_EQ(statistics.page_count, 1);
    CHECK_EQ(statistics.page_size, page_size);

    CHECK_EQ(statistics.allocated_bytes, str.size() * sizeof(T));
    CHECK_EQ(statistics.allocation_count, 1);
}

TEST_CASE_TEMPLATE("basic_string_arena::release", T, char, char8_t, wchar_t)
{
    using string_arena = basic_string_arena<T, page_size>;
    using string_lease = typename string_arena::lease;
    using string_view = typename string_arena::view_type;

    string_arena arena;
    string_view str = TEST_STRING("Hello, World!");

    string_lease lease = arena.allocate(str);

    {
        [[maybe_unused]] auto l = arena.allocate(str);

        auto statistics = arena.statistics();

        CHECK_EQ(statistics.page_count, 1);
        CHECK_EQ(statistics.page_size, page_size);

        CHECK_EQ(statistics.allocation_count, 2);
        CHECK_EQ(statistics.allocated_bytes, str.size() * sizeof(T) * 2);
    }

    {
        auto statistics = arena.statistics();

        CHECK_EQ(statistics.page_count, 1);
        CHECK_EQ(statistics.page_size, page_size);

        CHECK_EQ(statistics.allocation_count, 1);
        CHECK_EQ(statistics.allocated_bytes, str.size() * sizeof(T));
    }

    lease.release();

    {
        auto statistics = arena.statistics();

        CHECK_EQ(statistics.page_count, 1);
        CHECK_EQ(statistics.page_size, page_size);

        CHECK_EQ(statistics.allocation_count, 0);
        CHECK_EQ(statistics.allocated_bytes, 0);
    }
}

TEST_CASE_TEMPLATE("basic_string_arena::allocate multiple pages", T, char, char8_t, wchar_t)
{
    static constexpr std::size_t page_size = 10;

    using string_arena = basic_string_arena<T, page_size>;
    using string_lease = typename string_arena::lease;
    using string_view = typename string_arena::view_type;

    string_view str = TEST_STRING("0123456789");
    REQUIRE_EQ(str.size(), page_size);

    string_arena arena;
    string_lease lease1 = arena.allocate(str);
    string_lease lease2 = arena.allocate(str);

    CHECK(lease1.is_valid());
    CHECK(lease2.is_valid());

    CHECK_EQ(lease1.view(), str);
    CHECK_EQ(lease2.view(), str);

    const auto statistics = arena.statistics();

    CHECK_EQ(statistics.page_count, 2);
    CHECK_EQ(statistics.page_size, page_size);

    CHECK_EQ(statistics.allocation_count, 2);
    CHECK_EQ(statistics.allocated_bytes, str.size() * sizeof(T) * 2);
}

}} // namespace poc
