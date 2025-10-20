#pragma once

#include <poc.gui.backend/def.hpp>
#include POC_GUI_BACKEND_HEADER(frame.hpp)

namespace poc::gui::backend {

inline auto new_frame() -> void
{
    POC_GUI_BACKEND::new_frame();
}

inline auto render() -> void
{
    POC_GUI_BACKEND::render();
}

} // namespace poc::gui::backend