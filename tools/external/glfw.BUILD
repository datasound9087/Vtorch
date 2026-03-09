load("@rules_cc//cc:cc_library.bzl", "cc_library")

cc_library(
    name = "glfw",
    srcs = glob([
        "src/*.h",
        "src/*.c",
    ]),
    hdrs = glob(["include/**/*.h"]),
    defines = select({
        "@platforms//os:windows": ["_GLFW_WIN32"],
        "@platforms//os:linux": [
            "_GLFW_HAS_XF86VM",
            "_GLFW_X11",
        ],
        "//conditions:default": [],
    }),
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
)
