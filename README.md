# Home

[English](README.md) | [简体中文](README_zh.md)

# Introduction

arduino-tuyaopen is a open source IoT development framework provided by tuya for the Arduino platform, allowing many Arduino developers to quickly develop IoT devices based on the Tuya cloud platform and realize remote intelligent control of devices.

arduino-tuyaopen is based on [tuya-open-sdk-for-device](https://github.com/tuya/tuya-open-sdk-for-device), which is a cross-chip platform and operating system IoT development framework. It is designed based on the generic Southbound Interface and supports communication protocols such as Bluetooth, Wi-Fi, and Ethernet. It provides the core functions of IoT development, including network configuration, activation, control, and upgrade. It also has strong security compliance capabilities, including device authentication, data encryption, and communication encryption, meeting the data compliance requirements of various countries and regions around the world.

The IoT products developed based on tuya-open-sdk-for-device can use the functions of tuya_cloud_service component to take advantage of the powerful ecosystem provided by TuyaAPP, cloud services, and can interconnect with Power By Tuya devices.

At the same time, tuya-open-sdk-for-device will continue to expand, providing more cloud platform access functions, as well as voice, video, and face recognition functions, and arduino-tuyaopen will also be updated to support more functions.

# Supported chips
Currently, the project supports T2 and T3 two chips
| Chip |                          Datasheet                           |
| :--: | :----------------------------------------------------------: |
|  T2  | [T2](https://developer.tuya.com/cn/docs/iot/T2-U-module-datasheet?id=Kce1tncb80ldq) |
|  T3  | [T3](https://developer.tuya.com/cn/docs/iot/T3-U-Module-Datasheet?id=Kdd4pzscwf0il) |

# Supported operating systems

| Chip | Windows  | Linux |  macOS   |
| :--: | :------: | :---: | :------: |
|  T2  |   Support   | Support  | Temporarily not supported |
|  T3  | Temporarily not supported | Support  | Temporarily not supported |

> Note: Some chips have not been supported on certain operating systems, and we are working hard to support them, please look forward to it!

# arduino-tuyaopen usage

It is recommended to install and use the latest version of Arduino IDE 2, which can be downloaded from the official website of Arduino [arduino.cc](https://www.arduino.cc/en/software). All compilation, burning and testing of this project are carried out on Arduino IDE 2.

+ Copy the following development board management address:

  ```
  https://github.com/tuya/arduino-tuyaopen/releases/download/global/package_tuya_open_index.json
  ```

+ Open Arduino IDE 2 and click "File" -> "Preferences" to open the preferences window.

  ![Preferences](https://images.tuyacn.com/fe-static/docs/img/581335e7-e012-4895-aece-7af21d00bbf5.png)

+ In the "Other Board Manager URLs" field, enter the above development board management address.

  ![BoardManagerURL](https://images.tuyacn.com/fe-static/docs/img/cc3f4fa3-3fd6-458a-af90-a04b49225714.png)

+ In the "Board Manager", search for "Tuya Open" and install the latest version.

  ![BoardManager](https://images.tuyacn.com/fe-static/docs/img/1aa17f8d-1991-405a-a08e-def5d1db04de.png)

## Hardware Introduction for Development Board

+ [T2-U Development Board](https://developer.tuya.com/cn/docs/iot/t2-u-board?id=Kce6cq9e9vlmv)
+ [T3-U Module Datasheet](https://developer.tuya.com/cn/docs/iot/T3-U-Module-Datasheet?id=Kdd4pzscwf0il)

## Support

During the use of this project, if you encounter any problems or have new functional requirements or ideas, you can communicate and exchange ideas through issues under this project.
