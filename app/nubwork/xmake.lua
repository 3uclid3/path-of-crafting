target("nubwork")
    set_kind("binary")
    set_group("app")

    add_deps(
        "nubwork.core"
    )

    add_files("main.cpp")
