-- premake4.lua
workspace "bertrandt_workshop"
    architecture "x64"
    configurations
    {
        "Debug",
        "Release"
    }

outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "ik_lib"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir ("bin/" .. outputDir)
    objdir ("build/" .. outputDir)
    debugdir ("bin/" .. outputDir)

    libdirs { "libs" }
    flags { "MultiProcessorCompile" }

    files
    {
        "src/**.cpp",
        "inc/**.h",
	"main.cpp",
        "%{prj.name}/inc/**.h",
        "%{prj.name}/src/**.cpp"
    }
    includedirs
    {
        "inc/",
        "%{prj.name}/inc/"
    }

    filter "system:Windows"
        systemversion "latest"
        staticruntime "On"
        defines
        { }
    filter "system:Linux"
        systemversion "latest"
        staticruntime "On"
        defines
        { }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        --warnings "Everything"
        defines
        {
            "IK_DEFINE_DEBUG"
        }
    filter "configurations:Release" 
        runtime "Release"
        optimize "On"
        --warnings "Default"
        defines
        {
            "IK_DEFINE_RELEASE"
        }