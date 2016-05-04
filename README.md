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
Garadget communicates with the clients via Particle cloud by
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
String `ver=%d.%d|cnt=%d|rdt=%d|mtt=%d|mot=%d|rlt=%d|rlp=%d|srr=%d|srt=%d|aev=%d|aot=%d|ans=%d|ane=%d` containing pipe separated value for firmware version and timing parameters (all numeric):
- `ver` - version, dot separated major and minor (currently 1.6)
- `rdt` - sensor scan interval in mS (200-60,000, default 1,000)
- `mtt` - door moving time in mS from completely completely opened to completely closed (1,000 - 120,000, default 10,000)
- `rlt` - button press time mS, time for relay to keep contacts closed (10-2,000, default 300)
- `rlp` - delay between consecutive button presses in mS (10-5,000 default 1,000)
- `srr` - number of sensor reads used in averaging (1-20, default 3)
- `srt` - reflection threshold below which the door is considered open (1-80, default 25)
- `aev` - number serving as bitmap for enabling status alerts (starting from LSB: closed, open, closing, opening, stopped, init, online, offline, config)
- `aot` - alert for open timeout in seconds (0 disables, default 1,200 - 20min)
- `ans` - alert for night time start in minutes from midnight (same value as ane disables, default 1320 - 10pm)
- `ane` - alert for night time end in minutes from midnight (same value as ans disables, default 360 - 6am)
- `tzo` - float number for hours offset from GMT and optional character to resolve ambiguity where multiple locations are in the same offset. Full list is available in alerts configuration page of web interface


### Variable: netConfig
String `ip=%d.%d.%d.%d|snet=%d.%d.%d.%d|gway=%d.%d.%d.%d|mac=%02X:%02X:%02X:%02X:%02X:%02X|ssid=%s` where
- `ip` - IP address
- `snet` - subnet
- `gway` - gateway
- `mac` - MAC address of the device

### Function: setState
Requests change of door state. As parameter the function receives a string containing a new state such as (`open`, `closed`, `stop`)

### Function: setConfig
Updates doorConfig parameters as listed above. As parameter the function receives a string containing pipe delimited values for `doorConfig` that need to be updated e.g. `srr=5|rlp=%d`. Omitted parameters will remain unchanged, the order of parameters is not important. The length is limited to 63 characters so the request for multiple changes may need to be split into multiple calls. Additional write only `nme` parameter is available to notify the unit about the name change.

### Event: state
Published when state of the door changes, parameter is the new state. This event can be used to update the UI.

### Event: alert
Published when alert is generated or device configuration changes. Parameter is JSON with the type of alert and related data. Multiple alert types are supported:

#### state
Published when state of the door or device changes and alert for the new state is enabled in configuration (`aev` setting).
Example JSON: `{"name": "Home", "type": "state", data: "opening"}`

#### timeout
Published when door remains open longer than the configured timeout, parameter is the configured timeout (`aot` setting).
Example JSON: `{"name": "Home", "type": "timeout", "data": "20m"}`

#### night
Published when door remains opens at the beginning of the configured night period or opened during that period. Parameter is the configured time of start of the night period. Parameter is configured time range (`ans` and `ane` settings).
Example JSON: `{"name": "Home", "type": "night", "data": "1320-360"}`

#### config
Published when device configuration is updated, parameter is the updated configuration string.
Example JSON: `{"name": "Home", "type": "config", "data": "ans=1320|ane=360|tzo=-7.0"}`

## Developer Resources
- [Garadget Community Forum](http://community.garadget.com/) - post your questions, ideas and requests.
- [Particle Build](https://build.particle.io/build/new) - web based development environment. Edit, compile and upload your new code wirelessly.
- [Particle Dashboard](https://dashboard.particle.io/user/logs) - monitor cloud events
- [Particle Documentation](https://docs.particle.io/reference/firmware/photon/) - Photon documentation
- [Particle Community Forum](https://community.particle.io/)
