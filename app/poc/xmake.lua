target("poc")
    set_kind("binary")
    set_group("app")

    add_deps(
        "poc.app",

        -- Extensions
        "poc.history",
        "poc.item_detail",

        -- Debug Extensions
        "poc.gui.debug"
    )

    add_files("main.cpp")
