target("poc.app")
    set_kind("static")
    set_group("lib")

    add_deps(
        "poc.core",
        "poc.gui",
        "poc.gui.backend",
        "poc.item",
        "poc.platform",
        "poc.workspace",

        {public = true}
    )

    add_includedirs(".", { public = true })

    add_headerfiles("**.hpp")
    add_files("**.cpp")
