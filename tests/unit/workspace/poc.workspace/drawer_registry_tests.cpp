#include <doctest/doctest.h>

#include <poc.workspace/drawer_registry.hpp>

#include <vector>

#include <poc.workspace/context.hpp>

namespace poc::workspace { namespace {

struct fixture
{
    auto make_drawer(drawer_id id) -> drawer
    {
        return [this, id](draw_context&) { calls.push_back(id); };
    }

    auto add_drawer(drawer_id id, drawing_phase phase, draw_priority priority = draw_priority{0}) -> bool
    {
        return registry.add(id, make_drawer(id), phase, priority);
    }

    draw_context ctx;
    drawer_registry registry;

    std::vector<drawer_id> calls;
};

TEST_CASE_FIXTURE(fixture, "drawer_registry: draw_all respects phase ordering")
{
    constexpr auto background_id = make_drawer_id("background");
    constexpr auto main_id = make_drawer_id("main");
    constexpr auto overlay_id = make_drawer_id("overlay");

    REQUIRE(add_drawer(background_id, drawing_phase::background));
    REQUIRE(add_drawer(main_id, drawing_phase::main));
    REQUIRE(add_drawer(overlay_id, drawing_phase::overlay));

    registry.draw_all(ctx);

    CHECK_EQ(calls, std::vector<drawer_id>{background_id, main_id, overlay_id});
}

TEST_CASE_FIXTURE(fixture, "drawer_registry: add enforces unique ids across phases")
{
    constexpr auto unique_id = make_drawer_id("unique");

    REQUIRE(add_drawer(unique_id, drawing_phase::background));

    CHECK_FALSE(add_drawer(unique_id, drawing_phase::background));
    CHECK_FALSE(add_drawer(unique_id, drawing_phase::overlay));

    registry.draw_all(ctx);

    CHECK_EQ(calls, std::vector<drawer_id>{unique_id});
}

TEST_CASE_FIXTURE(fixture, "drawer_registry: add sorts drawers by priority")
{
    constexpr auto high_id = make_drawer_id("high");
    constexpr auto mid_id = make_drawer_id("mid");
    constexpr auto low_id = make_drawer_id("low");

    REQUIRE(add_drawer(high_id, drawing_phase::main, draw_priority{10}));
    REQUIRE(add_drawer(low_id, drawing_phase::main, draw_priority{-5}));
    REQUIRE(add_drawer(mid_id, drawing_phase::main, draw_priority{0}));

    registry.draw_all(ctx);

    CHECK_EQ(calls, std::vector<drawer_id>{low_id, mid_id, high_id});
}

TEST_CASE_FIXTURE(fixture, "drawer_registry: add updates indices after priority insertion")
{
    constexpr auto high_id = make_drawer_id("high");
    constexpr auto mid_id = make_drawer_id("mid");
    constexpr auto low_id = make_drawer_id("low");

    REQUIRE(add_drawer(low_id, drawing_phase::main, draw_priority{-5}));
    REQUIRE(add_drawer(high_id, drawing_phase::main, draw_priority{5}));
    REQUIRE(add_drawer(mid_id, drawing_phase::main, draw_priority{0}));

    CHECK(registry.remove(high_id));

    registry.draw_all(ctx);

    CHECK_EQ(calls, std::vector<drawer_id>{low_id, mid_id});
}

TEST_CASE_FIXTURE(fixture, "drawer_registry: remove updates indices and allows reinsertion")
{
    constexpr auto high_id = make_drawer_id("high");
    constexpr auto mid_id = make_drawer_id("mid");
    constexpr auto low_id = make_drawer_id("low");

    REQUIRE(add_drawer(low_id, drawing_phase::main, draw_priority{-5}));
    REQUIRE(add_drawer(mid_id, drawing_phase::main, draw_priority{0}));
    REQUIRE(add_drawer(high_id, drawing_phase::main, draw_priority{5}));

    CHECK(registry.remove(mid_id));
    CHECK_FALSE(registry.remove(mid_id));

    registry.draw_all(ctx);
    CHECK_EQ(calls, std::vector<drawer_id>{low_id, high_id});

    calls.clear();
    REQUIRE(add_drawer(mid_id, drawing_phase::main, draw_priority{10}));

    registry.draw_all(ctx);
    CHECK_EQ(calls, std::vector<drawer_id>{low_id, high_id, mid_id});
}

}} // namespace poc::workspace
