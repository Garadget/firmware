#ifndef GLOBAL_H
#define GLOBAL_H

// particle cloud product identification
#define PROD_ID 355

// firmware version for EEPROM data integrity check
#define VERSION_MAJOR 1
#define VERSION_MINOR 14
#define VERSION_ID VERSION_MAJOR * 100 + VERSION_MINOR

// earliest firmware version not requiring the EEPROM reset
#define VERSION_COMPAT 108

// boolean constants
//#define FALSE 0x00
//#define TRUE 0x01

// organization mode or generic Particle device
#define ORGMODE TRUE

// adds debug messages through the serial interface
// #define APPDEBUG TRUE

// antenna mode
// #define ANT_MODE ANT_EXTERNAL
// #define ANT_MODE ANT_INTERNAL

// switches in simulated door mode
#define APPVIRTUAL FALSE

// maximum payload size for variable according to spark.io documentation
#define MAXVARSIZE 622

// maximum size of device name
#define MAXNAMESIZE 31

// pin assignments
#define PIN_SENSOR D0
#define PIN_RELAY D3

// reported reflection levels on sensor closed and open
#define SWITCH_REFLCLSD 100
#define SWITCH_REFLOPEN 0

#define DEFULT_CONFIG "rdt=1000|mtt=10000|rlt=300|rlp=1000|srr=3|srt=15|aev=0|aot=0|ans=0|ane=0|tzo=10112-7,20032-6|nme=Garage"


#endif
