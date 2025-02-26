# Tuya IoT Quick Start

Tuya Cloud Application is an application provided by the Tuya IoT platform. With Tuya Cloud Application, developers can quickly implement remote device control, device management, and other functions.

The `quickStart` demonstrates a simple, cross-platform, cross-system switch example that supports multiple connections. Through the Tuya APP and Tuya Cloud Service, you can remotely control this LED.

## Product Creation

Refer to the [Product Creation Documentation](https://developer.tuya.com/en/docs/iot-device-dev/application-creation?id=Kbxw7ket3aujc) to create a product on the [Tuya IoT](https://iot.tuya.com) platform and obtain the PID of the created product.

Then replace the PID in `quickStart` with the PID you obtained.

```c
void setup() {
  ...
  TuyaIoT.begin("YOUR PID", PROJECT_VERSION);
}
```

## Confirm TuyaOpen License Code

Products developed through this project need to use the TuyaOpen-specific license code. Using other license codes will not connect to the Tuya Cloud properly.

Modify the license code in the code as follows:

```c
// Tuya license
#define TUYA_DEVICE_UUID    "uuidxxxxxxxxxxxxxxxx"
#define TUYA_DEVICE_AUTHKEY "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
```

If you do not have a TuyaOpen license code, you can obtain it through the following methods:

+ Purchase a module with the TuyaOpen license code pre-burned. This license code is burned into the corresponding module at the factory and will not be lost. TuyaOpen reads the license code through the `TuyaIoT.readBoardLicense(&license);` interface at startup. Please confirm whether the current device has the TuyaOpen license code burned.

+ If the current module does not have the TuyaOpen license code burned, you can purchase the TuyaOpen license code through the [TuyaIoT platform](https://platform.tuya.com/purchase/index?type=6).

+ Obtain a free TuyaOpen license code through the following activities:

  To allow developers to freely experience the TuyaOpen Framework, you can now get a free TuyaOpen Framework-specific license code by starring the TuyaOpen Framework repository on GitHub, including:

  + [tuyaopen](https://github.com/tuya/tuyaopen)
  + [arduino-tuyaopen](https://github.com/tuya/arduino-tuyaopen)
  + [luanode-tuyaopen](https://github.com/tuya/luanode-tuyaopen)

  With your GitHub account and screenshot, send an email to `chenyisong@tuya.com` or join the QQ group `796221529` and contact the group owner to receive a free TuyaOpen Framework-specific license code. Limited to 500 codes, first come first served, while supplies last. Scan the QR code below to join the group and claim your code👇:

  ![qq_qrcode](https://github.com/tuya/tuyaopen/blob/master/docs/images/zh/qq_qrcode.png?raw=true)
