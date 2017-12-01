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
#include "utils/timezones.h"
#include "nodes/node.h"
#include "global.h"

enum c_doorStatus {
  STATUS_UNKNOWN = 0,
  STATUS_CLOSED,
  STATUS_OPEN,
  STATUS_CLOSING,
  STATUS_OPENING,
  STATUS_STOPPED,
  STATUS_INIT
};

class c_config {

  public:

    // this structure must fit in EEPROM so total size must be under 2047 bytes
    typedef struct {
      uint8_t n_versionMajor;
      uint8_t n_versionMinor;
      uint16_t n_readTime;
      uint16_t n_motionTime;
      uint16_t n_relayTime;
      uint16_t n_relayPause;
      uint8_t n_reserved; // formally sensor reads
      uint8_t n_sensorThreshold;
      uint16_t n_alertEvents;
      uint16_t n_alertOpenTimeout;
      uint16_t n_alertNightStart;
      uint16_t n_alertNightEnd;
      char s_timeZone[25];
      char s_deviceName[MAXNAMESIZE];
      uint8_t n_protocols;
      uint8_t n_mqttBrokerIp[4];
      uint16_t n_mqttBrokerPort;
      uint16_t n_mqttTimeout;
    } c_doorConfig;

    typedef struct {
      c_doorStatus n_doorStatus;
      uint32_t n_lastEvent;
      uint8_t n_sensorReflection;
      uint16_t n_sensorBase;
    } c_deviceState;

    static c_config& f_getInstance();
    c_config(c_config const&);
    void operator=(c_config const&);

    c_doorConfig a_config;
    c_deviceState a_state;
    char s_config[MAXVARSIZE];

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
     * Sets the provided string as name of the device
     * @param[in] s_name String name of the device
     */
    void f_setName(String s_name);

    /**
     * Returns JSON for current config
     * @param[out] s_buffer Buffer for JSON result
     */
    void f_getJsonConfig(char* s_buffer);

    /**
     * Translates door status enum to string
     * @param[in] n_status enum value for the door status
     * @return string containing human readable door status
     */
    static const char* f_statusString(c_doorStatus n_status);

    /**
     * Translates door status string to enum
     * @param[in] n_status string containing human readable door status
     * @return enum value for the door status
     */
    static c_doorStatus f_statusEnum(const String s_status);

    /**
     * Saves updated config to EEPROM
     * @param[in] s_message string containing the event message
     */
    void f_save(const char* s_message = nullptr);

    /**
     * Formats seconds in compact human-readable string
     * @param[in] n_time time in milliseconds
     * @param[out] s_time result string
     */
    static void f_formatTime(uint32_t n_time, char* s_time);

    /**
     * set human-readable string with time since last status change
     * @param[out] s_time result string
     */
    void f_timeInStatus(char* s_time);

  protected:
    c_config();
    bool f_load();
    int8_t f_validate();
    int8_t f_reset();
    c_timezones o_timezones;
};

#endif
