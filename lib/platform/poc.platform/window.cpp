#include <poc.platform/window.hpp>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include <poc.core/utility/on_scope_exit.hpp>
#include <poc.platform/log.hpp>

namespace poc::platform {

window::window(const string& title, u32 width, u32 height) noexcept
    : _width(width)
    , _height(height)
{
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
    {
        POC_PLATFORM_CRITICAL("SDL_InitSubSystem failed: {}", SDL_GetError());
        return;
    }

    POC_PLATFORM_INFO("Video driver: {}", SDL_GetCurrentVideoDriver());

    // GL attributes
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);

    // Create window with OpenGL flag; high-DPI allowed by default
    constexpr auto windows_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    _handle = SDL_CreateWindow(title.data(), static_cast<int>(width), static_cast<int>(height), windows_flags);

    if (_handle == nullptr)
    {
        POC_PLATFORM_CRITICAL("SDL_CreateWindow failed: {}", SDL_GetError());
        return;
    }

    _gl_context = SDL_GL_CreateContext(_handle);

    if (_gl_context == nullptr)
    {
        POC_PLATFORM_CRITICAL("SDL_GL_CreateContext failed: {}", SDL_GetError());
        return;
    }

    SDL_GL_MakeCurrent(_handle, _gl_context);

    int gl_major = 0;
    int gl_minor = 0;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &gl_major);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &gl_minor);

    POC_PLATFORM_INFO("OpenGL {}.{}", gl_major, gl_minor);
    POC_PLATFORM_INFO("GL_VENDOR: {}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
    POC_PLATFORM_INFO("GL_RENDERER: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
    POC_PLATFORM_INFO("GL_VERSION: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

    // Default vsync on; ignore failure if driver refuses
    SDL_GL_SetSwapInterval(1);

    // Cache logical size
    _width = width;
    _height = height;

    _open = true;
}

window::~window() noexcept
{
    if (_gl_context)
    {
        SDL_GL_MakeCurrent(_handle, nullptr);
        SDL_GL_DestroyContext(_gl_context);
    }

    if (_handle)
    {
        SDL_DestroyWindow(_handle);
    }

    if (SDL_WasInit(SDL_INIT_VIDEO))
    {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }
}

auto window::close() noexcept -> void
{
    _open = false;
}

auto window::poll_events() noexcept -> void
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            _open = false;
            break;
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            if (event.window.windowID == SDL_GetWindowID(_handle))
            {
                _open = false;
            }
            break;
        case SDL_EVENT_WINDOW_RESIZED:
        {
            // Logical size change (points)
            int w = 0;
            int h = 0;
            SDL_GetWindowSize(_handle, &w, &h);
            _width = static_cast<u32>(w > 0 ? w : 0);
            _height = static_cast<u32>(h > 0 ? h : 0);
        }
        break;
        default:
            break;
        }
    }
}

auto window::clear() noexcept -> void
{
    glViewport(0, 0, static_cast<GLsizei>(_width), static_cast<GLsizei>(_height));
    glClearColor(0.10f, 0.10f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

auto window::present() noexcept -> void
{
    SDL_GL_SwapWindow(_handle);
}

auto window::set_title(const string& title) noexcept -> void
{
    SDL_SetWindowTitle(_handle, title.data());
}

auto window::resize(u32 width, u32 height) noexcept -> void
{
    _width = width;
    _height = height;
    SDL_SetWindowSize(_handle, width, height);
}

auto window::dpi_scale() const noexcept -> f32
{
    // Compute scale as drawable_px / window_points (X axis)
    int wp_w = 0;
    int wp_h = 0;
    int px_w = 0;
    int px_h = 0;

    SDL_GetWindowSize(_handle, &wp_w, &wp_h);
    SDL_GetWindowSizeInPixels(_handle, &px_w, &px_h);

    return (wp_w <= 0 || px_w <= 0) ? 1.0f : static_cast<f32>(px_w) / static_cast<f32>(wp_w);
}

} // namespace poc::platform