#!/bin/bash

# Copy required files
cp ../../README.md .
cp ../../LICENSE .
cp -rf ../../imgs .

# Install packages for packaging
pip3 install setuptools wheel twine

# Generate distribution files
python3 setup.py sdist bdist_wheel

# python3 -m twine upload dist/*