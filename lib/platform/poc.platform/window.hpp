#pragma once

#include <poc.core/types.hpp>

struct SDL_Window;
struct SDL_GLContextState;

namespace poc::platform {

using gl_context = SDL_GLContextState;

class window
{
public:
    using handle_type = SDL_Window;
    using gl_context_type = gl_context;

    window(const string& title, u32 width, u32 height) noexcept;
    ~window() noexcept;

    // lifecycle
    auto is_open() const noexcept -> bool;
    auto close() noexcept -> void;

    // event loop
    auto poll_events() noexcept -> void;

    // rendering
    auto clear() noexcept -> void;
    auto present() noexcept -> void;

    // window control
    auto set_title(const string& title) noexcept -> void;
    auto resize(u32 width, u32 height) noexcept -> void;

    // info
    [[nodiscard]] auto width() const noexcept -> u32;
    [[nodiscard]] auto height() const noexcept -> u32;
    [[nodiscard]] auto dpi_scale() const noexcept -> f32;

    [[nodiscard]] auto handle() const noexcept -> handle_type*;
    [[nodiscard]] auto gl_context() const noexcept -> gl_context_type*;

private:
    handle_type* _handle{nullptr};
    gl_context_type* _gl_context{nullptr};

    u32 _width{0};
    u32 _height{0};

    bool _open{false};
};

inline auto window::is_open() const noexcept -> bool
{
    return _open;
}

inline auto window::width() const noexcept -> u32
{
    return _width;
}

inline auto window::height() const noexcept -> u32
{
    return _height;
}

inline auto window::handle() const noexcept -> handle_type*
{
    return _handle;
}

inline auto window::gl_context() const noexcept -> gl_context_type*
{
    return _gl_context;
}

} // namespace poc::platform