target("poc")
    set_kind("binary")
    set_group("app")

    add_deps(
        "poc.app",

        -- Extensions

        -- Debug Extensions
        "poc.gui_demo"
    )

    add_files("main.cpp")
