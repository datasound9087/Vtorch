load("//tools/bazel:cc.bzl", "my_library")

package(default_visibility = ["//vtorch:__subpackages__"])

my_library(
    name = "EventLib",
    hdrs = glob(["*.h"]),
    implementation_deps = [
        "//vtorch/LoggingLib",
    ],
    deps = [
        "@gsl",
        "@eventbus",
    ],
)
