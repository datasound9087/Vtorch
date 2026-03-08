load("@vtorch//tools/bazel:cc.bzl", "my_library")

my_library(
    name = "glm",
    hdrs = glob([
        "glm/**/*.hpp",
        "glm/**/*.h",
    ]),
    features = ["-all_warnings"],
    textual_hdrs = glob(["glm/**/*.inl"]),
    visibility = ["//visibility:public"],
)
