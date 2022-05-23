#!/bin/sh

rm dist/*
python3 -m build
pip install dist/scihook-0.0.6.tar.gz
python3 -m unittest
