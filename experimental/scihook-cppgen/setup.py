#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""The setup script."""

from setuptools import setup, find_packages

from scihook_cppgen import __version__


with open("README.md") as readme_file:
    readme = readme_file.read()

requirements = [
    "Click",
    "clang",
    "libclang",
    "Mako"
]

setup(
    author="Dorian Leroy",
    author_email="dorian.leroy@cea.fr",
    classifiers=[
        "Development Status :: 2 - Pre-Alpha",
        "Intended Audience :: Developers",
        "Natural Language :: English",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.6",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: Implementation :: CPython",
    ],
    description="An instrumentation generator for SciHook",
    entry_points={"console_scripts": ["cppgen=scihook_cppgen.cli.cli:cppgen"]},
    python_requires=">=3",
    install_requires=requirements,
    long_description=readme,
    include_package_data=True,
    keywords="scihook-cppgen",
    name="scihook-cppgen",
    packages=find_packages(),
    version=__version__,
    zip_safe=False,
)
