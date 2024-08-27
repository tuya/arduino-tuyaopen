#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import argparse
import shutil

parser = argparse.ArgumentParser()
parser.add_argument('--build-path', type=str, help='Build path')
parser.add_argument('--sketch-path', type=str, help='Sketch path')
parser.add_argument('--sketch-name', type=str, help='Sketch name')

args = parser.parse_args()
buildPath = args.build_path
sketchPath = args.sketch_path
sketchName = args.sketch_name

# Normalize file paths
buildPath = os.path.normpath(buildPath)
sketchPath = os.path.normpath(sketchPath)

buildOutputPath = os.path.join(buildPath, "tuyaTmp", "output")
sketchOutputPath = os.path.join(sketchPath, "tuyaOutput")

# Check if the build output path exists
if not os.path.exists(buildOutputPath):
    print("Build output path does not exist: " + buildOutputPath)
    sys.exit(1)

# If the sketch output path exists, remove it
if os.path.exists(sketchOutputPath):
    shutil.rmtree(sketchOutputPath)

# Copy the binary dir to the sketch folder
shutil.copytree(buildOutputPath, sketchOutputPath)
