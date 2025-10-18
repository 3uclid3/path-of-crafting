

#include <doctest/doctest.h>

#include <poc.workspace/host.hpp>

#include <chrono>
#include <thread>

#include <poc.item_blobs/item_blobs.hpp>

namespace poc::workspace { namespace {

template<typename T>
class extension_base_fake : public extension
{
public:
    auto id() const -> id_type override
    {
        return T::static_id;
    }

    auto inited() const -> bool
    {
        return _inited;
    }

    auto init(init_context&) -> init_result override
    {
        _inited = true;
        return _init_result;
    }

    auto updated() const -> bool
    {
        return _updated;
    }

    auto update(update_context&) -> void override
    {
        _updated = true;
    }

    auto set_init_result(init_result result) -> void
    {
        _init_result = result;
    }

private:
    init_result _init_result{init_result::success};

    bool _inited{false};
    bool _updated{false};
};

class extension_one_fake : public extension_base_fake<extension_one_fake>
{
public:
    static constexpr id_type static_id = "one_fake";
};

class extension_two_fake : public extension_base_fake<extension_two_fake>
{
public:
    static constexpr id_type static_id = "two_fake";
};

TEST_CASE("host::add_extension")
{
    host h{};

    h.add_extension<extension_one_fake>();

    CHECK(h.has_extension<extension_one_fake>());
    CHECK_FALSE(h.has_extension<extension_two_fake>());
}

TEST_CASE("host::find_extension")
{
    host h{};

    auto& first = h.add_extension<extension_one_fake>();

    CHECK(h.find_extension<extension_one_fake>() == &first);
    CHECK(h.find_extension<extension_two_fake>() == nullptr);
}

TEST_CASE("host::remove_extension")
{
    host h{};

    h.add_extension<extension_one_fake>();

    CHECK(h.remove_extension<extension_one_fake>());
    CHECK_FALSE(h.has_extension<extension_one_fake>());
}

TEST_CASE("host::init: succeeds when all extensions initialize")
{
    host h{};

    auto& first = h.add_extension<extension_one_fake>();
    auto& second = h.add_extension<extension_two_fake>();

    CHECK(h.init());

    CHECK(first.inited());
    CHECK(second.inited());
}

TEST_CASE("host::init: stops after failing extension")
{
    host h{};

    auto& failing = h.add_extension<extension_one_fake>();
    auto& untouched = h.add_extension<extension_two_fake>();

    failing.set_init_result(extension::init_result::failure);

    CHECK_FALSE(h.init());

    CHECK(failing.inited());
    CHECK_FALSE(untouched.inited());
}

TEST_CASE("host::uninit: clears registered extensions")
{
    host h{};

    h.add_extension<extension_one_fake>();
    h.add_extension<extension_two_fake>();

    h.uninit();

    CHECK_FALSE(h.has_extension<extension_one_fake>());
    CHECK_FALSE(h.has_extension<extension_two_fake>());
}

TEST_CASE("host::update: forwards to extensions")
{
    host h{};

    auto& ext = h.add_extension<extension_one_fake>();

    CHECK_FALSE(ext.updated());

    h.update();

    CHECK(ext.updated());
}

TEST_CASE("host::update: drains ingester item into repository")
{
    static constexpr std::chrono::milliseconds timeout{200};

    host h{};
    h.init();
    h.ingester().submit(item_blobs::rarity_normal);

    const auto deadline = std::chrono::steady_clock::now() + timeout;
    const auto& items = h.store().items();

    while (std::chrono::steady_clock::now() < deadline)
    {
        h.update();

        if (!items.empty())
        {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    CHECK_FALSE(items.empty());
}

}} // namespace poc::workspace
