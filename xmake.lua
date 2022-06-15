set_policy("build.ccache", false)

add_rules("mode.debug", "mode.release")

add_requires("nlohmann_json")

set_languages("cxx23")

target("ncs")
    set_kind("$(kind)")

    if is_kind("binary") then
        add_files("source/main.cpp")
    end

    add_headerfiles("include/(**.hpp)")
    add_headerfiles("third_party/color-console/include/(**.hpp)")

    add_includedirs(
        "include",
        "third_party/color-console/include"
    )

    add_packages("nlohmann_json")