#include <poc.workspace/host.hpp>

#include <poc.core/assert.hpp>
#include <poc.core/types.hpp>
#include <poc.workspace/context.hpp>
#include <poc.workspace/log.hpp>

namespace poc::workspace {

auto host::init() -> bool
{
    init_context context{_drawers};

    for (auto* ext : _extensions)
    {
        auto result = ext->init(context);
        if (result != extension::init_result::success)
        {
            POC_WORKSPACE_ERROR("Failed to initialize extension: {} ({})", ext->id(), magic_enum::enum_name(result));
            return false;
        }
    }

    return true;
}

auto host::uninit() -> void
{
    _extensions.clear();
    _extension_by_ids.clear();
}

auto host::update() -> void
{
    drain_items();
    update_extensions();
}

auto host::draw() -> void
{
    draw_context context;
    _drawers.draw_all(context);
}

auto host::add_extension(std::unique_ptr<extension> ext) -> extension&
{
    auto raw_ptr = ext.get();

    POC_ASSERT(raw_ptr != nullptr);
    POC_ASSERT(raw_ptr->id() != extension::static_id);
    POC_ASSERT(!has_extension(raw_ptr->id()));

    _extensions.push_back(raw_ptr);
    _extension_by_ids[raw_ptr->id()] = std::move(ext);

    return *raw_ptr;
}

auto host::remove_extension(extension_id id) -> bool
{
    auto it = _extension_by_ids.find(id);
    if (it == _extension_by_ids.end())
        return false;

    auto ext_ptr = it->second.get();

    auto ext_it = std::ranges::find(_extensions, ext_ptr);
    _extensions.erase(ext_it, ext_it + 1);
    _extension_by_ids.erase(it);

    return true;
}

auto host::has_extension(extension_id id) const -> bool
{
    return find_extension(id) != nullptr;
}

auto host::find_extension(extension_id id) const -> const extension*
{
    auto it = _extension_by_ids.find(id);
    return it != _extension_by_ids.end() ? it->second.get() : nullptr;
}

auto host::find_extension(extension_id id) -> extension*
{
    auto it = _extension_by_ids.find(id);
    return it != _extension_by_ids.end() ? it->second.get() : nullptr;
}

auto host::drain_items() -> void
{
    if (_ingester.drain(_sink_items) > 0)
    {
        for (item& it : _sink_items)
        {
            _store.items().add(std::move(it));
        }
    }

    _sink_items.clear();
}

auto host::update_extensions() -> void
{
    update_context context{_drawers};
    for (auto* ext : _extensions)
    {
        ext->update(context);
    }
}

} // namespace poc::workspace
