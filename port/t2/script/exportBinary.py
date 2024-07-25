#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
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

buildOutputPath = os.path.join(buildPath, "output")
sketchOutputPath = os.path.join(sketchPath, "output")

# Check if the build output path exists
if not os.path.exists(buildOutputPath):
    print("Build output path does not exist: " + buildOutputPath)
    exit(1)

# If the sketch output path exists, remove it
if os.path.exists(sketchOutputPath):
    shutil.rmtree(sketchOutputPath)

# Copy the binary dir to the sketch folder
shutil.copytree(buildOutputPath, sketchOutputPath)

# Remove unnecessary files
## config.json
if os.path.exists(os.path.join(sketchOutputPath, "config.json")):
    os.remove(os.path.join(sketchOutputPath, "config.json"))
## {sketchName}.bin
if os.path.exists(os.path.join(sketchOutputPath, sketchName + ".bin")):
    os.remove(os.path.join(sketchOutputPath, sketchName + ".bin"))
## {sketchName}.cpr
if os.path.exists(os.path.join(sketchOutputPath, sketchName + ".cpr")):
    os.remove(os.path.join(sketchOutputPath, sketchName + ".cpr"))
## {sketchName}.out
if os.path.exists(os.path.join(sketchOutputPath, sketchName + ".out")):
    os.remove(os.path.join(sketchOutputPath, sketchName + ".out"))
## {sketchName}_enc.bin
if os.path.exists(os.path.join(sketchOutputPath, sketchName + "_enc.bin")):
    os.remove(os.path.join(sketchOutputPath, sketchName + "_enc.bin"))
