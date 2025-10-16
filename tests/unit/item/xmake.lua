target("poc.item.tests.unit")
    set_kind("binary")
    set_group("tests.unit")

    set_default(false)

    add_tests("default")
    add_tests("junit", { runargs = {
        "--reporters=junit",
        "--out=" .. junit_out("poc.item.tests.unit")
    }})

    add_deps("poc.doctest", "poc.item_blobs", "poc.item")
    
    add_includedirs(".")
    add_files("poc.item/**.cpp")
