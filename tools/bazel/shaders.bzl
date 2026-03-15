load("@rules_vulkan//vulkan:defs.bzl", "slang_shader")

def _my_shader_impl(name, srcs, lang, profile, target, **kwargs):
    slang_shader(
        name = name,
        srcs = srcs,
        lang = lang,
        profile = profile,
        target = target,
        out = name + ".spirv",
        **kwargs
    )

my_shader = macro(
    implementation = _my_shader_impl,
    inherit_attrs = slang_shader,
    attrs = {
        "lang": attr.string(default = "slang"),
        "profile": attr.string(default = "spirv_1_4"),
        "target": attr.string(default = "spirv"),
        "out": None,
    },
)
