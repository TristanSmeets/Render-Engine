local projectName = "Renderer"
local outputFolder = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

workspace "Render-Engine"
    filename (projectName)
    architecture "x64"
    configurations {"Debug", "Release"}
    location (projectName)

project (projectName)
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    location "%{prj.name}"
    staticruntime "off"

    targetdir (projectName .. "/bin/" .. outputFolder .. "/%{prj.name}")
    objdir (projectName .. "/bin-obj/" .. outputFolder .. "/%{prj.name}")

    files
    {
        "dependencies/imgui/**.h",
        "dependencies/imgui/**.cpp",
        "dependencies/glad/src/glad.c",
        "dependencies/stb/stb_image.h",
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
        "dependencies/stb",
        "dependencies/glad/include",
        "dependencies/GLFW/include",
        "dependencies/termcolor",
        "dependencies/assimp/include",
        "dependencies/imgui",
        "dependencies/glm"
    }

    libdirs
    {
        "dependencies/GLFW/lib-vc2017",
        "dependencies/assimp/lib"
    }

    links
    {
        "glfw3.lib",
        "assimp-vc141-mt.lib",
        "opengl32.lib"
    }

    filter "system:windows"
        systemversion "10.0.17763.0"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
