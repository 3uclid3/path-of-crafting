target("poc")
    set_kind("static")
    set_default(true)

    add_includedirs("include", { public = true })
    add_headerfiles("include/(**.hpp)")

    add_files("src/**.cpp")

    add_syslinks("user32")
