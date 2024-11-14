# fileutils/file_operations.py

import os
import shutil
import logging

def delete_if_exists(path):
    if not os.path.exists(path):
        logging.debug(f"{path} does not exist")
        return

    if os.path.isfile(path):
        os.remove(path)
        logging.debug(f"Deleted file {path}")
    elif os.path.isdir(path):
        shutil.rmtree(path)
        logging.debug(f"Deleted directory {path}")
    else:
        logging.warning(f"Unknown file type {path}")

def copy_with_overwrite(source, target):
    if not os.path.exists(source):
        logging.error(f"Source file {source} does not exist")
        return False
    
    if os.path.exists(target):
        delete_if_exists(target)
    
    if os.path.isdir(source):
        shutil.copytree(source, target)
    elif os.path.isfile(source):
        shutil.copy2(source, target)
    else:
        logging.error(f"Unknown file type {source}")
        return False

