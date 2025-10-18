set_project("Path of Crafting")
set_version("v0.1.0")
set_license("GPL-3.0-or-later")

set_languages("c++23")
set_exceptions("no-cxx")
set_warnings("allextra", "error")

set_toolchains("llvm")

if is_plat("windows") then
    set_runtimes("MT")
end

add_rules("mode.debug", "mode.release", "mode.coverage")
add_rules("plugin.compile_commands.autoupdate", { outputdir = "$(builddir)" })

includes(
    "app",
    "lib",
    "tests"
)
