set_project("xct")
set_version("0.1.0")
set_license("GPL-3.0-or-later")

set_languages("cxx23")
set_exceptions("no-cxx")
set_warnings("allextra", "error")

add_rules("mode.debug", "mode.release", "mode.coverage")
add_rules("plugin.compile_commands.autoupdate", { outputdir = get_config("builddir") })

option("junit_reports")
    set_default(false)
    set_showmenu(true)
    set_description("Enable JUnit XML report generation for tests")

add_requires("benchmark", "doctest")
   
includes("app", "lib", "tests")
