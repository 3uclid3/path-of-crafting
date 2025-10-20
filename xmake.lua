set_project("Path of Crafting")
set_version("v0.1.0")
set_license("GPL-3.0-or-later")

set_languages("c++23")
set_exceptions("no-cxx")
set_warnings("allextra", "error")

set_toolchains("llvm")

if is_plat("windows") then
    set_runtimes("MT")
elseif is_plat("linux") then 
    set_runtimes("c++_static")
end

add_rules("mode.debug", "mode.release", "mode.coverage")
add_rules("plugin.compile_commands.autoupdate", { outputdir = "$(builddir)" })

option("gui_backend")
    set_default("sdl3_opengl")
    set_values("sdl3_opengl")
    set_showmenu(true)
    set_description("Select GUI backend")

includes(
    "app",
    "extensions",
    "lib",
    "tests"
)
