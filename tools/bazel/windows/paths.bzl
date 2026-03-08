WIN_SDK_ROOT = "C:/Program Files (x86)/Windows Kits/10/"
WIN_SDK_INCLUDE_PATH = WIN_SDK_ROOT + "Include/10.0.26100.0/"
WIN_SDK_LIB_PATH = WIN_SDK_ROOT + "Lib/10.0.26100.0/"

MSVC_ROOT = "C:/Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.50.35717/"
MSVC_BIN_PATH = MSVC_ROOT + "bin/Hostx64/x64/"
MSVC_LIB_PATH = MSVC_ROOT + "lib/x64"
MSVC_INCLUDE_PATH = MSVC_ROOT + "include"

# MSVC executables
def msvc_exe(exe):
    return MSVC_BIN_PATH + exe

MSVC_LIB_EXE = msvc_exe("lib.exe")

MSVC_COMPILER_EXE = msvc_exe("cl.exe")

MSVC_LINKER_EXE = msvc_exe("link.exe")

MSVC_ML_EXE = msvc_exe("ml64.exe")

MSVC_DUMPBIN_EXE = msvc_exe("dumpbin.exe")

def includes():
    return [
        WIN_SDK_INCLUDE_PATH + "ucrt",
        WIN_SDK_INCLUDE_PATH + "shared",
        WIN_SDK_INCLUDE_PATH + "um",
        MSVC_INCLUDE_PATH,
    ]

def env_includes():
    return ";".join(includes())

def libs_paths():
    return [
        MSVC_LIB_PATH,
        WIN_SDK_LIB_PATH + "ucrt/x64",
        WIN_SDK_LIB_PATH + "um/x64",
    ]

def env_libs():
    return ";".join(libs_paths())
