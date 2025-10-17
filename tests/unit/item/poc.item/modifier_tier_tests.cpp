#include <doctest/doctest.h>

#include <poc.item/modifier_tier.hpp>

namespace poc { namespace {

TEST_CASE("modifier_tier: default ctor")
{
    modifier_tier tier{};

    CHECK(tier.is_tier());
    CHECK_FALSE(tier.is_rank());
    CHECK_EQ(tier.value(), 0);
}

TEST_CASE("modifier_tier: value ctor")
{
    modifier_tier tier{5};

    CHECK(tier.is_tier());
    CHECK_FALSE(tier.is_rank());
    CHECK_EQ(tier.value(), 5);
}

TEST_CASE("modifier_tier: rank ctor")
{
    modifier_tier tier{3, modifier_tier::rank_tag{}};

    CHECK_FALSE(tier.is_tier());
    CHECK(tier.is_rank());
    CHECK_EQ(tier.value(), 3);
}

TEST_CASE("modifier_tier: comparison")
{
    modifier_tier tier1{4};
    modifier_tier tier2{7};
    modifier_tier rank1{2, modifier_tier::rank_tag{}};
    modifier_tier rank2{5, modifier_tier::rank_tag{}};

    CHECK_LT(tier1, tier2);
    CHECK_GT(tier2, tier1);
    CHECK_NE(tier1, tier2);
    CHECK_LT(rank1, rank2);
    CHECK_GT(rank2, rank1);
    CHECK_NE(rank1, rank2);
    CHECK_LT(tier1, rank1); // tiers are considered less than ranks
    CHECK_GT(rank2, tier2);
}

TEST_CASE("modifier_tier: set value")
{
    modifier_tier tier{};

    tier.set(6);
    CHECK(tier.is_tier());
    CHECK_EQ(tier.value(), 6);

    tier.set_rank(4);
    CHECK(tier.is_rank());
    CHECK_EQ(tier.value(), 4);
}

TEST_CASE("modifier_tier: operator value_type()")
{
    modifier_tier tier{5};

    CHECK_EQ(static_cast<modifier_tier::value_type>(tier), 5);
}

}} // namespace poc
