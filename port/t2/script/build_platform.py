import os
import argparse
import sys
import logging
from SCons.Script import *

#
## log config
#
FORMAT = '[%(levelname)s][%(asctime)s][%(filename)s]%(message)s'
logging.basicConfig(
    level=logging.INFO,
    format=FORMAT,
    datefmt='%H:%M:%S'
)

def checkPath(path):
    """
    Check if the given path exists.

    Args:
        path (str): The path to check.

    Returns:
        bool: True if the path exists, False otherwise.
    """
    if not os.path.exists(path):
        logging.error("Path not exist: " + path)
        return False
    else:
        return True

#
# 参数解析
#
parser = argparse.ArgumentParser(description='Build vendor library')
parser.add_argument('--compiler-path', type=str, required=True, help='Specify the compiler path')
parser.add_argument('--vendor-path', type=str, required=True, help='Specify the vendor path')
parser.add_argument('--flags-path', type=str, required=True, help='Specify the flags path')
parser.add_argument('--open-sdk-path', type=str, required=True, help='Specify the open-sdk path')
parser.add_argument('--tkl-path', type=str, required=True, help='Specify the tkl path')
parser.add_argument('--object-output-dir', type=str, required=True, help='Specify the object output directory')
parser.add_argument('--lib-output-dir', type=str, required=True, help='Specify the library output directory')
parser.add_argument('--lib-output-name', type=str, required=True, help='Specify the library output name')

# 使用SCons自定义参数
if 'CUSTOM_ARGS' in ARGUMENTS:
    args = parser.parse_args(ARGUMENTS['CUSTOM_ARGS'].split())
else:
    logging.error("Error: CUSTOM_ARGS is missing.")
    sys.exit(1)

compilerPath = os.path.join(args.compiler_path, '')
vendorPath = os.path.join(args.vendor_path, '')
flagsPath = os.path.join(args.flags_path, '')
openSdkPath = os.path.join(args.open_sdk_path, '')
tklPath = os.path.join(args.tkl_path, '')

objectOutputDir=args.object_output_dir
libOutputDir=objectOutputDir
openSdkIncludeFlags = os.path.join(flagsPath, "open_sdk_includes.txt")
vendorIncludeFlags = os.path.join(flagsPath, "vendor_includes.txt")
tklIncludeFlags = os.path.join(flagsPath, "port_includes.txt")
outputLibName = args.lib_output_name

# 打印输入参数
logging.debug("compiler path: " + compilerPath)
logging.debug("vendor path: " + vendorPath)
logging.debug("flags path: " + flagsPath)
logging.debug("open-sdk path: " + openSdkPath)
logging.debug("open-sdk port path: " + tklPath)
logging.debug("object output directory: " + objectOutputDir)
logging.debug("library output directory: " + libOutputDir)
logging.debug("openSdkIncludeFlags: " + openSdkIncludeFlags)
logging.debug("vendorIncludeFlags: " + vendorIncludeFlags)
logging.debug("tklIncludeFlags: " + tklIncludeFlags)
logging.debug("outputLibName: " + outputLibName)

# 路径和文件是否存在判断
if checkPath(compilerPath) == False:
    sys.exit(1)
if checkPath(vendorPath) == False:
    sys.exit(1)
if checkPath(flagsPath) == False:
    sys.exit(1)
if checkPath(openSdkPath) == False:
    sys.exit(1)
if checkPath(tklPath) == False:
    sys.exit(1)
if checkPath(objectOutputDir) == False:
    sys.exit(1)
if checkPath(libOutputDir) == False:
    sys.exit(1)
if checkPath(openSdkIncludeFlags) == False:
    sys.exit(1)
if checkPath(vendorIncludeFlags) == False:
    sys.exit(1)
if checkPath(tklIncludeFlags) == False:
    sys.exit(1)

#
# 设置 SCons 的默认参数
#
cross_compiler = "arm-none-eabi-"

def print_cmd_line(s, target, src, env):
    nameStr = os.path.basename(str(target[0]))
    if outputLibName in s :
        logging.info("AR {}...".format(nameStr))
    else :
        logging.info("CC {}...".format(nameStr))

# 
VariantDir(objectOutputDir, vendorPath, duplicate=0)

# 不使用默认环境
DefaultEnvironment(tools=[])

# 创建交叉编译器环境
env = Environment(tools=['gcc', 'ar'])

env['PRINT_CMD_LINE_FUNC'] = print_cmd_line

for (tool, name) in [
    ('CC', 'gcc'),
    ('AR', 'ar')
]:
    if sys.platform == 'win32':
        env[tool] = os.path.join(compilerPath, cross_compiler + name + ".exe")
    elif sys.platform.startswith('linux'):
        env[tool] = os.path.join(compilerPath, cross_compiler + name)
    else:
        logging.error("Error: Unsupported platform.")
        Exit(1)

env['AS'] = env['CC']  # 使用C编译器作为汇编器

env['OBJSUFFIX'] = ".o"

env['LIBPREFIX'] = "lib"
env['LIBSUFFIX'] = ".a"

env.Replace(CCCOM = ['$CC -o $TARGET -c $CFLAGS $CCFLAGS $SOURCES'])
env.Replace(ASCOM = ['$AS $ASFLAGS -o $TARGET $SOURCES'])
env.Replace(ARCOM = ['$AR $ARFLAGS $TARGET $SOURCES'])

# 为`.S`后缀的文件注册汇编构建规则
env['BUILDERS']['Assemble'] = Builder(action='$ASCOM', suffix='.o', src_suffix='.S')

# 创建 vendor 构建环境
cFlags = os.path.join(flagsPath, "c_flags.txt")
vendorFlags = [
    '@' + cFlags,
    '-DWOLFSSL_BEKEN',
    '-iprefix', vendorPath,
    '@' + vendorIncludeFlags,
    '-iprefix', openSdkPath,
    '@' + openSdkIncludeFlags,
    '-iprefix', tklPath,
    '@' + tklIncludeFlags,
]

envVendor = env.Clone()
envVendor.Append(CCFLAGS=vendorFlags)
## vendor .c 文件
vendorSources = [
    os.path.join('app', 'standalone-ap', 'sa_ap.c'),
    os.path.join('app', 'standalone-station', 'sa_station.c'),
    os.path.join('app', 'config', 'param_config.c'),
    os.path.join('app', 'ate_app.c'),
    os.path.join('app', 'app_bk.c'),
    os.path.join('func', 'wlan_ui', 'wlan_cli.c'),
    os.path.join('func', 'wlan_ui', 'wlan_ui.c'),
    os.path.join('func', 'rwnx_intf', 'rw_msg_tx.c'),
    os.path.join('func', 'rwnx_intf', 'rw_ieee80211.c'),
    os.path.join('func', 'rwnx_intf', 'rw_msdu.c'),
    os.path.join('func', 'rwnx_intf', 'rw_msg_rx.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'hostapd', 'main_none.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'wpa_supplicant', 'main_supplicant.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'wpa_supplicant', 'blacklist.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'wpa_supplicant', 'config_none.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'wpa_supplicant', 'bss.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'wpa_supplicant', 'wpa_scan.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'wpa_supplicant', 'events.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'wpa_supplicant', 'op_classes.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'wpa_supplicant', 'wlan.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'wpa_supplicant', 'wpa_supplicant.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'wpa_supplicant', 'ctrl_iface.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'wpa_supplicant', 'wmm_ac.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'wpa_supplicant', 'cmd_wlan.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'wpa_supplicant', 'sme.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'wpa_supplicant', 'notify.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'wpa_supplicant', 'config.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'wpa_supplicant', 'wpas_glue.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'bk_patch', 'signal.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'bk_patch', 'ddrv.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'bk_patch', 'fake_socket.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'bk_patch', 'sk_intf.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'ap', 'wpa_auth.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'ap', 'ap_list.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'ap', 'ieee802_11.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'ap', 'hostapd.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'ap', 'ieee802_11_auth.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'ap', 'ieee802_11_ht.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'ap', 'wmm.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'ap', 'wpa_auth_ie.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'ap', 'wpa_auth_glue.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'ap', 'sta_info.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'ap', 'ieee802_11_shared.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'ap', 'ap_drv_ops.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'ap', 'drv_callbacks.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'ap', 'ap_mlme.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'ap', 'beacon.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'ap', 'hw_features.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'ap', 'tkip_countermeasures.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'ap', 'ap_config.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'ap', 'pmksa_cache_auth.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'ap', 'utils.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'ap', 'ieee802_1x.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'l2_packet', 'l2_packet_none.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'crypto', 'aes-ctr.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'crypto', 'aes-unwrap.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'crypto', 'dh_groups.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'crypto', 'sha1-pbkdf2.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'crypto', 'tls_none.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'crypto', 'sha1-prf.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'crypto', 'crypto_mbedtls-bignum.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'crypto', 'sha256-prf.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'crypto', 'aes-siv.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'crypto', 'sha256.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'crypto', 'crypto_mbedtls-ec.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'crypto', 'crypto_mbedtls.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'crypto', 'aes-omac1.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'crypto', 'rc4.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'crypto', 'dh_group5.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'crypto', 'aes-wrap.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'drivers', 'driver_common.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'drivers', 'drivers.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'drivers', 'driver_beken.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'utils', 'wpabuf.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'utils', 'wpa_debug.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'utils', 'common.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'utils', 'eloop.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'utils', 'os_none.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'rsn_supp', 'wpa_ie.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'rsn_supp', 'pmksa_cache.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'rsn_supp', 'wpa.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'common', 'dragonfly.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'common', 'hw_features_common.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'common', 'sae.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'common', 'notifier.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'common', 'ieee802_11_common.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'common', 'wpa_common.c'),
    os.path.join('func', 'wpa_supplicant-2.9', 'src', 'common', 'wpa_psk_cache.c'),
    os.path.join('func', 'rf_test', 'rx_sensitivity.c'),
    os.path.join('func', 'rf_test', 'tx_evm.c'),
    os.path.join('func', 'sim_uart', 'gpio_uart.c'),
    os.path.join('func', 'sim_uart', 'pwm_uart.c'),
    os.path.join('func', 'libc', 'stdio', 'lib_meminstream.c'),
    os.path.join('func', 'libc', 'stdio', 'lib_memsistream.c'),
    os.path.join('func', 'libc', 'stdio', 'lib_vsscanf.c'),
    os.path.join('func', 'libc', 'stdio', 'lib_libvscanf.c'),
    os.path.join('func', 'libc', 'stdio', 'lib_sscanf.c'),
    os.path.join('func', 'libc', 'stdlib', 'lib_strtod.c'),
    os.path.join('func', 'libc', 'stdlib', 'lib_qsort.c'),
    os.path.join('func', 'joint_up', 'role_launch.c'),
    os.path.join('func', 'misc', 'fake_clock.c'),
    os.path.join('func', 'misc', 'flash_bypass.c'),
    os.path.join('func', 'misc', 'start_type.c'),
    os.path.join('func', 'misc', 'target_util.c'),
    os.path.join('func', 'misc', 'soft_encrypt.c'),
    os.path.join('func', 'net_param_intf', 'net_param.c'),
    os.path.join('func', 'power_save', 'mcu_ps.c'),
    os.path.join('func', 'power_save', 'power_save.c'),
    os.path.join('func', 'power_save', 'manual_ps.c'),
    os.path.join('func', 'bk7011_cal', 'bk7231N_cal.c'),
    os.path.join('func', 'bk7011_cal', 'bk7231U_cal.c'),
    os.path.join('func', 'bk7011_cal', 'manual_cal_bk7231U.c'),
    os.path.join('func', 'temp_detect', 'temp_detect.c'),
    os.path.join('func', 'func.c'),
    os.path.join('func', 'spidma_intf', 'spidma_intf.c'),
    os.path.join('func', 'uart_debug', 'cmd_help.c'),
    os.path.join('func', 'uart_debug', 'command_line.c'),
    os.path.join('func', 'uart_debug', 'cmd_evm.c'),
    os.path.join('func', 'uart_debug', 'cmd_rx_sensitivity.c'),
    os.path.join('func', 'uart_debug', 'udebug.c'),
    os.path.join('func', 'uart_debug', 'command_table.c'),
    os.path.join('func', 'uart_debug', 'cmd_reg.c'),
    os.path.join('func', 'user_driver', 'BkDriverPwm.c'),
    os.path.join('func', 'user_driver', 'BkDriverGpio.c'),
    os.path.join('func', 'user_driver', 'BkDriverTimer.c'),
    os.path.join('func', 'user_driver', 'BkDriverWdg.c'),
    os.path.join('func', 'user_driver', 'BkDriverUart.c'),
    os.path.join('func', 'user_driver', 'BkDriverFlash.c'),
    os.path.join('func', 'user_driver', 'BkDriverRng.c'),
    os.path.join('func', 'lwip_intf', 'dhcpd', 'dhcp-server.c'),
    os.path.join('func', 'lwip_intf', 'dhcpd', 'dhcp-server-main.c'),
    os.path.join('func', 'hostapd_intf', 'hostapd_intf.c'),
    os.path.join('os', 'mem_arch.c'),
    os.path.join('os', 'str_arch.c'),
    os.path.join('os', 'platform_stub.c'),
    os.path.join('driver', 'driver.c'),
    os.path.join('driver', 'dma', 'dma.c'),
    os.path.join('driver', 'spidma', 'spidma.c'),
    os.path.join('driver', 'saradc', 'saradc.c'),
    os.path.join('driver', 'rw_pub', 'rw_platf_pub.c'),
    os.path.join('driver', 'ble_5_x_rw', 'platform', '7231n', 'entry', 'ble_main.c'),
    os.path.join('driver', 'ble_5_x_rw', 'platform', '7231n', 'rwip', 'src', 'rwble.c'),
    os.path.join('driver', 'ble_5_x_rw', 'platform', '7231n', 'rwip', 'src', 'rwip.c'),
    os.path.join('driver', 'ble_5_x_rw', 'platform', '7231n', 'driver', 'rf', 'ble_rf_port.c'),
    os.path.join('driver', 'ble_5_x_rw', 'platform', '7231n', 'driver', 'rf', 'rf_xvr.c'),
    os.path.join('driver', 'ble_5_x_rw', 'platform', '7231n', 'driver', 'uart', 'uart_ble.c'),
    os.path.join('driver', 'ble_5_x_rw', 'ble_lib', 'ip', 'hci', 'src', 'hci.c'),
    os.path.join('driver', 'sys_ctrl', 'sys_ctrl.c'),
    os.path.join('driver', 'icu', 'icu.c'),
    os.path.join('driver', 'gpio', 'gpio.c'),
    os.path.join('driver', 'entry', 'arch_main.c'),
    os.path.join('driver', 'macphy_bypass', 'mac_phy_bypass.c'),
    os.path.join('driver', 'spi', 'spi_master_bk7231n_new.c'),
    os.path.join('driver', 'spi', 'spi_bk7231n.c'),
    os.path.join('driver', 'spi', 'spi_slave_bk7231n.c'),
    os.path.join('driver', 'common', 'dd.c'),
    os.path.join('driver', 'common', 'drv_model.c'),
    os.path.join('driver', 'phy', 'phy_trident.c'),
    os.path.join('driver', 'flash', 'flash.c'),
    os.path.join('driver', 'intc', 'intc.c'),
    os.path.join('driver', 'wdt', 'wdt.c'),
    os.path.join('driver', 'pwm', 'pwm_new.c'),
    os.path.join('driver', 'pwm', 'bk_timer.c'),
    os.path.join('driver', 'pwm', 'pwm.c'),
    os.path.join('driver', 'pwm', 'mcu_ps_timer.c'),
    os.path.join('driver', 'uart', 'uart_bk.c'),
    os.path.join('driver', 'uart', 'printf.c'),
    os.path.join('driver', 'uart', 'Retarget.c'),
    os.path.join('driver', 'irda', 'irda.c'),
    os.path.join('driver', 'general_dma', 'general_dma.c'),
    os.path.join('demo', 'ieee802_11_demo.c')
]

vendorSourcesAbsolutePath = [os.path.join(objectOutputDir, src) for src in vendorSources]
objectsVendor = envVendor.Object(vendorSourcesAbsolutePath)

#
# 创建 vendor FreeRTOS 构建环境
#
vendorOSFlags = [
    '-g',
    '-marm',
    '-mcpu=arm968e-s',
    '-march=armv5te',
    '-mthumb-interwork',
    '-mlittle-endian',
    '-Os',
    '-std=c99',
    '-ffunction-sections',
    '-Wall',
    '-fsigned-char',
    '-fdata-sections',
    '-Wunknown-pragmas',
    '-iprefix', vendorPath,
    '@' + vendorIncludeFlags,
    '-iprefix', openSdkPath,
    '@' + openSdkIncludeFlags,
    '-iprefix', tklPath,
    '@' + tklIncludeFlags,
]
envVendorOS = env.Clone()
envVendorOS['OBJSUFFIX'] = ".marm.o"
envVendorOS.Append(CCFLAGS=vendorOSFlags)

## vendor OS .c 文件
vendorOSSources = [
    os.path.join('os', 'FreeRTOSv9.0.0', 'FreeRTOS', 'Source', 'croutine.c'),
    os.path.join('os', 'FreeRTOSv9.0.0', 'FreeRTOS', 'Source', 'list.c'),
    os.path.join('os', 'FreeRTOSv9.0.0', 'FreeRTOS', 'Source', 'event_groups.c'),
    os.path.join('os', 'FreeRTOSv9.0.0', 'FreeRTOS', 'Source', 'timers.c'),
    os.path.join('os', 'FreeRTOSv9.0.0', 'FreeRTOS', 'Source', 'queue.c'),
    os.path.join('os', 'FreeRTOSv9.0.0', 'FreeRTOS', 'Source', 'portable', 'MemMang', 'heap_6.c'),
    os.path.join('os', 'FreeRTOSv9.0.0', 'FreeRTOS', 'Source', 'portable', 'Keil', 'ARM968es', 'port.c'),
    os.path.join('os', 'FreeRTOSv9.0.0', 'FreeRTOS', 'Source', 'tasks.c'),
    os.path.join('os', 'FreeRTOSv9.0.0', 'rtos_pub.c'),
]

vendorOSSourcesAbsolutePath = [os.path.join(objectOutputDir, src) for src in vendorOSSources]
objectsVendorOS = envVendorOS.Object(vendorOSSourcesAbsolutePath)

#
# 创建 ASM 构建环境
#
vendorASMFlags = [
    '-c',
    '-g',
    '-marm',
    '-mthumb-interwork',
    '-mcpu=arm968e-s',
    '-march=armv5te',
    '-x',
    'assembler-with-cpp',
    '-iprefix', vendorPath,
    '@' + vendorIncludeFlags,
]
envVendorAMS = env.Clone()
envVendorAMS['OBJSUFFIX'] = ".O"
envVendorAMS.Append(ASFLAGS=vendorASMFlags)
## vendor .S 文件
vendorAMSSources = [
    os.path.join('driver', 'entry', 'boot_handlers.S'),
    os.path.join('driver', 'entry', 'boot_vectors.S')
]

vendorAMSSourcesAbsolutePath = [os.path.join(objectOutputDir, src) for src in vendorAMSSources]

objectsVendorAMS = []
for i in range(len(vendorAMSSourcesAbsolutePath)):
    objectsVendorAMS.append(envVendorAMS.Assemble(vendorAMSSourcesAbsolutePath[i]))

allObject = objectsVendor + objectsVendorOS + objectsVendorAMS

# 默认目标
Default(allObject)

# 添加后置动作
env.AddPostAction(allObject, [env['AR'] + ' rcs ' + os.path.join(libOutputDir, outputLibName) + ' $TARGET'])
