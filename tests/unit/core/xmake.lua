target("nubwork.core.tests.unit")
    set_kind("binary")
    set_group("tests.unit")

    set_default(false)

    add_tests("default")
    add_tests("junit", {
        kind = "binary",
        runargs = {
            "--reporters=junit",
            "--out=" .. junit_out("nubwork.core.tests.unit"),
        }
    })

    add_deps("nubwork.doctest", "nubwork.core")
    
    add_includedirs(".")
    add_files("**.cpp")
