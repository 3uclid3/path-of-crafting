target("poc.workspace")
    set_kind("static")
    set_group("lib")

    add_deps("poc.item")

    add_includedirs(".", { public = true })

    add_headerfiles("**.hpp")
    add_files("**.cpp")
