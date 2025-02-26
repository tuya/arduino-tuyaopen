# Tuya IoT 快速上手

涂鸦云应用是涂鸦 IoT 平台提供的一种应用，通过涂鸦云应用，开发者可以快速实现设备远程控制、设备管理等功能。

`quickStart` 演示一个简单的，跨平台、跨系统、支持多种连接的开关示例，通过涂鸦 APP、涂鸦云服务，可以对这个 LED 进行远程控制。

## 产品创建

参考[产品创建文档](https://developer.tuya.com/cn/docs/iot-device-dev/application-creation?id=Kbxw7ket3aujc)，在 [Tuya IoT](https://iot.tuya.com) 平台上创建产品，并获取到创建产品的 PID 。

然后在 `quickStart` 中 PID 替换为您创建得到的 PID。

```c
void setup() {
  ...
  TuyaIoT.begin("YOUR PID", PROJECT_VERSION);
}
```

## 确认 tuyaopen 授权码

通过该项目开发的产品需采用 TuyaOpen 专用授权码，使用其他授权码无法正常连接涂鸦云。

在代码中修改授权码的位置如下：

```c
// Tuya license
#define TUYA_DEVICE_UUID    "uuidxxxxxxxxxxxxxxxx"
#define TUYA_DEVICE_AUTHKEY "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
```

如果没有 tuyaopen 的授权码，可以通过下面的方式进行获取：

+ 购买已烧录 TuyaOpen 授权码模块。该授权码已经在出厂时烧录在对应模组中，且不会丢失。tuyaopen 在启动时通过 `TuyaIoT.readBoardLicense(&license);` 接口读取授权码。请确认当前设备是否为烧录了 tuyaopen 授权码。

+ 如当前模组未烧录 TuyaOpen 授权码，可通过 [TuyaIoT 平台](https://platform.tuya.com/purchase/index?type=6)购买 tuyaopen 授权码。

+ 通过下面的活动获取免费 TuyaOpen 授权码

  为了让开发者们可以自由体验 Tuyaopen Framework，现在只要在 GitHub 上给 Tuyaopen Framework 开发框架仓库加 star，包括：

  + [tuyaopen](https://github.com/tuya/tuyaopen)
  + [arduino-tuyaopen](https://github.com/tuya/arduino-tuyaopen)
  + [luanode-tuyaopen](https://github.com/tuya/luanode-tuyaopen)

  凭 github 账号和截图，发送邮件至 `chenyisong@tuya.com` 或 加入 QQ 群 `796221529` 向群主免费领取一个 TuyaOpen Framework 专用授权码。限量 500 个，先到先得，送完即止，赶紧扫码加群来领👇：

  ![qq_qrcode](https://github.com/tuya/tuyaopen/blob/master/docs/images/zh/qq_qrcode.png?raw=true)
