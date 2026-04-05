target("xct")
    set_kind("static")
    set_default(true)

    -- version
    set_configdir("$(builddir)/include/xct")
    add_configfiles("include/xct/version.hpp.in", { filename = "version.hpp" })
    add_includedirs("include", { public = true })
    add_headerfiles("$(builddir)/include/(**.hpp)")
    
    -- include
    add_includedirs("$(builddir)/include", { public = true })
    add_headerfiles("include/(**.hpp)")

    -- sources
    add_files("src/**.cpp")

    on_config(function(target)
        import("core.project.project")
        import("core.base.semver")
        local v = semver.new(project.version())
        target:set("configvar", "VERSION", tostring(v))
        target:set("configvar", "VERSION_MAJOR", v:major())
        target:set("configvar", "VERSION_MINOR", v:minor())
        target:set("configvar", "VERSION_PATCH", v:patch())
    end)
    