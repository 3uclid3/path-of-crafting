#include <doctest/doctest.h>

#include <poc.workspace/action_dispatcher.hpp>

#include <variant>
#include <vector>

#include <poc.workspace/context.hpp>

namespace poc::workspace { namespace {

struct action_default
{
    static constexpr action_id id{make_action_id("action_default")};
    // Uses default coalesce policy: keep_all

    int value{};
};

struct action_reentrant
{
    static constexpr action_id id{make_action_id("action_reentrant")};

    int value{};
};

template<action_coalesce_policy Policy>
struct action_policy_id;

template<action_coalesce_policy Policy>
struct action
{
    static constexpr action_id id{make_action_id(magic_enum::enum_name<Policy>())};
    static constexpr action_coalesce_policy coalesce_policy{Policy};

    int value{};
};

using action_keep_all = action<action_coalesce_policy::keep_all>;
using action_replace_last = action<action_coalesce_policy::replace_last>;
using action_drop_if_exists = action<action_coalesce_policy::drop_if_exists>;

using any_action = std::variant<action_default, action_reentrant, action_keep_all, action_replace_last, action_drop_if_exists>;

struct fixture
{
    fixture()
    {
        REQUIRE(dispatcher.register_handler<action_default>([this](action_context&, const auto& action) {
            actions.push_back(action);
        }));

        REQUIRE(dispatcher.register_handler<action_reentrant>([this](action_context&, const auto& action) {
            dispatcher.enqueue<action_default>(action.value + 10);

            actions.push_back(action);
        }));

        REQUIRE(dispatcher.register_handler<action_keep_all>([this](action_context&, const auto& action) {
            actions.push_back(action);
        }));

        REQUIRE(dispatcher.register_handler<action_replace_last>([this](action_context&, const auto& action) {
            actions.push_back(action);
        }));

        REQUIRE(dispatcher.register_handler<action_drop_if_exists>([this](action_context&, const auto& action) {
            actions.push_back(action);
        }));
    }

    auto as_default(std::size_t index) -> const action_default&
    {
        REQUIRE(std::holds_alternative<action_default>(actions[index]));
        return std::get<action_default>(actions[index]);
    }

    auto as_reentrant(std::size_t index) -> const action_reentrant&
    {
        REQUIRE(std::holds_alternative<action_reentrant>(actions[index]));
        return std::get<action_reentrant>(actions[index]);
    }

    auto as_keep_all(std::size_t index) -> const action_keep_all&
    {
        REQUIRE(std::holds_alternative<action_keep_all>(actions[index]));
        return std::get<action_keep_all>(actions[index]);
    }

    auto as_replace_last(std::size_t index) -> const action_replace_last&
    {
        REQUIRE(std::holds_alternative<action_replace_last>(actions[index]));
        return std::get<action_replace_last>(actions[index]);
    }

    auto as_drop_if_exists(std::size_t index) -> const action_drop_if_exists&
    {
        REQUIRE(std::holds_alternative<action_drop_if_exists>(actions[index]));
        return std::get<action_drop_if_exists>(actions[index]);
    }

    std::vector<any_action> actions;

    action_context context;
    action_dispatcher dispatcher;
};

TEST_CASE_FIXTURE(fixture, "action_dispatcher: default traits")
{
    static_assert(action_traits<action_default>::coalesce_policy == action_coalesce_policy::keep_all);

    dispatcher.enqueue<action_default>(1);
    dispatcher.enqueue<action_default>(2);
    dispatcher.enqueue<action_default>(3);

    dispatcher.dispatch(context);

    REQUIRE_EQ(actions.size(), 3);
    CHECK_EQ(as_default(0).value, 1);
    CHECK_EQ(as_default(1).value, 2);
    CHECK_EQ(as_default(2).value, 3);
}

TEST_CASE_FIXTURE(fixture, "action_dispatcher: keep_all")
{
    dispatcher.enqueue<action_keep_all>(1);
    dispatcher.enqueue<action_default>(2);
    dispatcher.enqueue<action_keep_all>(3);
    dispatcher.enqueue<action_keep_all>(4);

    dispatcher.dispatch(context);

    REQUIRE_EQ(actions.size(), 4);
    CHECK_EQ(as_keep_all(0).value, 1);
    CHECK_EQ(as_default(1).value, 2);
    CHECK_EQ(as_keep_all(2).value, 3);
    CHECK_EQ(as_keep_all(3).value, 4);
}

TEST_CASE_FIXTURE(fixture, "action_dispatcher: replace_last")
{
    dispatcher.enqueue<action_replace_last>(1);
    dispatcher.enqueue<action_default>(2);
    dispatcher.enqueue<action_replace_last>(3);
    dispatcher.enqueue<action_replace_last>(4);

    dispatcher.dispatch(context);

    REQUIRE_EQ(actions.size(), 3);
    CHECK_EQ(as_replace_last(0).value, 1);
    CHECK_EQ(as_default(1).value, 2);
    CHECK_EQ(as_replace_last(2).value, 4);
}

TEST_CASE_FIXTURE(fixture, "action_dispatcher: drop_if_exists")
{
    dispatcher.enqueue<action_drop_if_exists>(1);
    dispatcher.enqueue<action_default>(2);
    dispatcher.enqueue<action_drop_if_exists>(3);
    dispatcher.enqueue<action_drop_if_exists>(4);

    dispatcher.dispatch(context);

    REQUIRE_EQ(actions.size(), 2);
    CHECK_EQ(as_drop_if_exists(0).value, 1);
    CHECK_EQ(as_default(1).value, 2);
}

TEST_CASE_FIXTURE(fixture, "action_dispatcher: duplicate handler")
{
    CHECK_FALSE(dispatcher.register_handler<action_default>([](action_context&, const auto&) {}));
}

TEST_CASE_FIXTURE(fixture, "action_dispatcher: multiple dispatch calls")
{
    dispatcher.enqueue<action_default>(2);

    dispatcher.dispatch(context);
    dispatcher.dispatch(context);
    dispatcher.dispatch(context);

    REQUIRE_EQ(actions.size(), 1);
    CHECK_EQ(as_default(0).value, 2);
}

TEST_CASE_FIXTURE(fixture, "action_dispatcher: reentrant enqueue during dispatch")
{
    dispatcher.enqueue<action_reentrant>(5);

    dispatcher.dispatch(context);

    REQUIRE_EQ(actions.size(), 1);
    CHECK_EQ(as_reentrant(0).value, 5);

    dispatcher.dispatch(context);

    REQUIRE_EQ(actions.size(), 2);
    CHECK_EQ(as_default(1).value, 15);
}

}} // namespace poc::workspace
