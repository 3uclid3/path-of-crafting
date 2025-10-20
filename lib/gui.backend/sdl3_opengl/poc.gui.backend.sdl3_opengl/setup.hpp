#pragma once

namespace poc::platform {
class window;
} // namespace poc::platform

namespace poc::gui::backend::sdl3_opengl {

auto init(platform::window& window) -> void;
auto uninit() -> void;

} // namespace poc::gui::backend::sdl3_opengl
