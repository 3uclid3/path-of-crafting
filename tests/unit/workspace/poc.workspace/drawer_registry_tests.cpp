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
    REQUIRE(add_drawer("background", drawing_phase::background));
    REQUIRE(add_drawer("main", drawing_phase::main));
    REQUIRE(add_drawer("overlay", drawing_phase::overlay));

    registry.draw_all(ctx);

    CHECK_EQ(calls, std::vector<drawer_id>{"background", "main", "overlay"});
}

TEST_CASE_FIXTURE(fixture, "drawer_registry: add enforces unique ids across phases")
{
    REQUIRE(add_drawer("unique", drawing_phase::background));

    CHECK_FALSE(add_drawer("unique", drawing_phase::background));
    CHECK_FALSE(add_drawer("unique", drawing_phase::overlay));

    registry.draw_all(ctx);

    CHECK_EQ(calls, std::vector<drawer_id>{"unique"});
}

TEST_CASE_FIXTURE(fixture, "drawer_registry: add sorts drawers by priority")
{
    REQUIRE(add_drawer("high", drawing_phase::main, draw_priority{10}));
    REQUIRE(add_drawer("low", drawing_phase::main, draw_priority{-5}));
    REQUIRE(add_drawer("mid", drawing_phase::main, draw_priority{0}));

    registry.draw_all(ctx);

    CHECK_EQ(calls, std::vector<drawer_id>{"low", "mid", "high"});
}

TEST_CASE_FIXTURE(fixture, "drawer_registry: add updates indices after priority insertion")
{
    REQUIRE(add_drawer("low", drawing_phase::main, draw_priority{-5}));
    REQUIRE(add_drawer("high", drawing_phase::main, draw_priority{5}));
    REQUIRE(add_drawer("mid", drawing_phase::main, draw_priority{0}));

    CHECK(registry.remove("high"));

    registry.draw_all(ctx);

    CHECK_EQ(calls, std::vector<drawer_id>{"low", "mid"});
}

TEST_CASE_FIXTURE(fixture, "drawer_registry: remove updates indices and allows reinsertion")
{
    REQUIRE(add_drawer("low", drawing_phase::main, draw_priority{-5}));
    REQUIRE(add_drawer("mid", drawing_phase::main, draw_priority{0}));
    REQUIRE(add_drawer("high", drawing_phase::main, draw_priority{5}));

    CHECK(registry.remove("mid"));
    CHECK_FALSE(registry.remove("mid"));

    registry.draw_all(ctx);
    CHECK_EQ(calls, std::vector<drawer_id>{"low", "high"});

    calls.clear();
    REQUIRE(add_drawer("mid", drawing_phase::main, draw_priority{10}));

    registry.draw_all(ctx);
    CHECK_EQ(calls, std::vector<drawer_id>{"low", "high", "mid"});
}

}} // namespace poc::workspace
