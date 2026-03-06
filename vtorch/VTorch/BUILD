load("//tools/bazel:cc.bzl", "my_binary")

my_binary(
    name = "VTorch",
    srcs = glob(["*.cpp"]),
    deps = [
        "@glfw",
        "@gsl",
        "@vk_sdk//:vulkan",
        "@glm",
    ]
)