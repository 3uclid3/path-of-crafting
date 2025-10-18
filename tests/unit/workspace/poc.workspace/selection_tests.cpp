#include <doctest/doctest.h>

#include <poc.workspace/selection.hpp>

#include <vector>

namespace poc::workspace { namespace {

TEST_CASE("selection: construct")
{
    selection s;
    CHECK(s.empty());
    CHECK_EQ(s.size(), 0);
}

TEST_CASE("selection: select")
{
    selection s;
    s.select(item_id{1});

    REQUIRE_EQ(s.size(), 1);

    CHECK(s.contains(item_id{1}));
    CHECK_FALSE(s.contains(item_id{2}));
}

TEST_CASE("selection: select multiple")
{
    selection s;
    s.select(std::vector{item_id{1}, item_id{2}});

    REQUIRE_EQ(s.size(), 2);

    CHECK(s.contains(item_id{1}));
    CHECK(s.contains(item_id{2}));
    CHECK_FALSE(s.contains(item_id{3}));
}

TEST_CASE("selection: deselect")
{
    selection s;
    s.select(item_id{1});
    s.deselect(item_id{1});

    CHECK(s.empty());
    CHECK_FALSE(s.contains(item_id{1}));
}

TEST_CASE("selection: deselect multiple")
{
    selection s;
    s.select(std::vector{item_id{1}, item_id{2}});
    s.deselect(std::vector{item_id{1}, item_id{2}});

    CHECK(s.empty());
    CHECK_FALSE(s.contains(item_id{1}));
    CHECK_FALSE(s.contains(item_id{2}));
}

TEST_CASE("selection: toggle")
{
    selection s;
    s.toggle(item_id{1});

    REQUIRE_EQ(s.size(), 1);
    CHECK(s.contains(item_id{1}));

    s.toggle(item_id{1});

    CHECK(s.empty());
    CHECK_FALSE(s.contains(item_id{1}));
}

TEST_CASE("selection: clear")
{
    selection s;
    s.select(item_id{1});
    s.select(item_id{2});

    REQUIRE_EQ(s.size(), 2);

    s.clear();

    CHECK(s.empty());
    CHECK_FALSE(s.contains(item_id{1}));
    CHECK_FALSE(s.contains(item_id{2}));
}

TEST_CASE("selection: reserve")
{
    selection s;
    s.reserve(10);

    CHECK(s.empty());
    CHECK_EQ(s.size(), 0);
}

TEST_CASE("selection: iterable")
{
    selection s;
    s.select(std::vector{item_id{1}, item_id{2}, item_id{3}});

    std::vector<item_id> items;
    for (const auto& item : s)
    {
        items.push_back(item);
    }

    REQUIRE_EQ(items.size(), 3);
    CHECK_EQ(items[0], item_id{1});
    CHECK_EQ(items[1], item_id{2});
    CHECK_EQ(items[2], item_id{3});
}

TEST_CASE("selection: changed signal")
{
    selection s;

    std::vector<std::tuple<selected, std::vector<item_id>>> items;
    auto connection = s.changed().connect([&](selected sel, std::span<const selection::value_type> itms) {
        items.emplace_back(sel, std::vector<item_id>(itms.begin(), itms.end()));
    });

    s.select(item_id{1});
    s.select(std::vector{item_id{2}, item_id{3}});
    s.deselect(item_id{1});
    s.deselect(std::vector{item_id{2}, item_id{3}});

    REQUIRE_EQ(items.size(), 4);
    CHECK_EQ(items[0], std::make_tuple(selected(true), std::vector{item_id{1}}));
    CHECK_EQ(items[1], std::make_tuple(selected(true), std::vector{item_id{2}, item_id{3}}));
    CHECK_EQ(items[2], std::make_tuple(selected(false), std::vector{item_id{1}}));
    CHECK_EQ(items[3], std::make_tuple(selected(false), std::vector{item_id{2}, item_id{3}}));
}

TEST_CASE("selection: changed signal allows nested select")
{
    selection s;
    std::vector<std::tuple<selected, std::vector<item_id>>> events;

    bool nested_called = false;
    auto connection = s.changed().connect([&](selected sel, std::span<const selection::value_type> value) {
        events.emplace_back(sel, std::vector<item_id>{value.begin(), value.end()});

        if (!nested_called)
        {
            nested_called = true;
            s.select(item_id{2});
        }
    });

    s.select(item_id{1});

    REQUIRE_EQ(events.size(), 2);
    CHECK_EQ(events[0], std::make_tuple(selected(true), std::vector{item_id{1}}));
    CHECK_EQ(events[1], std::make_tuple(selected(true), std::vector{item_id{2}}));

    CHECK(s.contains(item_id{1}));
    CHECK(s.contains(item_id{2}));
    CHECK_EQ(s.size(), 2);
}

TEST_CASE("selection: changed signal allows nested deselect after select")
{
    selection s;
    std::vector<std::tuple<selected, std::vector<item_id>>> events;

    bool nested_called = false;
    auto connection = s.changed().connect([&](selected sel, std::span<const selection::value_type> value) {
        events.emplace_back(sel, std::vector<item_id>{value.begin(), value.end()});

        if (!nested_called)
        {
            nested_called = true;
            s.deselect(item_id{1});
        }
    });

    s.select(item_id{1});

    REQUIRE_EQ(events.size(), 2);
    CHECK_EQ(events[0], std::make_tuple(selected(true), std::vector{item_id{1}}));
    CHECK_EQ(events[1], std::make_tuple(selected(false), std::vector{item_id{1}}));

    CHECK(s.empty());
}

TEST_CASE("selection: changed signal allows nested select after deselect")
{
    selection s;
    s.select(std::vector{item_id{1}, item_id{2}});

    std::vector<std::tuple<selected, std::vector<item_id>>> events;

    bool nested_called = false;
    auto connection = s.changed().connect([&](selected sel, std::span<const selection::value_type> value) {
        events.emplace_back(sel, std::vector<item_id>{value.begin(), value.end()});

        if (!nested_called)
        {
            nested_called = true;
            s.select(item_id{3});
        }
    });

    s.deselect(item_id{1});

    REQUIRE_EQ(events.size(), 2);
    CHECK_EQ(events[0], std::make_tuple(selected(false), std::vector{item_id{1}}));
    CHECK_EQ(events[1], std::make_tuple(selected(true), std::vector{item_id{3}}));

    CHECK(s.contains(item_id{2}));
    CHECK(s.contains(item_id{3}));
    CHECK_EQ(s.size(), 2);
}

TEST_CASE("selection: changed signal allows nested deselect")
{
    selection s;
    s.select(std::vector{item_id{1}, item_id{2}});

    std::vector<std::tuple<selected, std::vector<item_id>>> events;

    bool nested_called = false;
    auto connection = s.changed().connect([&](selected sel, std::span<const selection::value_type> value) {
        events.emplace_back(sel, std::vector<item_id>{value.begin(), value.end()});

        if (!nested_called)
        {
            nested_called = true;
            s.deselect(item_id{2});
        }
    });

    s.deselect(item_id{1});

    REQUIRE_EQ(events.size(), 2);
    CHECK_EQ(events[0], std::make_tuple(selected(false), std::vector{item_id{1}}));
    CHECK_EQ(events[1], std::make_tuple(selected(false), std::vector{item_id{2}}));

    CHECK(s.empty());
}

}} // namespace poc::workspace
