#pragma once

#include <functional>

namespace poc::workspace::view {

class draw_context
{
};

using drawer = std::function<void(draw_context&)>;

} // namespace poc::workspace::view
