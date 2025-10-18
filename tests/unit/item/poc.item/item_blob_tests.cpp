#include <doctest/doctest.h>

#include <poc.item/item_blob.hpp>

namespace poc {
namespace {

TEST_CASE("item_blob: default ctor")
{
    item_blob blob{};

    CHECK(blob.empty());
    CHECK_EQ(blob.size(), 0);
    CHECK_EQ(blob.view(), "");
}

TEST_CASE("item_blob: string ctor")
{
    string raw = "some item data";
    item_blob blob{raw};

    CHECK_EQ(blob.view(), raw);
}

TEST_CASE("item_blob: string_view ctor")
{
    string_view raw = "some item data";
    item_blob blob{raw};

    CHECK_EQ(blob.view(), raw);
}

TEST_CASE("item_blob: string_lease ctor")
{
    string_arena arena{};
    item_blob blob{arena.allocate("some item data")};

    CHECK_EQ(blob.view(), string_view{"some item data"});
}

TEST_CASE("item_blob: move ctor")
{
    string raw = "some item data";
    item_blob blob1{raw};
    item_blob blob2{std::move(blob1)};

    CHECK_EQ(blob2.view(), raw);
    CHECK(blob1.view().empty());
}

TEST_CASE("item_blob: move operator")
{
    string raw = "some item data";
    item_blob blob1{raw};
    item_blob blob2{};

    blob2 = std::move(blob1);

    CHECK_EQ(blob2.view(), raw);
    CHECK(blob1.view().empty());
}

TEST_CASE("item_blob: indexing")
{
    string raw = "0123456789";
    item_blob blob{raw};

    for (std::size_t i = 0; i < raw.size(); ++i)
    {
        CHECK_EQ(blob[i], raw[i]);
    }
}

TEST_CASE("item_blob: iteration")
{
    string raw = "0123456789";
    item_blob blob{raw};

    std::size_t i = 0;
    for (auto c : blob)
    {
        CHECK_EQ(c, raw[i++]);
    }
}

}} // namespace poc