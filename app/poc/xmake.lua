target("poc")
    set_kind("binary")
    set_group("app")

    add_deps(
        "poc.core",
        "poc.gui.core",
        "poc.gui.backend.sdl3_opengl",
        "poc.platform"
    )

    add_files("main.cpp")
