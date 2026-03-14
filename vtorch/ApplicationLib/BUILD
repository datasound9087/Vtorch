load("//tools/bazel:cc.bzl", "my_library")

package(default_visibility = ["//vtorch:__subpackages__"])

my_library(
    name = "ApplicationLib",
    srcs = glob(["*.cpp"]),
    hdrs = glob(["*.h"]),
    implementation_deps = [
        "//vtorch/LoggingLib",
        "//vtorch/VulkanLib",
    ],
    deps = [
        "//vtorch/RendererLib",
        "@glfw",
        "@gsl",
    ],
)
