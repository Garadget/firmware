# Firmware
Garadget is the garage door "futurizer". It allows to monitor and operate your existing garage doors from smartphones, wearables and other devices. It's inexpensive, easy to install and developer friendly. For more information please visit http://www.garadget.com

The code in this repository is for the program that controls Garaget's hardware. It's C++ application similar to those created for popular Arduino boards. You are encouraged to study it, report the problems, modify it, try it with your Garadget and share.

## Particle Platform
At the core of the device is Particle Photon P1 module allowing Garadget to benefit from:
- [powerful hardware](https://docs.particle.io/datasheets/p1-datasheet/#features)
- [feature rich firmware libraries](https://docs.particle.io/reference/firmware/photon/)
- LAN and Internet connectivity
- [numerous developer resources](https://docs.particle.io/guide/tools-and-features/dev/)

## Communication Protocol
Garadget communicates with the clients via Particle cloud using 
- [exposing variables](https://docs.particle.io/reference/firmware/photon/#particle-variable-) to report status and configuration
- [accepting function calls](https://docs.particle.io/reference/firmware/photon/#particle-function-) to change status and update configuration
- [publishing events](https://docs.particle.io/reference/firmware/photon/#particle-publish-) to report status changes and send alerts

### Variable: doorStatus
String `status=%s|time=%u%c|sensor=%u|signal=%d` where
- `status` - is current status of the door such as `open`, `closed` etc
- `time` - time in that status 0-119s, 2-119m, 2-47h, 2+d
- `sensor` is the sensor reflection rate 0-100
- `signal` is WiFi signal strength in dB

### Variable: doorConfig
String `ver=%d.%d|cnt=%d|rdt=%d|mtt=%d|mot=%d|rlt=%d|rlp=%d|srr=%d|srt=%d|aot=%d|ans=%d|ane=%d` containing pipe separated value for firmware version and timing parameters (all numeric):
- `ver` - version, dot separated major and minor (currently 1.3)
- `rdt` - sensor scan interval in mS (200-60,000, default 1,000)
- `mtt` - door moving time in mS from completely completely opened to completely closed (1,000 - 120,000, default 10,000)
- `rlt` - button press time mS, time for relay to keep contacts closed (10-2,000, default 300)
- `rlp` - delay between consecutive button presses in mS (10-5,000 default 1,000)
- `srr` - number of sensor reads used in averaging (1-20, default 3)
- `srt` - reflection threshold below which the door is considered open (1-80, default 25)
- `aot` - alert for open timeout in seconds (0 disables, default 320 - 20min)
- `ans` - alert for night time start in minutes from midnight (same value as ane disables, default 1320 - 10pm)
- `ane` - alert for night time end in minutes from midnight (same value as ans disables, default 360 - 6am)

### Variable: netConfig
String `ip=%d.%d.%d.%d|snet=%d.%d.%d.%d|gway=%d.%d.%d.%d|mac=%02X:%02X:%02X:%02X:%02X:%02X|ssid=%s` where
- `ip` - IP address
- `snet` - subnet
- `gway` - gateway
- `mac` - MAC address of the device

### Function: setState
Requests change of door state. As parameter the function receives a string containing a new state such as (`open`, `closed`, `stop`)

### Function: setConfig
Updates configuration parameters. As parameter the function receives a string containing pipe delimited values for `doorConfig` that need to be updated e.g. `srr=5|rlp=%d`. Omitted parameters will remain unchanged, the order of parameters is not important. The length is limited to 63 characters so the request for multiple changes may need to be split into multiple calls.

### Event: state
Published when state of the door changes, parameter is the new state

### Event: config
Published when device configuration is updated, parameter is the number of variables changed

### Event: timeout
Published when door remains open longer than the configured timeout, parameter is the the configured timeout

### Event: night
Published when door remains opens at the beginning of the configured night period or opened during that period. Parameter is the configured time of start of the night period

## Developer Resources
- [Particle Build](https://build.particle.io/build/new) - web based development environment. Edit, compile and upload your new code wirelessly.
- [Particle Dashboard](https://dashboard.particle.io/user/logs) - monitor cloud events
- [Particle Documentation](https://docs.particle.io/reference/firmware/photon/) - Photon documentation
- [Particle Community Forum](https://community.particle.io/)
