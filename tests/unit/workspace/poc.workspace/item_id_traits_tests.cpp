#include <doctest/doctest.h>

#include <poc.workspace/item_id_traits.hpp>

namespace poc::workspace { namespace {

TEST_CASE("item_id_traits: construct")
{
    auto check = [](item_id_traits::index_type index, item_id_traits::version_type version) {
        CAPTURE(index);
        CAPTURE(version);

        const auto id = item_id_traits::construct(index, version);

        CHECK_EQ(item_id_traits::to_index(id), index);
        CHECK_EQ(item_id_traits::to_version(id), version);
    };

    check(0, 0);
    check(4, 2);
    check(item_id_traits::index_mask - 1, item_id_traits::version_mask - 1);
    check(item_id_traits::index_mask, item_id_traits::version_mask);
}

TEST_CASE("item_id_traits: next")
{
    auto check = [](item_id_traits::index_type index, item_id_traits::version_type version) {
        const auto start_id = item_id_traits::construct(index, version);
        const auto next_id = item_id_traits::next(start_id);

        CHECK_EQ(item_id_traits::to_index(next_id), index);
        CHECK_EQ(item_id_traits::to_version(next_id), version + 1);
    };

    check(0, 0);
    check(4, 2);
}

TEST_CASE("item_id_traits: next limits")
{
    const auto start_id = item_id_traits::construct(8, item_id_traits::version_mask - 1);
    const auto next_id = item_id_traits::next(start_id);

    CHECK_EQ(item_id_traits::to_index(next_id), 8);
    CHECK_EQ(item_id_traits::to_version(next_id), 0);
}

TEST_CASE("item_id_traits: merge")
{
    const auto id0 = item_id_traits::construct(1, 2);
    const auto id1 = item_id_traits::construct(3, 4);

    const auto merged = item_id_traits::construct(1, 4);
    const auto rmerged = item_id_traits::construct(3, 2);

    CHECK_EQ(item_id_traits::merge(id0, id1), merged);
    CHECK_EQ(item_id_traits::merge(item_id_traits::to_integral(id0), item_id_traits::to_integral(id1)), merged);
    CHECK_EQ(item_id_traits::merge(item_id_traits::to_integral(id0), id1), merged);
    CHECK_EQ(item_id_traits::merge(id0, item_id_traits::to_integral(id1)), merged);

    CHECK_EQ(item_id_traits::merge(id1, id0), rmerged);
}

}} // namespace poc::workspace
