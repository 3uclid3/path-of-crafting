add_requires("imgui 1.92.1-docking", {configs = {sdl3 = true, opengl3 = true}})
add_requires("libsdl3", {configs = {shared = false}})

target("poc.gui.backend.sdl3_opengl")
    set_kind("static")
    set_group("lib")

    add_deps("poc.gui", "poc.platform")
    add_packages("imgui", "libsdl3")

    if is_os("windows") then
        add_syslinks("opengl32")
    else 
        add_syslinks("GL", "dl", "pthread")
    end

    add_includedirs(".", { public = true })

    add_headerfiles("**.hpp")
    add_files("**.cpp")
