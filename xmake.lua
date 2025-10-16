set_project("nubwork")
set_version("v0.1.0")
set_license("GPL-3.0-or-later")

set_languages("c++23")
set_toolchains("llvm")

set_warnings("allextra", "error")

add_rules("mode.debug", "mode.release", "mode.coverage")
add_rules("plugin.compile_commands.autoupdate", { outputdir = "$(builddir)" })

includes(
    "app",
    "lib",
    "tests"
)
