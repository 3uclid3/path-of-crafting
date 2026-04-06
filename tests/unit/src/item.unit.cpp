#include <poc/item.hpp>

#include <doctest/doctest.h>

namespace poc { namespace {

TEST_CASE("item::is_magic_blob: returns true for item blobs")
{
    CHECK(item::is_magic_blob("Item Class: Belts\nRarity: Magic"));
}

TEST_CASE("item::is_magic_blob: returns false for non-magic item blobs")
{
    CHECK_FALSE(item::is_magic_blob("Item Class: Belts\nRarity: Rare"));
}

TEST_CASE("item::is_magic_blob: returns false for non-item blobs")
{
    CHECK_FALSE(item::is_magic_blob("This is not an item blob"));
}

TEST_CASE("item::parse_magic_name: extracts name from magic item blob")
{
    std::string_view blob = "Item Class: Belts\nRarity: Magic\nSome Other Info\n";
    CHECK(item::parse_magic_name(blob) == "Some Other Info");
}

}} // namespace poc