target("poc.item_detail")
    set_kind("static")
    set_group("lib")

    set_default(false)

    add_deps(
        "poc.gui",
        "poc.workspace"
    )

    add_includedirs(".", { public = true })

    add_headerfiles("**.hpp")
    add_files("**.cpp")
