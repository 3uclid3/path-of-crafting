target("poc.gui.core")
    set_kind("static")
    set_group("lib")

    add_deps("poc.core")
    add_packages("imgui")

    add_includedirs(".", { public = true })

    add_headerfiles("**.hpp")
    add_files("**.cpp")
