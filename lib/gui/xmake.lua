add_requires("imgui 1.92.1-docking", {configs = {sdl3 = true, opengl3 = true}})

target("poc.gui")
    set_kind("static")
    set_group("lib")

    add_deps("poc.core")
    add_packages("imgui")

    add_includedirs(".", { public = true })

    add_headerfiles("**.hpp")
    add_files("**.cpp")
