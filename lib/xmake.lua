target("poc")
    set_kind("static")
    set_default(true)

    -- includes
    add_includedirs("include", { public = true })
    add_headerfiles("include/(**.hpp)")

    -- sources
    add_files("src/**.cpp")
