load("//tools/bazel:cc.bzl", "my_binary")

my_binary(
    name = "VTorchTestApp",
    srcs = glob(["*.cpp"]),
    # features = ["subsystem_windows"],
    deps = [
        "//vtorch/ApplicationLib",
        "//vtorch/LoggingLib",
        "@gsl",
    ],
)
