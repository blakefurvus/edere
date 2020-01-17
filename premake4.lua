solution "edere"
    configurations { "debug", "release" }

    BASE_DIR = path.getabsolute(".")
    targetdir (BASE_DIR .. "/bin")
    objdir (BASE_DIR .. "/bin/int")

    configuration "debug"
        flags { "symbols" }

    configuration "release"
        defines { "NDEBUG" }
        flags { "optimize" }

include "edere"