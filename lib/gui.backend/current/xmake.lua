local gui_backend = get_config("gui_backend") or ""

target("poc.gui.backend")
    set_kind("headeronly")
    set_group("lib")

    add_deps("poc.gui.backend." .. gui_backend)

    add_defines("POC_GUI_BACKEND=" .. gui_backend, { public = true })

    add_includedirs(".", { public = true })

    add_headerfiles("**.hpp")
