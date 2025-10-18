#include <poc.workspace/drawer_registry.hpp>

#include <poc.core/algorithm.hpp>

namespace poc::workspace {

auto drawer_registry::add(drawer_id id, drawer draw, drawing_phase phase, draw_priority priority) -> bool
{
    // ID must be unique throughout all phases
    if (std::ranges::any_of(_indexes, [&](const auto& indexes) { return indexes.find(id) != indexes.end(); }))
    {
        return false;
    }

    auto& metas = _metas[std::to_underlying(phase)];
    auto& drawers = _drawers[std::to_underlying(phase)];
    auto& indexes = _indexes[std::to_underlying(phase)];

    // Find insertion point based on priority
    auto it = std::ranges::lower_bound(
        metas,
        priority,
        {},
        [](const auto& meta) { return meta.priority; });

    it = metas.insert(it, drawer_meta{id, phase, priority});

    // insert drawer at the same index
    auto index = std::distance(metas.begin(), it);

    drawers.insert(drawers.begin() + index, std::move(draw));
    indexes[id] = index;

    for (std::size_t i = index + 1; i < drawers.size(); ++i)
    {
        auto& meta = metas[i];
        indexes[meta.id] = i;
    }

    return true;
}

auto drawer_registry::remove(drawer_id id) -> bool
{
    auto locate = [&]() -> std::optional<std::pair<std::size_t, std::size_t>> {
        for (std::size_t phase = 0; phase < _drawers.size(); ++phase)
        {
            if (auto it = _indexes[phase].find(id); it != _indexes[phase].end())
            {
                return std::make_optional<std::pair<std::size_t, std::size_t>>(phase, it->second);
            }
        }
        return std::nullopt;
    };

    auto location = locate();
    if (!location.has_value())
    {
        return false;
    }

    auto [phase, index] = location.value();

    auto& metas = _metas[phase];
    auto& drawers = _drawers[phase];
    auto& indexes = _indexes[phase];

    metas.erase(metas.begin() + index);
    drawers.erase(drawers.begin() + index);
    indexes.erase(id);

    // adjust indexes
    for (auto i = index; i < drawers.size(); ++i)
    {
        auto& meta = metas[i];
        indexes[meta.id] = i;
    }

    return true;
}

auto drawer_registry::draw_all(draw_context& ctx) -> void
{
    for (const auto& phase : _drawers)
    {
        for (const auto& draw : phase)
        {
            draw(ctx);
        }
    }
}

} // namespace poc::workspace
