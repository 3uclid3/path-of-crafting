function get_tests_runargs()
    if has_config("junit_reports") then
        return {
            "--reporters=junit",
            "--out=" .. path.join(os.projectdir(), "build", "xct.junit.xml")
        }
    else 
        return {}
    end
end

target("xct.unit")
    set_kind("binary")
    set_default(false)

    set_group("unit")
    add_tests("unit", {runargs = get_tests_runargs() })

    add_deps("xct")
    add_packages("doctest")
    
    add_defines("DOCTEST_CONFIG_USE_STD_HEADERS")

    add_includedirs("include")
    add_headerfiles("include/**.hpp")
    add_files("src/**.cpp")
    