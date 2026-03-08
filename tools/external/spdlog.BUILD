load("@vtorch//tools/bazel:cc.bzl", "my_library")

my_library(
    name = "spdlog",
    srcs = glob(["src/*.cpp"]),
    hdrs = glob(["include/**/*.h"]),
    defines = ["SPDLOG_COMPILED_LIB"],
    features = ["-all_warnings"],
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
)
