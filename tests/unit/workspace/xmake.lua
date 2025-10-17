target("poc.workspace.tests.unit")
    set_kind("binary")
    set_group("tests.unit")

    set_default(false)

    add_tests("default")
    add_tests("junit", { runargs = {
        "--reporters=junit",
        "--out=" .. junit_out("poc.workspace.tests.unit")
    }})

    add_deps("poc.doctest", "poc.item_blobs", "poc.workspace")
    
    add_includedirs(".")
    add_files("poc.workspace/**.cpp")
