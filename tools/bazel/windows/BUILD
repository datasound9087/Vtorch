load("@rules_cc//cc/toolchains:cc_toolchain.bzl", "cc_toolchain")
load("//tools/bazel/windows:msvc.bzl", "cc_toolchain_config")
load(
    "//tools/bazel/windows:paths.bzl",
    "MSVC_BIN_PATH",
    "MSVC_COMPILER_EXE",
    "MSVC_DUMPBIN_EXE",
    "MSVC_LIB_EXE",
    "MSVC_LINKER_EXE",
    "MSVC_ML_EXE",
    "env_includes",
    "env_libs",
    "includes",
)

filegroup(name = "empty")

cc_toolchain_config(
    name = "msvc_x64",
    abi_libc_version = "local",
    abi_version = "local",
    archiver_flags = ["/MACHINE:X64"],
    compiler = "msvc-cl",
    cpu = "x64_windows",
    cxx_builtin_include_directories = includes(),
    dbg_mode_debug_flag = "/DEBUG",
    default_link_flags = ["/MACHINE:X64"],
    fastbuild_mode_debug_flag = "/DEBUG",
    host_system_name = "local",
    msvc_cl_path = MSVC_COMPILER_EXE,
    msvc_env_include = env_includes(),
    msvc_env_lib = env_libs(),
    msvc_env_path = MSVC_BIN_PATH,
    msvc_env_tmp = "C:/Windows/Temp",
    msvc_lib_path = MSVC_LIB_EXE,
    msvc_link_path = MSVC_LINKER_EXE,
    msvc_ml_path = MSVC_ML_EXE,
    shorten_virtual_includes = True,
    supports_parse_showincludes = True,
    target_libc = "msvcrt",
    target_system_name = "local",
    tool_paths = {
        "ar": MSVC_LIB_EXE,
        "ml": MSVC_ML_EXE,
        "cpp": MSVC_COMPILER_EXE,
        "gcc": MSVC_COMPILER_EXE,
        "gcov": "no-op",
        "ld": MSVC_LINKER_EXE,
        "nm": "no-op",
        "objcopy": "no-op",
        "objdump": "no-op",
        "strip": "no-op",
        "dumpbin": MSVC_DUMPBIN_EXE,
        "cpp-module-deps-scanner": "no-op",
    },
    toolchain_identifier = "msvc_x64",
)

cc_toolchain(
    name = "cc_toolchain_msvc_x86_x64",
    all_files = ":empty",
    ar_files = ":empty",
    as_files = ":empty",
    compiler_files = ":empty",
    dwp_files = ":empty",
    linker_files = ":empty",
    objcopy_files = ":empty",
    strip_files = ":empty",
    supports_param_files = 1,
    toolchain_config = ":msvc_x64",
    toolchain_identifier = "msvc_x64",
)

toolchain(
    name = "cc_toolchain_for_windows_x86_64",
    exec_compatible_with = [
        "@platforms//cpu:x86_64",
        "@platforms//os:windows",
    ],
    target_compatible_with = [
        "@platforms//cpu:x86_64",
        "@platforms//os:windows",
    ],
    toolchain = ":cc_toolchain_msvc_x86_x64",
    toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
)
