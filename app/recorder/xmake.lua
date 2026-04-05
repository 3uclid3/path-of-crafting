target("poc.recorder")
    set_kind("binary")

    add_deps("poc")

    add_files("src/**.cpp")
