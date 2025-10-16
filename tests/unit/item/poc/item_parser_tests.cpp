#include <doctest/doctest.h>

#include <poc/item_blobs.hpp>
#include <poc/item_parser.hpp>

namespace poc { namespace {

TEST_CASE("item_parser: empty returns error")
{
    item_parser parser;

    const auto result = parser.parse(item_blobs::empty);
    REQUIRE_FALSE(result.has_value());

    CHECK_EQ(result.error(), item_parser::error::empty);
}

TEST_CASE("item_parser: unknown")
{
    item_parser parser;

    const auto result = parser.parse(item_blobs::unknown);
    REQUIRE(result.has_value());

    const auto& item = result.value();
    CHECK_EQ(item.kind(), item_kind::unknown);
    CHECK_EQ(item.rarity(), item_rarity::unknown);
}

TEST_CASE("item_parser: rarity normal")
{
    item_parser parser;

    const auto result = parser.parse(item_blobs::rarity_normal);
    REQUIRE(result.has_value());

    const auto& item = result.value();
    CHECK_EQ(item.rarity(), item_rarity::normal);
    CHECK_EQ(item.name(), "Convening Wand");
    CHECK_EQ(item.base_name(), "Convening Wand");

    CHECK(item.implicits().empty());
    CHECK(item.prefixes().empty());
    CHECK(item.suffixes().empty());
}

TEST_CASE("item_parser: rarity magic" * doctest::may_fail())
{
    item_parser parser;

    const auto result = parser.parse(item_blobs::rarity_magic);
    REQUIRE(result.has_value());

    const auto& item = result.value();
    CHECK_EQ(item.rarity(), item_rarity::magic);
    CHECK_EQ(item.name(), "Convening Wand of Triumph");
    CHECK_EQ(item.base_name(), "Convening Wand");

    CHECK(item.implicits().empty());
    CHECK(item.prefixes().empty());

    const auto& suffixes = item.suffixes();
    REQUIRE_EQ(suffixes.size(), 1);
    CHECK_EQ(suffixes[0].name(), "of Triumph");
    CHECK_EQ(suffixes[0].tier(), 4);
    CHECK_EQ(suffixes[0].lines(), "Gain 31(24-32) Life per Enemy Killed");
    CHECK_EQ(suffixes[0].tags(), "");
}

TEST_CASE("item_parser: rarity rare")
{
    item_parser parser;

    const auto result = parser.parse(item_blobs::rarity_rare);
    REQUIRE(result.has_value());

    const auto& item = result.value();
    CHECK_EQ(item.rarity(), item_rarity::rare);
    CHECK_EQ(item.name(), "Blight Goad");
    CHECK_EQ(item.base_name(), "Convening Wand");

    CHECK(item.implicits().empty());

    const auto& prefixes = item.prefixes();
    REQUIRE_EQ(prefixes.size(), 2);
    CHECK_EQ(prefixes[0].name(), "Duke's");
    CHECK_EQ(prefixes[0].tier(), 3);
    CHECK_EQ(prefixes[0].lines(), "+35(34-37) to maximum Mana\nMinions deal 26(25-29)% increased Damage");
    CHECK_EQ(prefixes[0].tags(), "");
    CHECK_EQ(prefixes[1].name(), "Polished");
    CHECK_EQ(prefixes[1].tier(), 7);
    CHECK_EQ(prefixes[1].lines(), "Adds 8(6-9) to 13(13-15) Physical Damage");
    CHECK_EQ(prefixes[1].tags(), "");

    const auto& suffixes = item.suffixes();
    REQUIRE_EQ(suffixes.size(), 3);
    CHECK_EQ(suffixes[0].name(), "of Triumph");
    CHECK_EQ(suffixes[0].tier(), 4);
    CHECK_EQ(suffixes[0].lines(), "Gain 31(24-32) Life per Enemy Killed");
    CHECK_EQ(suffixes[0].tags(), "");
    CHECK_EQ(suffixes[1].name(), "of the Throng");
    CHECK_EQ(suffixes[1].tier(), 4);
    CHECK_EQ(suffixes[1].lines(), "Minions have +24(20-24)% to Critical Strike Multiplier");
    CHECK_EQ(suffixes[1].tags(), "");
    CHECK_EQ(suffixes[2].name(), "of Rime");
    CHECK_EQ(suffixes[2].tier(), 3);
    CHECK_EQ(suffixes[2].lines(), "19(19-22)% increased Cold Damage");
    CHECK_EQ(suffixes[2].tags(), "");
}

}} // namespace poc
