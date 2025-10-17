#pragma once

#include <functional>

#include <poc.workspace/view/drawer.hpp>
#include <poc/string.hpp>

namespace poc::workspace::view {

class registry
{
public:
    auto add(string id, drawer d) -> bool;
    auto remove(string_view id) -> bool;

    auto drawers() noexcept -> std::span<const drawer> { return _drawers; }

private:
    std::vector<string> _ids;
    std::vector<drawer> _drawers;
};

} // namespace poc::workspace::view
