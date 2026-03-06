load("@vtorch//tools/bazel:cc.bzl", "my_library")

my_library(
    name = "gsl",
    hdrs = glob(["include/gsl/*"]),
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
)