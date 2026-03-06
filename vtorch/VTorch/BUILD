load("//tools/bazel:cc.bzl", "my_binary")

my_binary(
    name = "VTorch",
    srcs = glob(["*.cpp"]),
    deps = [
        "@glfw",
        "@glm",
        "@gsl",
        "@vk_sdk//:vulkan",
    ],
)
