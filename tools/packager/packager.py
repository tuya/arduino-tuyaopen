#coding=utf-8
# packager/packager.py

import os
import shutil
import logging
import platform
import subprocess

from .chip_info import chip_info_class
from .packager_t2 import get_qio_binary_t2
from .packager_t3_t5 import get_qio_binary_t3_t5
from .packager_ln882h import get_qio_binary_ln882h

class packager_class():
    def __init__(self, chip, compiler_path, tools_path, output_path, sketch_name, sketch_version):
        self.chip_info = chip_info_class(chip, compiler_path, tools_path, output_path, sketch_name, sketch_version)

    def elf_to_binary(self):
        objcopy_tool = os.path.join(self.chip_info.compiler_path, 'bin', 'arm-none-eabi-objcopy')
        elf_file = os.path.join(self.chip_info.output_path, self.chip_info.sketch_name + '.elf')
        bin_file = os.path.join(self.chip_info.output_path, self.chip_info.sketch_name + '.bin')

        if platform.system() == 'Windows':
            objcopy_tool += '.exe'

        if not os.path.exists(objcopy_tool):
            logging.error(f"{objcopy_tool} not found")
            return False

        if not os.path.exists(elf_file):
            logging.error(f"{elf_file} file not found")
            return False

        objcopy_command = [
            objcopy_tool,
            '-O', 'binary',
            elf_file,
            bin_file
        ]
        logging.debug("objcopy_command: " + ' '.join(objcopy_command))
        result = subprocess.run(objcopy_command)
        if result.returncode != 0 or not os.path.exists(bin_file):
            logging.error("objcopy failed")
            return False

        self.chip_info.bin_file = bin_file # .elf -> .bin

        return True

    def get_qio_binary(self):
        if self.chip_info.chip == 't2':
            return get_qio_binary_t2(self.chip_info)
        elif self.chip_info.chip == 't3' or self.chip_info.chip == 't5':
            return get_qio_binary_t3_t5(self.chip_info)
        elif self.chip_info.chip == 'ln882h':
            return get_qio_binary_ln882h(self.chip_info)
        else:
            return False

    def get_arduino_upload_binary(self, arduino_upload_binary):
        if not os.path.exists(self.chip_info.bin_file_QIO):
            logging.error("bin_file_QIO not found")
            return False

        if os.path.exists(arduino_upload_binary):
            os.remove(arduino_upload_binary)

        shutil.copy2(self.chip_info.bin_file_QIO, arduino_upload_binary)

        return True