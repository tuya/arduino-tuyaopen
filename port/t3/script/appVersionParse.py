#coding=utf-8

import json
import logging
import re
import os

defaultVension="1.0.0"

#
## log config
#
FORMAT = '[%(levelname)s][%(asctime)s][%(filename)s:%(lineno)d]%(message)s'
logging.basicConfig(
    level=logging.INFO,
    format=FORMAT,
    datefmt='%H:%M:%S'
)

def appVersionParse(filePath):
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

def appVersionWrite(jsonFile, outputFile):
    version = appVersionParse(jsonFile)

    # Check if the file exists, if so, delete it
    if os.path.exists(outputFile):
        try:
            os.remove(outputFile)
            logging.info(f"{outputFile} already existed and has been deleted")
        except OSError as e:
            logging.error(f"Failed to delete {outputFile}: {e}")
            return

    # Prepare the version string with the required format and a newline at the end
    versionStr = '-DPROJECT_VERSION=\\\"' + version + '\\\"\n'

    # Write the version number to the file
    try:
        with open(outputFile, "w") as text_file:
            text_file.write(versionStr)
        logging.debug(f"Version number {versionStr} has been written to {outputFile}")
    except IOError:
        logging.error(f"Failed to write to file {outputFile}")