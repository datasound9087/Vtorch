load("//tools/bazel:cc.bzl", "my_library")

package(default_visibility = ["//vtorch:__subpackages__"])

my_library(
    name = "VulkanLib",
    srcs = glob(["*.cpp"]),
    hdrs = glob(["*.h"]),
    defines = [
        "VULKAN_HPP_NO_STRUCT_CONSTRUCTORS ",
        # We want to be able to handle vk::Result::eErrorOutOfDateKHR without throwing an exception.
        "VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS",
    ],
    implementation_deps = [
        "//vtorch/LoggingLib",
    ],
    deps = [
        "//vtorch/EventLib",
        "//vtorch/RendererLib",
        "//vtorch/VulkanLib/Impl",
        "@glfw",
        "@gsl",
        "@vk_sdk//:vulkan",
        "@vma",
    ],
)
