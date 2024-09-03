# Home

[English](README.md) | [简体中文](README_zh.md)

The purpose of this project, **arduino-tuyaopen**, is to demonstrate how simple it can be to develop an IoT device utilizing the Arduino ecosystem. This project is based on the **[tuya-open-sdk-for-device](https://github.com/tuya/tuya-open-sdk-for-device)** and currently supports the T2 and T3 chip models. This document provides all the necessary information on how to use this project.

## Supported Chips

| Chip |                          Datasheet                           |
| :--: | :----------------------------------------------------------: |
|  T2  | [T2](https://developer.tuya.com/en/docs/iot/T2-U-module-datasheet?id=Kce1tncb80ldq) |
|  T3  | [T3](https://developer.tuya.com/en/docs/iot/T3-U-Module-Datasheet?id=Kdd4pzscwf0il) |

## Supported Operating Systems

| Chip | Windows  | Linux |  macOS   |
| :--: | :------: | :---: | :------: |
|  T2  | Supported | Supported  | Not Supported |
|  T3  | Not Supported | Supported  | Not Supported |

> Note: Some chips do not currently support certain operating systems due to a lack of necessary tools. Support for these will be provided in the future.

Click [arduino.cc](https://www.arduino.cc/en/software) to download the Arduino IDE. It is recommended to install Arduino IDE 2, as all compilation, burning, and testing for this project were performed on Arduino IDE 2.

## Installing arduino-tuyaopen with Arduino IDE

+ Copy the board manager URL below:

  ```
  https://github.com/tuya/arduino-tuyaopen/releases/download/global/package_tuya_open_index.json
  ```

+ Launch Arduino IDE 2, and sequentially click "File" -> "Preferences" to open the Preferences window.

  ![Preferences](https://images.tuyacn.com/fe-static/docs/img/581335e7-e012-4895-aece-7af21d00bbf5.png)

+ Enter the above board manager URL in "Additional Board Manager URLs".

  ![BoardManagerURL](https://images.tuyacn.com/fe-static/docs/img/cc3f4fa3-3fd6-458a-af90-a04b49225714.png)

+ In the "Board Manager", type "Tuya Open" to search for and install the latest version.

  ![BoardManager](https://images.tuyacn.com/fe-static/docs/img/1aa17f8d-1991-405a-a08e-def5d1db04de.png)

## Introduction to Development Board Hardware

+ [T2-U Development Board](https://developer.tuya.com/en/docs/iot/t2-u-board?id=Kce6cq9e9vlmv)
+ [T3-U Module Specifications](https://developer.tuya.com/en/docs/iot/T3-U-Module-Datasheet?id=Kdd4pzscwf0il)

## Support

If you encounter any issues or have new feature requests or ideas while using this project, you can communicate and exchange ideas through issues in this project repository.
