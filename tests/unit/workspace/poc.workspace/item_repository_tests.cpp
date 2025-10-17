#include <array>
#include <format>
#include <vector>

#include <doctest/doctest.h>

#include <poc.item/item.builder.hpp>
#include <poc.item/item.hpp>
#include <poc.workspace/item_id_traits.hpp>
#include <poc.workspace/item_repository.hpp>

namespace poc::workspace { namespace {

struct fixture
{
    static constexpr std::size_t initial_capacity{4};

    auto make_item(std::string_view name) -> item
    {
        item_builder.begin(item_blob{name});
        item_builder.set_name(item_builder.blob().view());

        return item_builder.build();
    }

    item_repository repository{initial_capacity};
    item::builder item_builder;
};

TEST_CASE_FIXTURE(fixture, "item_repository: default constructed is empty")
{
    CHECK(repository.empty());
    CHECK_EQ(repository.size(), 0);
}

TEST_CASE_FIXTURE(fixture, "item_repository: added item can be retrieved")
{
    auto id = repository.add(make_item("Test Item"));

    CHECK(!repository.empty());
    CHECK_EQ(repository.size(), 1);

    const auto* retrieved = repository.get(id);
    REQUIRE(retrieved != nullptr);
    CHECK_EQ(retrieved->name(), "Test Item");
}

TEST_CASE_FIXTURE(fixture, "item_repository: removed item can no longer be retrieved")
{
    auto id = repository.add(make_item("To Remove"));

    REQUIRE(repository.remove(id));
    CHECK(repository.get(id) == nullptr);
}

TEST_CASE_FIXTURE(fixture, "item_repository: reusing free slot keeps mapping intact")
{
    const auto first = repository.add(make_item("First"));
    const auto second = repository.add(make_item("Second"));

    REQUIRE(repository.remove(first));

    const auto third = repository.add(make_item("Third"));

    const auto* kept = repository.get(second);
    REQUIRE(kept != nullptr);
    CHECK_EQ(kept->name(), "Second");

    const auto* readded = repository.get(third);
    REQUIRE(readded != nullptr);
    CHECK_EQ(readded->name(), "Third");
}

TEST_CASE_FIXTURE(fixture, "item_repository: removing unknown id fails")
{
    const auto invalid_id = item_id_traits::construct(42, 0);
    CHECK_FALSE(repository.remove(invalid_id));
}

TEST_CASE_FIXTURE(fixture, "item_repository: stale id is rejected")
{
    const auto first = repository.add(make_item("Original"));
    REQUIRE(repository.remove(first));

    const auto recycled = repository.add(make_item("Replacement"));

    CHECK_FALSE(repository.remove(first));
    CHECK(repository.get(first) == nullptr);
    CHECK(repository.get_timestamp(first) == item_repository::time_point{});

    const auto* current = repository.get(recycled);
    REQUIRE(current != nullptr);
    CHECK_EQ(current->name(), "Replacement");
}

TEST_CASE_FIXTURE(fixture, "item_repository: timestamp set for valid id")
{
    const auto id = repository.add(make_item("Timestamped"));

    const auto timestamp = repository.get_timestamp(id);
    CHECK(timestamp != item_repository::time_point{});
}

TEST_CASE_FIXTURE(fixture, "item_repository: timestamp defaults for invalid id")
{
    const auto invalid_id = item_id_traits::construct(99, 0);
    CHECK(repository.get_timestamp(invalid_id) == item_repository::time_point{});
}

TEST_CASE_FIXTURE(fixture, "item_repository: adding past initial capacity grows storage")
{
    std::array<item_id, initial_capacity + 2> ids{};

    for (std::size_t i = 0; i < ids.size(); ++i)
    {
        ids[i] = repository.add(make_item(std::format("Item {}", i)));
    }

    CHECK_EQ(repository.size(), ids.size());

    for (std::size_t i = 0; i < ids.size(); ++i)
    {
        const auto* stored = repository.get(ids[i]);
        REQUIRE(stored != nullptr);
        CHECK_EQ(stored->name(), std::format("Item {}", i));
    }
}

TEST_CASE_FIXTURE(fixture, "item_repository: added signal emits generated ids")
{
    std::vector<item_id> observed;
    auto connection = repository.added().connect([&](item_id id) { observed.push_back(id); });

    const auto first = repository.add(make_item("First signal"));
    const auto second = repository.add(make_item("Second signal"));

    REQUIRE_EQ(observed.size(), 2);
    CHECK_EQ(observed[0], first);
    CHECK_EQ(observed[1], second);
}

TEST_CASE_FIXTURE(fixture, "item_repository: removed signal emits removed ids")
{
    std::vector<item_id> observed;
    auto connection = repository.removed().connect([&](item_id id) { observed.push_back(id); });

    const auto first = repository.add(make_item("First removal"));
    const auto second = repository.add(make_item("Second removal"));

    REQUIRE(repository.remove(first));
    REQUIRE(repository.remove(second));
    CHECK_FALSE(repository.remove(first));

    REQUIRE_EQ(observed.size(), 2);
    CHECK_EQ(observed[0], first);
    CHECK_EQ(observed[1], second);
}

}} // namespace poc::workspace
