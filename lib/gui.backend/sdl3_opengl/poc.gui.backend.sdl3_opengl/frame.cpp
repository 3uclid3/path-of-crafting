#include <poc.gui.backend.sdl3_opengl/frame.hpp>

#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>

namespace poc::gui::backend::sdl3_opengl {

auto new_frame() -> void
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
}

auto render() -> void
{
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

} // namespace poc::gui::backend::sdl3_opengl
