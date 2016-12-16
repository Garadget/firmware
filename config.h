// $Id$
/**
 * @file config.h
 * @brief Implements garadget configuration related functionality
 * @author Denis Grisak
 * @version 1.12
 */
// $Log$

#ifndef CONFIG_H
#define CONFIG_H

#include "application.h"
#include "timezones.h"
#include "global.h"

class c_config {

  // this structure must fit in EEPROM so total size must be under 100 bytes
  typedef struct {
    uint8_t n_versionMajor;
    uint8_t n_versionMinor;
    uint16_t n_readTime;
    uint16_t n_motionTime;
    uint16_t n_relayTime;
    uint16_t n_relayPause;
    uint8_t n_sensorReads;
    uint8_t n_sensorThreshold;
    uint16_t n_alertEvents;
    uint16_t n_alertOpenTimeout;
    uint16_t n_alertNightStart;
    uint16_t n_alertNightEnd;
    char s_timeZone[25];
    char s_deviceName[MAXNAMESIZE];
  } configStruct;

  union doorConfig {
    configStruct values;
    uint8_t bytes[sizeof(configStruct)];
  };

protected:
  c_timezones o_timezones;

public:
  char s_config[MAXVARSIZE];
  doorConfig a_config;
  c_config();

/**
 * Parses the provided string and saves values to device's config'
 * @param[in] s_config String to parse and save
 * @return 0 on success and -1 on failure
 */
  int8_t f_set(String s_config);

  /**
   * Requests device name from cloud
   */
  void f_requestName();

  /**
   * Sets the provided string as name of the device
   * @param[in] s_name String name of the device
   */
  void f_setName(String s_name);

protected:
  bool f_load();
  int8_t f_save();
  int8_t f_reset();
  void f_update();
};

#endif
