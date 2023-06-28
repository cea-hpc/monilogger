# SciHook

A Python and C++ instrumentation library for scientific computing.

![CI](https://github.com/cea-hpc/scihook/actions/workflows/cmake.yml/badge.svg)

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

Check everything is working fine:
```
python -m unittest
```

## Repository organization

This repository is structured as follows:

```
├── examples          -> examples of programs instrumented with SciHook
│   └── fibonacci     -> simple instrumented fibonacci program
├── include           -> SciHook include folder
├── src               -> source folder containing both C++ library code and Python bindings code
│   ├── bindings      -> C++ and Python code providing Python bindings
│   │   └── scihook   -> SciHook Python package
│   └── core          -> C++ library code (builds libscihook.so)
└── tests             -> unit tests for SciHook (pure Python)
```
