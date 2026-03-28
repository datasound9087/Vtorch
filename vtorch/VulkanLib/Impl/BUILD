load("@rules_cc//cc:cc_library.bzl", "cc_library")

# We want the VMA implementation to be treated separately
cc_library(
    name = "Impl",
    srcs = glob(["*.cpp"]),
    visibility = ["//vtorch/VulkanLib:__pkg__"],
    deps = [
        "@vk_sdk//:vulkan",
        "@vma",
    ],
)
