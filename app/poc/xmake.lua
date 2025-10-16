target("poc")
    set_kind("binary")
    set_group("app")

    add_deps(
        "poc.core"
    )

    add_files("main.cpp")
