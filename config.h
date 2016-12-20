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

  // this structure must fit in EEPROM so total size must be under 2047 bytes
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
  } doorConfig;

protected:
  c_timezones o_timezones;

public:
  doorConfig a_config;
  char s_config[MAXVARSIZE];
  c_config();

/**
 * Parses the provided string and saves values to device's config'
 * @param[in] s_config String to parse and save
 * @param[in] b_validate ignores out of range values if false, resets config to defaults if true
 * @return number of updated parameters
 */
  int8_t f_parse(String s_config, bool b_validate);

  /**
   * Sets an individual configuration parameter
   * @param[in] s_param Name of the parameter
   * @param[in] s_value New value for the parameter
   * @return negative number on error, zero on no change and positive on success
   */
  int8_t f_setValue(String s_param, String s_value);

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
  void f_save();
  int8_t f_validate();
  int8_t f_reset();
  void f_update();
};

#endif
