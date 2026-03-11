load("//tools/bazel:cc.bzl", "my_binary")

my_binary(
    name = "VTorchTestApp",
    srcs = glob(["*.cpp"]),
    deps = [
        "//vtorch/LoggingLib",
        "//vtorch/VulkanLib",
        "@gsl",
    ],
)
