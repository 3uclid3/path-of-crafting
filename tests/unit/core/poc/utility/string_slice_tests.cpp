#include <doctest/doctest.h>

#include <poc/utility/on_scope_exit.hpp>

namespace poc { namespace {

TEST_CASE("on_scope_exit: executes callback on scope exit")
{
    bool executed = false;

    {
        on_scope_exit exit_guard([&executed]() {
            executed = true;
        });

        CHECK_FALSE(executed);
    }

    CHECK(executed);
}

}} // namespace poc
