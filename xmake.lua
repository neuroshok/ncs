set_policy("build.ccache", false)

add_rules("mode.debug", "mode.release")

add_requires("nlohmann_json")
add_requires("tiny-process-library")

set_languages("cxx23")

target("ncs")
    set_kind("binary")
    add_files("source/**.cpp")
    add_files("module/**.cpp")

    add_includedirs(
        "include",
        "module",
        "third_party/color-console/include"
    )

    add_packages("tiny-process-library")
    add_packages("nlohmann_json")