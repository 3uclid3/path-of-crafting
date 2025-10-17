target("poc.item_blobs")
    set_kind("headeronly")
    set_group("tests.data")
    
    set_default(false)

    add_deps("poc.item")

    add_includedirs(".", {public = true})

