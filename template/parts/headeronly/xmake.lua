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
option_end()

add_requires("benchmark", "doctest")
   
target("xct")
    set_kind("headeronly")
    set_default(true)

    -- version
    set_configdir("$(builddir)/include/xct")
    add_configfiles("include/xct/version.hpp.in", { filename = "version.hpp" })
    add_includedirs("include", { public = true })
    add_headerfiles("$(builddir)/include/(**.hpp)")
    
    -- include
    add_includedirs("$(builddir)/include", { public = true })
    add_headerfiles("include/(**.hpp)")

    on_config(function(target)
        import("core.project.project")
        import("core.base.semver")
        local v = semver.new(project.version())
        target:set("configvar", "VERSION", tostring(v))
        target:set("configvar", "VERSION_MAJOR", v:major())
        target:set("configvar", "VERSION_MINOR", v:minor())
        target:set("configvar", "VERSION_PATCH", v:patch())
    end)
    
includes("tests")
