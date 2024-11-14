#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import argparse
import logging
import shutil

toolspath = os.path.dirname(os.path.realpath(__file__))
toolspath = os.path.normpath(toolspath)

sys.path.insert(0, os.path.join(toolspath))
import fileutils

FORMAT = '[%(levelname)-8s][%(asctime)s][%(filename)s:%(lineno)d]%(message)s'
logging.basicConfig(
    level=logging.INFO,
    format=FORMAT,
    datefmt='%H:%M:%S'
)

parser = argparse.ArgumentParser()
parser.add_argument('--build-path', type=str, help='Build path')
parser.add_argument('--sketch-path', type=str, help='Sketch path')

args = parser.parse_args()
buildPath = args.build_path
sketchPath = args.sketch_path

# Normalize file paths
buildPath = os.path.normpath(buildPath)
sketchPath = os.path.normpath(sketchPath)

buildOutputPath = os.path.join(buildPath, "tuyaTmp", "output")
sketchOutputPath = os.path.join(sketchPath, "tuyaOutput")

# Check if the build output path exists
if not os.path.exists(buildOutputPath):
    logging.error("Build output path does not exist: " + buildOutputPath)
    sys.exit(1)

fileutils.delete_if_exists(sketchOutputPath)

# Copy the binary dir to the sketch folder
shutil.copytree(buildOutputPath, sketchOutputPath)

sys.exit(0)
