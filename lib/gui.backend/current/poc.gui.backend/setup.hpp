#pragma once

#include <poc.gui.backend/def.hpp>
#include POC_GUI_BACKEND_HEADER(setup.hpp)

namespace poc::gui::backend {

inline auto init(platform::window& window) -> void
{
    POC_GUI_BACKEND::init(window);
}

inline auto uninit() -> void
{
    POC_GUI_BACKEND::uninit();
}

} // namespace poc::gui::backend