add_requires("doctest")

-- Helper to build JUnit output path for a target
function junit_out(targetname)
    return path.join(os.projectdir(), "build", string.format("%s.junit.xml", targetname))
end

target("poc.doctest")
    set_kind("static")
    set_group("tests.utility")
    set_default(false)
    
    add_packages("doctest", {public = true})
    add_deps("poc.core")

    add_files("main.cpp")
    
    add_defines("DOCTEST_CONFIG_USE_STD_HEADERS", "DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS", {public = true})

includes(
    -- utility/data library
    "item_blobs",

    -- tests
    "core",
    "item"
)
