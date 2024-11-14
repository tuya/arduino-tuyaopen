import os
import logging
import shutil
import platform
import subprocess
import json

from .chip_info import *

t3_config_str = '''
{
    "magic": "FreeRTOS",
    "version": "0.1",
    "count": 2,
    "section": [
        {
            "firmware": "bootloader.bin",
            "version": "2M.1220",
            "partition": "bootloader",
            "start_addr": "0x00000000",
            "size": "64K"
        },
        {
            "firmware": "app.bin",
            "version": "2M.1220",
            "partition": "app",
            "start_addr": "0x00010000",
            "size": "2176K"
        }
    ]
}
'''

t5_config_str = '''
{
    "magic": "FreeRTOS",
    "version": "0.1",
    "count": 2,
    "section": [
        {
            "firmware": "bootloader.bin",
            "version": "2M.1220",
            "partition": "bootloader",
            "start_addr": "0x0",
            "size": "64K"
        },
        {
            "firmware": "app.bin",
            "version": "2M.1220",
            "partition": "app",
            "start_addr": "0x10000",
            "size": "3520K"
        }
    ]
}
'''

def get_qio_binary_t3_t5(chip_info):
    logging.debug(f"platform system: {platform.system()}")
    if platform.system() == 'Windows':
        cmake_Gen_image_tools = os.path.join(chip_info.tools_path, 'windows', 'cmake_Gen_image.exe')
        cmake_encrypt_crc_tool = os.path.join(chip_info.tools_path, 'windows', 'cmake_encrypt_crc.exe')
    elif platform.system() == 'Linux':
        cmake_Gen_image_tools = os.path.join(chip_info.tools_path, 'linux', 'cmake_Gen_image')
        cmake_encrypt_crc_tool = os.path.join(chip_info.tools_path, 'linux', 'cmake_encrypt_crc')
    elif platform.system() == 'Darwin':
        mac_arch = platform.machine()
        logging.info(f"MAC machine is: {mac_arch}")
        cmake_Gen_image_tools = os.path.join(chip_info.tools_path, 'mac', mac_arch, 'cmake_Gen_image')
        cmake_encrypt_crc_tool = os.path.join(chip_info.tools_path, 'mac', mac_arch, 'cmake_encrypt_crc')
    else:
        logging.error(f"Unknown OS: {platform.system()}")
        return False

    bootloader_file = os.path.join(chip_info.tools_path, chip_info.chip + '_bootloader.bin')

    logging.debug(f"cmake_Gen_image: {cmake_Gen_image_tools}")
    logging.debug(f"cmake_encrypt_crc: {cmake_encrypt_crc_tool}")
    logging.debug(f"bootloader_file: {bootloader_file}")

    if not os.path.exists(cmake_Gen_image_tools) or not os.path.exists(cmake_encrypt_crc_tool):
        logging.error("cmake_Gen_image or cmake_encrypt_crc not found")
        return False

    if not os.path.exists(bootloader_file):
        logging.error("bootloader_file not find")
        return False

    os.chdir(chip_info.output_path)

    # Generate json file
    if chip_info.chip == 't3':
        config_str = t3_config_str
    elif chip_info.chip == 't5':
        config_str = t5_config_str
    else:
        return False

    config_json = json.loads(config_str)
    config_json["section"][0]["firmware"] = bootloader_file
    config_json["section"][1]["firmware"] = chip_info.bin_file
    logging.debug("config_json: " + json.dumps(config_json, indent=4))
    config_file = os.path.join(chip_info.output_path, "config.json")
    with open(config_file, 'w') as f:
        json.dump(config_json, f, indent=4)

    all_app_pack_file = os.path.join(chip_info.output_path, 'all_app_pack.bin')
    gen_image_command = [
        cmake_Gen_image_tools,
        'genfile',
        '-injsonfile',
        config_file,
        '-infile',
        bootloader_file,
        chip_info.bin_file,
        '-outfile',
        all_app_pack_file
    ]
    logging.debug("gen_image_command: " + ' '.join(gen_image_command))
    result = subprocess.run(gen_image_command)
    if result.returncode != 0 or not os.path.exists(all_app_pack_file):
        logging.error("gen_image_command failed")
        return False

    all_app_pack_crc_file = os.path.join(chip_info.output_path, 'all_app_pack_crc.bin')
    cmake_encrypt_crc_command = [
        cmake_encrypt_crc_tool,
        '-crc',
        all_app_pack_file
    ]
    logging.debug("cmake_encrypt_crc_command: " + ' '.join(cmake_encrypt_crc_command))
    result = subprocess.run(cmake_encrypt_crc_command)
    if result.returncode != 0 or not os.path.exists(all_app_pack_crc_file):
        logging.error("cmake_encrypt_crc_command failed")
        return False

    chip_info.bin_file_QIO = os.path.join(chip_info.output_path, f"{chip_info.sketch_name}_QIO_{chip_info.sketch_version}.bin")

    shutil.move(all_app_pack_crc_file, chip_info.bin_file_QIO)

    return True

