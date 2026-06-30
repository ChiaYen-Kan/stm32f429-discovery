STM32F429 Discovery USB Demo
=============
porting some usb device driver to STM32F429 base on [felis/USB_Host_Shield_2.0](https://github.com/felis/USB_Host_Shield_2.0)

Development environment
-------------
- eclipse
- eclipse embedded cdt
- arm gnu toolchain
- xpack windows build tools

Directories
-------------
workspce in eclipse

```
├── BSP
├── Base
├── CMSIS
├── HALDriver
├── LCD
├── README.md
├── USBH-HAL-BT-DS4
├── USBH-HAL-BT-SWP
├── USBH-TUSB-BT-DS4
├── USBH-TUSB-BT-SWP
├── USBH-TUSB-DS
├── USBHostLibrary
├── capture
├── pic
└── tinyusb <- please clone TinyUSB to here
```

Projects
-------------
**Base** for test board is work

**LCD** for test lcd is work

**USBH-HAL-BT-DS4** use STM HAL usb driver to test usb bluetooth dongle with Dualshock4 controller

**USBH-HAL-BT-SWP** use STM HAL usb driver to test usb bluetooth dongle with SwitchPro compatible controller

**USBH-TUSB-BT-DS4** use TinyUSB driver to test usb bluetooth dongle with Dualshock4 controller

**USBH-TUSB-BT-SWP** use TinyUSB driver to test usb bluetooth dongle with SwitchPro compatible controller

**USBH-TUSB-DS** use TinyUSB driver to test with Dualshock4/Dualshock3 controller in usb mode

Supported usb bluetooth dongle
-------------
| dongle | VID | PID ||
|-|:-:|:-:|:-------------:|
| CSR 2.0 | 0x0a12 | 0x0001 | <a href="./pic/CSR 2.0.jpg"><img src="./pic/CSR 2.0.jpg" alt="CSR 2.0" width="100"/></a> |
| CSR 4.0 | 0x0a12 | 0x0001 | <a href="./pic/CSR 4.0.jpg"><img src="./pic/CSR 4.0.jpg" alt="CSR 4.0" width="100"/></a> |
| Realtek bluetooth 5.4 | 0x0bda | 0xa728 | <a href="./pic/Realtek bluetooth 5.4.jpg"><img src="./pic/Realtek bluetooth 5.4.jpg" alt="Realtek bluetooth 5.4" width="100"/></a> |
| Realtek bluetooth 6.0 | 0x0bda | 0xa760 | <a href="./pic/Realtek bluetooth 6.0.jpg"><img src="./pic/Realtek bluetooth 6.0.jpg" alt="Realtek bluetooth 6.0" width="100"/></a> |




