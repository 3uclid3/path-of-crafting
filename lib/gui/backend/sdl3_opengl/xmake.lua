target("poc.gui.backend.sdl3_opengl")
    set_kind("static")
    set_group("lib")

    add_deps("poc.gui.core", "poc.platform")
    add_packages("imgui")

    add_includedirs(".", { public = true })

    add_headerfiles("**.hpp")
    add_files("**.cpp")
