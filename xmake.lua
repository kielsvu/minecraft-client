set_project("outcome")
set_version("1.0.0")
set_languages("c++20")

add_rules("mode.release", "mode.debug")

target("Outcome")
    set_kind("shared")
    set_filename("libOutcome.so")

    add_includedirs("include", {public = true})
    add_includedirs("vendor/imgui", "vendor/imgui/backends")
    add_files("src/**.cpp")
    add_files("vendor/imgui/*.cpp")
    add_files("vendor/imgui/backends/imgui_impl_opengl3.cpp")

    add_linkdirs("vendor/lib")
    add_links("And64InlineHook", "log", "android", "EGL", "GLESv3", "dl")

    add_cxflags("-fvisibility=hidden", "-fno-exceptions", "-fno-rtti",
                "-O2", "-ffunction-sections", "-fdata-sections")
    add_ldflags("-Wl,--gc-sections", "-Wl,--no-undefined")

    if is_plat("android") then
        add_ldflags("-Wl,-soname,libOutcome.so")
end
