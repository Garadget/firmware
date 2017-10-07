// $Id$
/**
* @file config.cpp
* @brief Implements garadget configuration related functionality
* @author Denis Grisak
* @version 1.12
*/
// $Log$

#include "config.h"

c_config& c_config::f_getInstance() {
  static c_config o_config;
  return o_config;
}

/** constructor */
c_config::c_config() {
  a_state.n_lastEvent = millis();
  f_load();
  o_timezones.f_setConfig(String(a_config.s_timeZone));
}

/**
* Loads saved configuration from EEPROM or defaults
*/
bool c_config::f_load() {

  // read door config from EEPROM
  EEPROM.get(0, a_config);
  f_validate();

  // if integrity check failed then load defaults
  uint16_t n_savedVersionId = a_config.n_versionMajor * 100 + a_config.n_versionMinor;
  if (n_savedVersionId != VERSION_ID) {
    if (n_savedVersionId < VERSION_COMPAT || n_savedVersionId > VERSION_ID) {
      f_reset();
      return FALSE;
    }
    a_config.n_versionMajor = VERSION_MAJOR;
    a_config.n_versionMinor = VERSION_MINOR;
    f_save();
  }
  return TRUE;
}

/**
* validates the current configuration against the boundaries
*/
int8_t c_config::f_validate() {
  return f_parse(String(s_config), TRUE);
}

/**
* Saves updated configuration variables to EEPROM
*/
void c_config::f_save() {
  EEPROM.put(0, a_config);
}

/**
* Loads configuration with default values
*/
int8_t c_config::f_reset() {
  a_config.n_versionMajor = VERSION_MAJOR;
  a_config.n_versionMinor = VERSION_MINOR;
  return f_parse(DEFULT_CONFIG, FALSE) + 5;
}

/**
* Parses received configuration string and updates the values
*/
int8_t c_config::f_parse(String s_newConfig, bool b_validate = false) {

  if (s_newConfig.equals("defaults")) {
    Log.info("Config - loading defaults");
    return f_reset();
  }

  int n_updates = 0, n_result, n_start = 0, n_end;
  String s_param, s_value;

  do {
    n_end = s_newConfig.indexOf('=', n_start);
    if (n_end == -1)
      return -1;
    s_param = s_newConfig.substring(n_start, n_end).c_str();
    n_start = n_end + 1;

    n_end = s_newConfig.indexOf('|', n_start);
    if (n_end == -1) {
      s_value = s_newConfig.substring(n_start).c_str();
    }
    else {
      s_value = s_newConfig.substring(n_start, n_end).c_str();
      n_start = n_end + 1;
    }

    n_result = f_setValue(s_param, s_value);
    if (b_validate && n_result < 0)
      return f_reset();

    if (n_result > 0)
      n_updates++;
  }
  while (n_end != -1);

  if (n_updates)
    f_save();
  return n_updates;
}

int8_t c_config::f_setValue(String s_param, String s_value) {

  int n_value;

  if (s_param.equals("ver"))
    return 0;

  if (s_param.equals("nme")) {
    if (s_value.equals(a_config.s_deviceName))
      return 0;
    c_config::f_setName(s_value);
    return 1;
  }

  if (s_param.equals("rdt")) {
    n_value = s_value.toInt();
    if (n_value < 200 || n_value > 60000)
      return -1;
    if (n_value == a_config.n_readTime)
      return 0;
    a_config.n_readTime = n_value;
    return 1;
  }

  if (s_param.equals("mtt")) {
    n_value = s_value.toInt();
    if (n_value < 5000 || n_value > 60000)
      return -1;
    if (n_value == a_config.n_motionTime)
      return 0;
    a_config.n_motionTime = n_value;
    return 1;
  }

  if (s_param.equals("rlt")) {
    n_value = s_value.toInt();
    if (n_value < 10 || n_value > 2000)
      return -1;
    if (n_value == a_config.n_relayTime)
      return 0;
    a_config.n_relayTime = n_value;
    return 1;
  }

  if (s_param.equals("rlp")) {
    n_value = s_value.toInt();
    if (n_value < 10 || n_value > 5000)
      return -1;
    if (n_value == a_config.n_relayPause)
      return 0;
    a_config.n_relayPause = n_value;
    return 1;
  }

  if (s_param.equals("srt")) {
    n_value = s_value.toInt();
    if (n_value < 1 || n_value > 80)
      return -1;
    if (n_value == a_config.n_sensorThreshold)
      return 0;
    a_config.n_sensorThreshold = n_value;
    return 1;
  }

  if (s_param.equals("aev")) {
    n_value = s_value.toInt();
    if (n_value > 0b111111111)
      return -1;
    if (n_value == a_config.n_alertEvents)
      return 0;
    a_config.n_alertEvents = n_value;
    return 1;
  }

  if (s_param.equals("aot")) {
    n_value = s_value.toInt();
    if (n_value > 43200)
      return -1;
    if (n_value == a_config.n_alertOpenTimeout)
      return 0;

    a_config.n_alertOpenTimeout = n_value;
    return 1;
  }

  if (s_param.equals("ans")) {
    n_value = s_value.toInt();
    if (n_value >= 1440)
      return -1;
    if (n_value == a_config.n_alertNightStart)
      return 0;
    a_config.n_alertNightStart = n_value;
    return 1;
  }

  if (s_param.equals("ane")) {
    n_value = s_value.toInt();
    if (n_value >= 1440)
      return -1;
    if (n_value == a_config.n_alertNightEnd)
      return 0;
    a_config.n_alertNightEnd = n_value;
    return 1;
  }

  if (s_param.equals("tzo")) {
    if (!o_timezones.f_setConfig(s_value))
      return -1;

    if (s_value.equals(a_config.s_timeZone))
      return 0;
    Log.info("Config - updated Timezone, time now: %s", Time.timeStr().c_str());
    s_value.toCharArray(a_config.s_timeZone, 25);
    return 1;
  }

  if (s_param.equals("mqtt")) {
    n_value = s_value.toInt();
    if (n_value > 0b011)
      return -1;
    if (n_value == a_config.n_protocols)
      return 0;
    a_config.n_protocols = n_value;
    return 1;
  }

  if (s_param.equals("mqip")) {
    uint8_t n_start = 0, n_end, n_return = 0;
    for (uint8_t n_octet = 0; n_octet < 4; n_octet++) {
      n_end = s_value.indexOf('.', n_start);
      n_value = s_value.substring(n_start, n_end).toInt();
      if (!s_value.length() || n_value > 0xFF)
        return -1;
      if (n_value != a_config.n_mqttBrokerIp[n_octet])
        n_return = 1;
      a_config.n_mqttBrokerIp[n_octet] = n_value;
      n_start = n_end + 1;
    }
    return n_return;
  }

  if (s_param.equals("mqpt")) {
    n_value = s_value.toInt();
    if (!n_value || n_value > 0xFFFF)
      return -1;
    if (n_value == a_config.n_mqttBrokerPort)
      return 0;
    a_config.n_mqttBrokerPort = n_value;
    return 1;
  }

  if (s_param.equals("mqto")) {
    n_value = s_value.toInt();
    if (n_value > 0xFFFF)
      return -1;
    if (n_value == a_config.n_mqttTimeout)
      return 0;
    a_config.n_mqttTimeout = n_value;
    return 1;
  }
  return -1;
}

void c_config::f_getJsonConfig(char* s_buffer) {

  WiFiAccessPoint a_accessPoints[1];
  int n_accessPoints = WiFi.getCredentials(a_accessPoints, 1);

//  byte a_macAddress[6];
//  WiFi.macAddress(a_macAddress);

  sprintf(
    s_buffer,
    "{\"sys\":\"%s\",\"ver\":\"%u.%u\",\"id\":\"%s\",\"ssid\":\"%s\",\"rdt\":%u,\"mtt\":%u,\"rlt\":%u,\"rlp\":%u,\"srt\":%u,\"nme\":\"%s\",\"mqtt\":%u,\"mqip\":\"%u.%u.%u.%u\",\"mqpt\":%u,\"mqto\":%u}",
    System.version().c_str(), // +11b
    a_config.n_versionMajor,
    a_config.n_versionMinor,
    System.deviceID().c_str(), // +32b
    n_accessPoints ? a_accessPoints[0].ssid : "", // +41b
    a_config.n_readTime,
    a_config.n_motionTime,
    a_config.n_relayTime,
    a_config.n_relayPause,
    a_config.n_sensorThreshold,
    a_config.s_deviceName,
    a_config.n_protocols,
    a_config.n_mqttBrokerIp[0],
    a_config.n_mqttBrokerIp[1],
    a_config.n_mqttBrokerIp[2],
    a_config.n_mqttBrokerIp[3],
    a_config.n_mqttBrokerPort,
    a_config.n_mqttTimeout
  );
}

/**
* Saves specified string as device name
*/
void c_config::f_setName(String s_name) {
  s_name.replace('_', ' ').toCharArray(a_config.s_deviceName, MAXNAMESIZE);
  Log.info("Config - name changed to %s", s_name.c_str());
}

/**
 * Translates enum state to string
 */
const char* c_config::f_statusString(c_doorStatus n_status) {
  switch (n_status) {
    case STATUS_CLOSED:
      return "closed";
    case STATUS_OPEN:
      return "open";
    case STATUS_CLOSING:
      return "closing";
    case STATUS_OPENING:
      return "opening";
    case STATUS_STOPPED:
      return "stopped";
    case STATUS_INIT:
      return "initialized";
    default:
      return "unknown";
  }
}

/**
 * Translates string state to enum
 */
c_doorStatus c_config::f_statusEnum(String s_status) {
  if (s_status.startsWith("close"))
    return STATUS_CLOSED;
  if (s_status.equals("open"))
    return STATUS_OPEN;
  if (s_status.equals("closing"))
    return STATUS_CLOSING;
  if (s_status.equals("opening"))
    return STATUS_OPENING;
  if (s_status.startsWith("stop"))
    return STATUS_STOPPED;
  return STATUS_UNKNOWN;
}

/**
 * Formats seconds in compact human-readable string
 */
void c_config::f_formatTime(uint32_t n_time, char* s_time) {
  n_time /= 1000;
  char s_units = 's';
  if (n_time >= 120) {
    s_units = 'm';
    n_time /= 60;
    if (n_time >= 120) {
      s_units = 'h';
      n_time /= 60;
      if (n_time >= 48) {
        s_units = 'd';
        n_time /= 24;
      }
    }
  }
  sprintf(s_time, "%lu%c", n_time, s_units);
}

/**
 * set human-readable string with time since last status change
 */
void c_config::f_timeInStatus(char* s_time) {
  f_formatTime(millis() - a_state.n_lastEvent, s_time);
}
