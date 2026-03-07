def _get_files(ctx, attr):
    if not hasattr(ctx.rule.attr, attr):
        return []

    files = []

    # Get all files under target
    for file_target in getattr(ctx.rule.attr, attr):
        files += file_target.files.to_list()
    return files

def _format_file(ctx, package, file):
    # Declare output file
    outfile = ctx.actions.declare_file("{name}.clang-format_validation".format(
        name = file.short_path.removeprefix(package + "/"),
    ))

    # Retrieve clang format style file path
    style_file = ctx.attr._clang_format_file.files.to_list()[0].path

    # Create args
    args = ctx.actions.args()
    args.add(clang_format_info.path)
    args.add("--Werror")
    args.add("--dry-run")
    args.add(file.path)
    args.add("--style=file:{}".format(style_file))
    args.add(outfile.path)

    # Run clang-format
    ctx.actions.run(
        inputs = [file],
        outputs = [outfile],
        executable = ctx.executable._executable,
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
            default = Label("//tools/bazel/clang_format:clang_format_file"),
            allow_single_file = True,
        ),
        "_executable": attr.label(
            default = Label("C:/Users/Sam/dev/clang-format.exe"),
            executable = True,
            cfg = "exec",
        )
    },
)