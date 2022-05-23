#!/bin/sh

rm dist/*
python3 -m build
pip install dist/scihook-0.1.0.tar.gz
python3 -m unittest
