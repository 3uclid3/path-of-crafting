

#include <doctest/doctest.h>

#include <poc.workspace/ingester.hpp>

#include <chrono>
#include <string_view>
#include <thread>
#include <vector>

#include <poc.item/item.hpp>
#include <poc.item_blobs/item_blobs.hpp>

namespace poc::workspace { namespace {

struct fixture
{
    static inline constexpr std::chrono::milliseconds timeout{200};

    auto wait_for_items(std::vector<item>& out, std::size_t expected) -> std::size_t
    {
        const auto deadline = std::chrono::steady_clock::now() + timeout;

        std::size_t total = 0;

        while (std::chrono::steady_clock::now() < deadline)
        {
            total += ingest.drain(out);

            if (out.size() >= expected)
            {
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        return total;
    }

    ingester ingest;
};

TEST_CASE_FIXTURE(fixture, "ingester: can_submit requires header")
{
    CHECK(ingest.can_submit(item_blobs::rarity_rare));
    CHECK_FALSE(ingest.can_submit("Invalid blob"));
}

TEST_CASE_FIXTURE(fixture, "ingester: invalid blob is ignored")
{
    ingest.submit(item_blob{});

    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    std::vector<item> out;
    CHECK_EQ(ingest.drain(out), 0);
    CHECK(out.empty());
}

TEST_CASE_FIXTURE(fixture, "ingester: valid blob drains into output")
{
    ingest.submit(item_blobs::rarity_rare);

    std::vector<item> out;
    const auto drained = wait_for_items(out, 1);

    REQUIRE_EQ(drained, 1);
    REQUIRE_EQ(out.size(), 1);
    CHECK_EQ(out.front().name(), "Blight Goad");

    CHECK_EQ(ingest.drain(out), 0);
    CHECK_EQ(out.size(), 1);
}

TEST_CASE_FIXTURE(fixture, "ingester: preserves submission order")
{
    ingest.submit(item_blobs::rarity_normal);
    ingest.submit(item_blobs::rarity_rare);

    std::vector<item> out;
    const auto drained = wait_for_items(out, 2);

    REQUIRE(drained >= 2);
    REQUIRE_EQ(out.size(), 2);
    CHECK_EQ(out[0].name(), "Convening Wand");
    CHECK_EQ(out[1].name(), "Blight Goad");
}

}} // namespace poc::workspace
