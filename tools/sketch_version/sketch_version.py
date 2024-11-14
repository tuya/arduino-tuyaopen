#coding=utf-8
# sketch_version/sketch_version_parse.py

import json
import logging
import re
import os
import shutil

defaultVension="1.0.0"

def sketch_version_parse(filePath):
    if not os.path.exists(filePath):
        return defaultVension

    try:
        with open(filePath, 'r') as file:
            data = json.load(file)
            version = data.get("PROJECT_VERSION", defaultVension)
            
            # Validate the version format
            if not re.match(r'^\d+\.\d+\.\d+$', version):
                raise ValueError(f"Incorrect version format, using default version {defaultVension}")
    except (json.JSONDecodeError, KeyError, ValueError) as e:
        logging.error(f"Parsing error: {e}")
        version = defaultVension

    return version

def sketch_version_write(jsonFile, outputDir):
    # Check if the output directory exists, if not, create it
    if not os.path.exists(outputDir):
        try:
            os.makedirs(outputDir)
            logging.debug(f"Created directory {outputDir}")
        except OSError as e:
            logging.error(f"Failed to create directory {outputDir}: {e}")
            return False

    output_json_file = os.path.join(outputDir, 'appConfig.json')
    output_version_file = os.path.join(outputDir, 'appBuildDefine.txt')

    # Copy the appConfig.json file to the output directory
    if os.path.exists(jsonFile):
        try:
            shutil.copy2(jsonFile, output_json_file)
            logging.debug(f"File {jsonFile} has been copied to {output_json_file}")
        except IOError:
            logging.error(f"Failed to copy {jsonFile} to {output_json_file}")
            return False

    version = sketch_version_parse(output_json_file)

    # Prepare the version string with the required format and a newline at the end
    versionStr = '-DPROJECT_VERSION=\\\"' + version + '\\\"\n'

    # Write the version number to the file
    try:
        with open(output_version_file, "w") as text_file:
            text_file.write(versionStr)
        logging.debug(f"Version number {versionStr} has been written to {output_version_file}")
    except IOError:
        logging.error(f"Failed to write to file {output_version_file}")
        return False
    
    return True