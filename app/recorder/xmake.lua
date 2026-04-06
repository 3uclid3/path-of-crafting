target("poc.recorder")
    set_kind("binary")

    add_packages("argparse")
    add_deps("poc")

    add_files("src/**.cpp")
