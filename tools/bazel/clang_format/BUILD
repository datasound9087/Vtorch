load("@rules_python//python:defs.bzl", "py_binary")

filegroup(
    name = "clang_format_file",
    srcs = [".clang-format"],
    visibility = ["//visibility:public"],
)

py_binary(
    name = "wrapper",
    srcs = ["wrapper.py"],
    visibility = ["//visibility:public"],
)
