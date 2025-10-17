add_requires("libsdl3", {configs = {shared = false}})

target("poc.platform")
    set_kind("static")
    set_group("lib")

    add_deps("poc.core")
    add_packages("libsdl3")

    if is_os("windows") then
        add_syslinks("opengl32")
    else 
        add_syslinks("GL", "dl", "pthread")
    end

    add_includedirs(".", { public = true })

    add_headerfiles("**.hpp")
    add_files("**.cpp")
