# Home

[English](README.md) | [简体中文](README_zh.md)

该项目的目的是 Tuya 希望可以借助于 Arduino 生态使更多开发者感受到：原来开发一款物联网设备可以如此简单。项目的名称为 **arduino-tuyaopen** ，这是因为该项目是基于 **[tuya-open-sdk-for-device](https://github.com/tuya/tuya-open-sdk-for-device)** 实现的。目前该项目支持 T2，T3 两款芯片。通过这个文档你可以获取到如何使用该项目的所有信息。

## 支持的芯片

| 芯片 |                          Datasheet                           |
| :--: | :----------------------------------------------------------: |
|  T2  | [T2](https://developer.tuya.com/cn/docs/iot/T2-U-module-datasheet?id=Kce1tncb80ldq) |
|  T3  | [T3](https://developer.tuya.com/cn/docs/iot/T3-U-Module-Datasheet?id=Kdd4pzscwf0il) |

## 支持的操作系统

| 芯片 | Windows  | Linux |  macOS   |
| :--: | :------: | :---: | :------: |
|  T2  |   支持   | 支持  | 暂不支持 |
|  T3  | 暂不支持 | 支持  | 暂不支持 |

> 注意：部分芯片目前不支持某些操作系统，因为缺乏相应的工具。我们将来会提供支持。

点击 [arduino.cc](https://www.arduino.cc/en/software) 下载 Arduino IDE。推荐安装 Arduino IDE 2，该项目的所有编译烧录测试都是在 Arduino IDE 2 上进行的。

## 使用 Arduino IDE 安装 arduino-tuyaopen

+ 复制下方开发板管理地址：

  ```
  https://github.com/tuya/arduino-tuyaopen/releases/download/global/package_tuya_open_index.json
  ```

+ 启动 Arduino IDE 2，并依次点击 “文件” -> “首选项” ，打开首选项窗口。

  ![Preferences](https://images.tuyacn.com/fe-static/docs/img/9f354b85-5f9d-4af6-be60-c114d7b1e822.png)

+ 在 “其他开发板管理器地址” 中输入上述开发板管理地址。

  ![BoardManagerURL](https://images.tuyacn.com/fe-static/docs/img/3e53a3ce-c603-481a-9ac0-fd20d6fa6525.png)

+ 在 “开发板管理器”，输入 "Tuya Open" 查找并安装最新版本。

  ![BoardManager](https://images.tuyacn.com/fe-static/docs/img/120abc55-49dc-4690-8f01-c12e1f45c127.png)

## 开发板硬件简介

+ [T2-U 开发板](https://developer.tuya.com/cn/docs/iot/t2-u-board?id=Kce6cq9e9vlmv)
+ [T3-U 模组规格书](https://developer.tuya.com/cn/docs/iot/T3-U-Module-Datasheet?id=Kdd4pzscwf0il)

## 支持

在使用该项目的过程中，如果您遇到问题或者有新的功能需求或想法，可以在该项目下通过 issues 的方式进行沟通交流。
