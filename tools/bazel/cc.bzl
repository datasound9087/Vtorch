load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

def _my_binary_impl(**kwargs):
    cc_binary(
        **kwargs
    )

my_binary = _my_binary_impl

def _my_library_impl(**kwargs):
    cc_library(
        **kwargs
    )

my_library = _my_library_impl
