#include <poc.history/extension.hpp>

#include <poc.core/assert.hpp>

namespace poc::history {

auto extension::id() const -> id_type
{
    return static_id;
}

auto extension::init(init_context& context) -> init_result
{
    POC_VERIFY(context.add_drawer(_drawer, &drawer::draw));

    track(context.item_added().connect(&_drawer, &drawer::on_item_added));
    track(context.item_removed().connect(&_drawer, &drawer::on_item_removed));

    return init_result::success;
}

auto extension::update(update_context&) -> void
{
    // nothing to do
}

} // namespace poc::history
