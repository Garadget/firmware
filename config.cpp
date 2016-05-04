// $Id$
/**
* @file config.cpp
* @brief Implements garadget configuration related functionality
* @author Denis Grisak
* @version 1.6
*/
// $Log$

#include "config.h"

/** constructor */
c_config::c_config() {
  f_load();
  Particle.variable("doorConfig", s_config, STRING);
}

/**
* Loads saved configuration from EEPROM or defaults
*/
bool c_config::f_load() {

  // read door config from EEPROM
  for (uint8_t n_byte = 0; n_byte < sizeof(configStruct); n_byte++)
    a_config.bytes[n_byte] = EEPROM.read(n_byte);

  // if integrity check failed then load defaults
  if (a_config.values.n_versionMajor != VERSION_MAJOR ||
    a_config.values.n_versionMinor != VERSION_MINOR) {
    f_reset();
    return FALSE;
  }
  f_update();
  return TRUE;
}

/**
* Saves updated configuration variables to EEPROM
*/
int8_t c_config::f_save() {
  uint8_t n_updates = 0;
  for (uint8_t n_byte = 0; n_byte < sizeof(configStruct); n_byte++) {
    if (a_config.bytes[n_byte] != EEPROM.read(n_byte)) {
      EEPROM.write(n_byte, a_config.bytes[n_byte]);
      n_updates++;
    }
  }
  f_update();
  return n_updates;
}

/**
* Loads configuration with default values
*/
int8_t c_config::f_reset() {
  a_config.values.n_versionMajor = VERSION_MAJOR;
  a_config.values.n_versionMinor = VERSION_MINOR;
  a_config.values.n_readTime = DEFAULT_READTIME;
  a_config.values.n_motionTime = DEFAULT_MOTIONTIME;
  a_config.values.n_relayTime = DEFAULT_RELAYTIME;
  a_config.values.n_relayPause = DEFAULT_RELAYPAUSE;
  a_config.values.n_sensorReads = DEFAULT_SENSORREADS;
  a_config.values.n_sensorThreshold = DEFAULT_SENSORTRESHOLD;
  a_config.values.n_alertEvents = DEFAULT_ALERTEVENTS;
  a_config.values.n_alertOpenTimeout = DEFAULT_ALERTOPENTIMEOUT;
  a_config.values.n_alertNightStart = DEFAULT_ALERTNIGHTSTART;
  a_config.values.n_alertNightEnd = DEFAULT_ALERTNIGHTEND;
  a_config.values.n_timeZone = DEFAULT_TIMEZONE;
  a_config.values.n_tzIndex = DEFAULT_TZINDEX;
  return f_save();
}

/**
* Generates the string for door configuration variables
*/
void c_config::f_update() {
  sprintf(
    s_config,
    "ver=%u.%u|rdt=%u|mtt=%u|rlt=%u|rlp=%u|srr=%u|srt=%u|aev=%u|aot=%u|ans=%u|ane=%u|tzo=%.1f%c",
    a_config.values.n_versionMajor,
    a_config.values.n_versionMinor,
    a_config.values.n_readTime,
    a_config.values.n_motionTime,
    a_config.values.n_relayTime,
    a_config.values.n_relayPause,
    a_config.values.n_sensorReads,
    a_config.values.n_sensorThreshold,
    a_config.values.n_alertEvents,
    a_config.values.n_alertOpenTimeout,
    a_config.values.n_alertNightStart,
    a_config.values.n_alertNightEnd,
    a_config.values.n_timeZone,
    a_config.values.n_tzIndex
  );
}

/**
* Parses received configuration string and updates the values
*/
int8_t c_config::f_set(String s_newConfig) {

  if (s_newConfig.equals("defaults")) {
    #ifdef APPDEBUG
      Serial.println("Loading default configuration");
    #endif
    return f_reset();
  }

  int n_start = 0, n_end, n_value;
  String s_command, s_value;

  do {
    n_end = s_newConfig.indexOf('=', n_start);
    if (n_end == -1)
      return -1;
    s_command = s_newConfig.substring(n_start, n_end).c_str();
    n_start = n_end + 1;

    n_end = s_newConfig.indexOf('|', n_start);
    if (n_end == -1) {
      s_value = s_newConfig.substring(n_start).c_str();
    }
    else {
      s_value = s_newConfig.substring(n_start, n_end).c_str();
      n_start = n_end + 1;
    }

    if (s_command.equals("nme")) {
      c_config::f_setName(s_value);
    }
    else if (s_command.equals("rdt")) {
      n_value = s_value.toInt();
      if (n_value < 200 || n_value > 60000)
        n_value = DEFAULT_READTIME;
      a_config.values.n_readTime = n_value;
    }
    else if (s_command.equals("mtt")) {
      n_value = s_value.toInt();
      if (n_value < 500 || n_value > 120000)
        n_value = DEFAULT_MOTIONTIME;
      a_config.values.n_motionTime = n_value;
    }
    else if (s_command.equals("rlt")) {
      n_value = s_value.toInt();
      if (n_value < 10 || n_value > 2000)
        n_value = DEFAULT_RELAYTIME;
      a_config.values.n_relayTime = n_value;
    }
    else if (s_command.equals("rlp")) {
      n_value = s_value.toInt();
      if (n_value < 10 || n_value > 5000)
        n_value = DEFAULT_RELAYPAUSE;
      a_config.values.n_relayPause = n_value;
    }
    else if (s_command.equals("srr")) {
      n_value = s_value.toInt();
      if (n_value < 1 || n_value > 20)
        n_value = DEFAULT_SENSORREADS;
      a_config.values.n_sensorReads = n_value;
    }
    else if (s_command.equals("srt")) {
      n_value = s_value.toInt();
      if (n_value < 1 || n_value > 80)
        n_value = DEFAULT_SENSORTRESHOLD;
      a_config.values.n_sensorThreshold = n_value;
    }
    else if (s_command.equals("aev")) {
      a_config.values.n_alertEvents = s_value.toInt();
    }
    else if (s_command.equals("aot")) {
      n_value = s_value.toInt();
      if (n_value > 43200)
        n_value = DEFAULT_ALERTOPENTIMEOUT;
      a_config.values.n_alertOpenTimeout = n_value;
    }
    else if (s_command.equals("ans")) {
      n_value = s_value.toInt();
      if (n_value >= 1440)
        n_value = DEFAULT_ALERTNIGHTSTART;
      a_config.values.n_alertNightStart = n_value;
    }
    else if (s_command.equals("ane")) {
      n_value = s_value.toInt();
      if (n_value >= 1440)
        n_value = DEFAULT_ALERTNIGHTEND;
      a_config.values.n_alertNightEnd = n_value;
    }
    else if (s_command.equals("tzo")) {
      float n_valueFloat = s_value.toFloat();
      if (n_valueFloat > 13.0)
        n_value = DEFAULT_TIMEZONE;
      a_config.values.n_timeZone = n_valueFloat;
      Time.zone(n_valueFloat);

      char n_valueChar = s_value.charAt(s_value.length() - 1);
      a_config.values.n_tzIndex = (n_valueChar >= 'A' && n_valueChar <= 'z')
        ? n_valueChar
        : DEFAULT_TZINDEX;
    }
  }
  while (n_end != -1);
  return f_save();
}

/**
 * Requests device name from cloud
 */
void c_config::f_requestName() {
  Particle.publish("spark/device/name");
}

/**
* Saves specified string as device name
*/
void c_config::f_setName(String s_name) {
  s_name.replace('_', ' ').toCharArray(s_deviceName, MAXNAMESIZE);
  #ifdef APPDEBUG
    Serial.print("Renamed to ");
    Serial.println(s_deviceName);
  #endif
}
