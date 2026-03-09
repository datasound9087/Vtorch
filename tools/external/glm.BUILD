load("@rules_cc//cc:cc_library.bzl", "cc_library")

cc_library(
    name = "glm",
    hdrs = glob([
        "glm/**/*.hpp",
        "glm/**/*.h",
    ]),
    features = ["-all_warnings"],
    textual_hdrs = glob(["glm/**/*.inl"]),
    visibility = ["//visibility:public"],
)
