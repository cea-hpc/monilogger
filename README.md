# SciHook

A Python and C++ instrumentation library for scientific computing.

## Installation

### From source

This installs SciHook's shared library and header file to <install_path_for_scihook>:

```
mkdir <install_path_for_scihook>
git clone git@github.com:cea-hpc/scihook.git
cd scihook
mkdir build
cmake -B build -DCMAKE_INSTALL_PREFIX=<install_path_for_scihook>
cmake --build build
cmake --install build
```

This installs SciHook's Python module in the current virtual environment or Python installation:
```
SCIHOOK_ROOT=<install_path_for_scihook> pip install .
```
