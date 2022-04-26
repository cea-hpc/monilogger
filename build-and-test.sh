#!/bin/sh

rm dist/*
python3 -m build
pip install dist/monilogger-0.0.4.tar.gz
python3 -m unittest
