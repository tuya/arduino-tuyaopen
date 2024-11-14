#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import logging
import argparse

toolspath = os.path.dirname(os.path.realpath(__file__))
toolspath = os.path.normpath(toolspath)

sys.path.insert(0, os.path.join(toolspath))
import sketch_version

sketch_config_json_name = 'appConfig.json'

def log_config(level=logging.INFO):
    FORMAT = '[%(levelname)-8s][%(asctime)s][%(filename)s:%(lineno)d]%(message)s'
    logging.basicConfig(
        level=level,
        format=FORMAT,
        datefmt='%H:%M:%S'
    )

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--sketch-path", type=str, required=True, help="path to sketch")
    parser.add_argument("--build-path", type=str, required=True, help="path to build")
    parser.add_argument("-v", "--verbose", action='store_true', help="verbose output")
    return parser.parse_args()

if __name__ == "__main__":
    args = parse_args()
    if args.verbose:
        log_config(logging.DEBUG)
    else:
        log_config()

    logging.debug(f"Toolspath: {toolspath}")

    # Copy sketch/appConfig.json -> /tmp/.../tuyaTmp/appConfig/appConfig.json
    app_config_json = os.path.join(args.sketch_path, sketch_config_json_name)
    app_config_dir = os.path.join(args.build_path, "tuyaTmp", 'appConfig')
    if not sketch_version.sketch_version_write(app_config_json, app_config_dir):
        sys.exit(1)

    sys.exit(0)
