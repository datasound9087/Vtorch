load("@cc_compatibility_proxy//:proxy.bzl", _upstream_cc_binary = "cc_binary", _upstream_cc_library = "cc_library")
load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_library.bzl", "cc_library")

MY_CODE_FEATURES = [
    "external_include_paths",
    "all_warnings",
    "treat_warnings_as_errors",
]

def _my_binary_impl(
        name,
        deps,
        features,
        **kwargs):
    cc_binary(
        name = name,
        deps = deps,
        features = MY_CODE_FEATURES + (features or []),
        **kwargs
    )

my_binary = macro(
    implementation = _my_binary_impl,
    inherit_attrs = _upstream_cc_binary,
    attrs = {},
)

def _my_library_impl(
        name,
        deps,
        features,
        **kwargs):
    cc_library(
        name = name,
        deps = deps,
        features = MY_CODE_FEATURES + (features or []),
        **kwargs
    )

my_library = macro(
    implementation = _my_library_impl,
    inherit_attrs = _upstream_cc_library,
    attrs = {},
)
