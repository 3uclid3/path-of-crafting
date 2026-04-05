function get_tests_runargs()
    if has_config("junit_reports") then
        return {
            "--reporters=junit",
            "--out=" .. path.join(os.projectdir(), "build", "poc.junit.xml")
        }
    else 
        return {}
    end
end

target("poc.unit")
    set_kind("binary")
    set_default(false)

    set_group("unit")
    add_tests("unit", {runargs = get_tests_runargs() })

    add_deps("poc")
    add_packages("doctest")
    
    add_defines("DOCTEST_CONFIG_USE_STD_HEADERS")

    add_files("src/**.cpp")
    