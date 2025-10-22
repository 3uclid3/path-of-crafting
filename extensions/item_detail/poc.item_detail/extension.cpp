#include <poc.item_detail/extension.hpp>

#include <poc.core/assert.hpp>

namespace poc::item_detail {

auto extension::id() const -> id_type
{
    return static_id;
}

auto extension::init(init_context& context) -> init_result
{
    POC_VERIFY(context.add_drawer(_drawer, &drawer::draw));

    track(context.selection_changed().connect(&_drawer, &drawer::on_selection_changed));

    return init_result::success;
}

auto extension::update(update_context&) -> void
{
    // nothing to do
}

} // namespace poc::item_detail
