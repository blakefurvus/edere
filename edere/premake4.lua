project "edere"
    kind "ConsoleApp"
    language "C"

    includedirs { (BASE_DIR .. "/gui") }
    files { "src/*.c" }

    links { "SDL2_ttf", "SDL2" }