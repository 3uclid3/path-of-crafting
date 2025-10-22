#include <doctest/doctest.h>

#include <poc.workspace/extension.hpp>

namespace poc::workspace { namespace {

struct track_extension : extension
{
    static constexpr id_type static_id = make_extension_id("track_extension");

    auto id() const -> id_type override
    {
        return static_id;
    }

    auto init(init_context&) -> init_result override
    {
        return init_result::success;
    }

    auto update(update_context&) -> void override
    {
    }

    using extension::track;
};

TEST_CASE("extension: track")
{
    signal<> signal;
    track_extension extension;

    bool called = false;
    extension.track(signal.connector().connect([&called]() {
        called = true;
    }));

    CHECK_EQ(signal.size(), 1);

    signal.emit();

    CHECK(called);
}

}} // namespace poc::workspace