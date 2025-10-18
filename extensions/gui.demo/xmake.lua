target("poc.app")
    set_kind("static")
    set_group("lib")

    set_default(false)

    add_deps(
        "poc.core",
        "poc.gui.core",
        "poc.workspace"
    )

    add_includedirs(".", { public = true })

    add_headerfiles("**.hpp")
    add_files("**.cpp")
