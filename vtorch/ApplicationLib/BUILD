load("//tools/bazel:cc.bzl", "my_library")

package(default_visibility = ["//vtorch:__subpackages__"])

my_library(
    name = "ApplicationLib",
    srcs = glob(["*.cpp"]),
    hdrs = glob(["*.h"]),
    deps = [
        "//vtorch/LoggingLib",
        "@glfw",
        "@gsl",
    ],
)
