target("poc.alerter")
    set_kind("binary")

    add_packages("argparse")
    add_deps("poc")

    add_files("src/**.cpp")

    add_installfiles("$(projectdir)/assets/sounds/alert.wav", {prefixdir = ""})

    after_build(function(target)
        os.cp("$(projectdir)/assets/sounds/alert.wav", target:targetdir())
    end)
