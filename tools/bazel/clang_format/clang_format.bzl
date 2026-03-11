load("@rules_cc//cc/common:cc_info.bzl", "CcInfo")

def _get_files(ctx, attr):
    if not hasattr(ctx.rule.attr, attr):
        return []

    files = []

    # Get all files under target
    for file_target in getattr(ctx.rule.attr, attr):
        files += file_target.files.to_list()
    return files

def _format_file(ctx, package, file):
    filename = "{name}".format(
        name = file.short_path.removeprefix(package + "/"),
    )

    # Declare output file
    outfile = ctx.actions.declare_file(filename + ".clang_format.validation")
    script_file = ctx.actions.declare_file(filename + ".clang_format.bat")

    # Create args
    args = ctx.actions.args()
    args.add("--dry-run")
    args.add("--Werror")
    args.add(file.path)
    args.use_param_file("@%s", use_always = True)

    content_template = """
@echo off
"{clang_format}" %* > "{output_file}"
    """
    content = content_template.format(
        clang_format = ctx.attr._executable,
        output_file = outfile.path,
    )

    ctx.actions.write(
        output = script_file,
        content = content,
        is_executable = True,
    )

    ctx.actions.run(
        inputs = [file],
        outputs = [outfile],
        executable = script_file,
        arguments = [args],
        mnemonic = "ClangFormat",
    )

    return outfile

def _clang_format_aspect_impl(target, ctx):
    inputs = _get_files(ctx, "hdrs") + _get_files(ctx, "srcs")

    # Only run on targets for this WORKSPACE (no external deps)
    if target.label.workspace_root != "":
        return [OutputGroupInfo(_validation = depset())]

    # All output files for inputs
    outputs = []
    for file in inputs:
        outputs.append(_format_file(ctx, target.label.package, file))

    # All output files for deps
    dep_outputs = []
    for dep in ctx.rule.attr.deps:
        dep_outputs.append(dep[OutputGroupInfo]._validation)

    # _validation holds unsued outputs (bazel builtin validation output group)
    # Does not need to be requested via --output_groups
    return [OutputGroupInfo(_validation = depset(outputs, transitive = dep_outputs))]

clang_format_aspect = aspect(
    implementation = _clang_format_aspect_impl,
    attr_aspects = ["deps"],
    required_providers = [CcInfo],
    attrs = {
        "_clang_format_file": attr.label(
            default = Label("//:clang_format_file"),
            allow_single_file = True,
        ),
        "_executable": attr.string(
            default = "C:/Users/Sam/dev/clang-format.exe",
        ),
    },
)
