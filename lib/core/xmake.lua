add_requires("magic_enum", "spdlog")

target("poc.core")
    set_kind("static")
    set_group("lib")

    add_packages("magic_enum", "spdlog", { public = true })

    add_includedirs(".", { public = true })

    add_headerfiles("**.hpp")
    add_files("**.cpp")

    add_defines("SPDLOG_DISABLE_DEFAULT_LOGGER", "SPDLOG_USE_STD_FORMAT", { public = true })
    if is_mode("debug") then
        add_defines("SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE", { public = true })
    elseif is_mode("release") then
        add_defines("SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO", { public = true })
    elseif is_mode("coverage") then
        add_defines("SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_OFF", "POC_ASSERTS_ENABLED=0", { public = true })
    end