load("@rules_cc//cc:cc_library.bzl", "cc_library")

cc_library(
    name = "spdlog",
    srcs = glob(["src/*.cpp"]),
    hdrs = glob(["include/**/*.h"]),
    defines = ["SPDLOG_COMPILED_LIB"],
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
)
