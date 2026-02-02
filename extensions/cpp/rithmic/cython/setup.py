import os
import sys
import platform
from setuptools import setup, Extension
from Cython.Build import cythonize

# --- SDK Detection Logic ---
current_dir = os.path.dirname(os.path.abspath(__file__))
# SDK is at ../sdk
sdk_root = os.path.abspath(os.path.join(current_dir, "../sdk"))
include_dir = os.path.join(sdk_root, "include")

system = sys.platform
req_files = []
lib_dir = ""
extra_link_args = []
libraries = [
    "RApiPlus-optimize",
    "OmneEngine-optimize",
    "OmneStreamEngine-optimize",
    "OmneChannel-optimize",
    "api-optimize",
    "kit-optimize",
    "ssl",
    "crypto",
    "z"
]

if system == "darwin":
    machine = platform.machine()
    if machine == "arm64":
        lib_dir = os.path.join(sdk_root, "darwin-20.6-arm64/lib")
    else:
        lib_dir = os.path.join(sdk_root, "darwin-10/lib")
    
    extra_link_args = [
        "-framework", "CoreFoundation",
        "-framework", "SystemConfiguration",
        "-framework", "Security",
        # Rithmic Static libs might need this
        "-Wl,-force_load" 
        # Actually usually just linking is enough, but sometimes we need specific order.
        # We will depend on setuptools order.
    ]
    # On Mac, some Rithmic libs have underscore prefix in filename like lib_api-optimize.a
    # But -lapi-optimize maps to libapi-optimize.a.
    # CMakeLists says: lib_api-optimize.a
    # linker expects -l_api-optimize? No, typically library names exclude 'lib'.
    # If filename is 'lib_api-optimize.a', then flag is '-l_api-optimize'.
    
    # We need to adjust library names based on physical files.
    # Let's fix library names below.

elif system.startswith("linux"):
    lib_dir = os.path.join(sdk_root, "linux-gnu-3.10.0-x86_64/lib")
    libraries.append("pthread")
    libraries.append("dl")

# Correct library names based on CMakeLists
# libRApiPlus-optimize.a -> RApiPlus-optimize
# lib_api-optimize.a -> _api-optimize
# lib_kit-optimize.a -> _kit-optimize

final_libraries = []
for lib in libraries:
    if lib == "api-optimize":
        final_libraries.append("_api-optimize")
    elif lib == "kit-optimize":
        final_libraries.append("_kit-optimize")
    else:
        final_libraries.append(lib)

extensions = [
    Extension(
        "rithmic_ext",
        ["rithmic.pyx"],
        include_dirs=[include_dir],
        library_dirs=[lib_dir],
        libraries=final_libraries,
        extra_link_args=extra_link_args,
        language="c++",
        extra_compile_args=["-std=c++17", "-O3"]
    )
]

setup(
    name="rithmic_ext",
    ext_modules=cythonize(extensions, compiler_directives={'language_level': "3"}),
)
