#ifndef GLOBAL_H
#define GLOBAL_H

// particle cloud product identification
#define PROD_ID 355

// firmware version for EEPROM data integrity check
#define VERSION_MAJOR 0x01
#define VERSION_MINOR 0x09

// earliest firmware version not requiring the EEPROM reset
#define VERSION_COMPAT 108

// boolean constants
//#define FALSE 0x00
//#define TRUE 0x01

// organization mode or generic Particle device
#define ORGMODE TRUE

// adds debug messages through the serial interface
#define APPDEBUG TRUE

// antenna mode
//#define ANT_MODE ANT_EXTERNAL
#define ANT_MODE ANT_INTERNAL

// switches in simulated door mode
#define APPVIRTUAL FALSE

// maximum payload size for variable according to spark.io documentation
#define MAXVARSIZE 622

// maximum size of device name
#define MAXNAMESIZE 31

// pin assignments
#define PIN_LASER D2
#define PIN_RELAY D3
#define PIN_PHOTO A0

// garadget own address - first
#define PORT_ADDRESS 0x10
// I2C size of the address range starting from own
#define PORT_RANGE 0x10
// I2C message buffer size
#define PORT_BUFFER 0x20

// delay between sensor scans (mS)
// more frequent scans result in faster status update but blinking may be
// irritating to the consumer
#define DEFAULT_READTIME 1000

// expected time for the door to complete full open or close (mS)
// should be set to how long it takes for the door to fully open
#define DEFAULT_MOTIONTIME 10000

// time for garage door button press (mS)
// must be long enough enough for the door opener to register the click
#define DEFAULT_RELAYTIME 300

// minimum time between garage door button presses (mS)
// must be long enough for the door opener to recongnize individual clicks
#define DEFAULT_RELAYPAUSE 1000

// number of sensor reads, results are averaged
// can be increased if status is not reliably determined
#define DEFAULT_SENSORREADS 3

// minimum brightness change that triggers the sensor (0-100%)
// can be adjusted down if target is too far but this can result in false
// positives if objects cross the beam closer to the device
#define DEFAULT_SENSORTRESHOLD 25

// bitmap of event notification switches
// bits corresponding to states declared in enum doorState in the same order
#define DEFAULT_ALERTEVENTS 0x00

// time in seconds for door to remain open before alert is sent
// 0 disables the alert
#define DEFAULT_ALERTOPENTIMEOUT 0

// time in minutes for beginning and end of night alert timeframe
// equal values disable alert
#define DEFAULT_ALERTNIGHTSTART 0
#define DEFAULT_ALERTNIGHTEND 0

// default timezone/DST
#define DEFULT_TZDST "10112-7,20032-6"

#endif
