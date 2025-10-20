#include <poc.gui.backend.sdl3_opengl/setup.hpp>

#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>
#include <SDL3/SDL.h>

#include <poc.platform/window.hpp>

namespace poc::gui::backend::sdl3_opengl {

namespace {

static bool imgui_event_watch(void*, SDL_Event* e)
{
    ImGui_ImplSDL3_ProcessEvent(e);
    return true; // ignored by AddEventWatch
}

} // namespace

auto init(platform::window& window) -> void
{
    ImGui_ImplSDL3_InitForOpenGL(window.handle(), window.gl_context());
    ImGui_ImplOpenGL3_Init();

    SDL_AddEventWatch(imgui_event_watch, nullptr);
}

auto uninit() -> void
{
    SDL_RemoveEventWatch(imgui_event_watch, nullptr);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
}

} // namespace poc::gui::backend::sdl3_opengl
