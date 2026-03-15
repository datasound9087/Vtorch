load("//tools/bazel:cc.bzl", "my_library")

package(default_visibility = ["//vtorch:__subpackages__"])

my_library(
    name = "VulkanLib",
    srcs = glob(["*.cpp"]),
    hdrs = glob(["*.h"]),
    defines = [
        "VULKAN_HPP_NO_STRUCT_CONSTRUCTORS ",
        "VULKAN_HPP_DISPATCH_LOADER_DYNAMIC",
    ],
    implementation_deps = [
        "//vtorch/LoggingLib",
    ],
    deps = [
        "//vtorch/RendererLib",
        "@glfw",
        "@gsl",
        "@vk_sdk//:vulkan",
        "@vma",
    ],
)
