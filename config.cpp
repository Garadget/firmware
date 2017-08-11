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
  f_load();
  o_timezones.f_setConfig(String(a_config.s_timeZone));
  Particle.variable("doorConfig", s_config, STRING);
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
  f_update();
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

  a_config.n_mqttOnly = 0;
  a_config.n_mqttBrokerIp[0] = 0;
  a_config.n_mqttBrokerIp[1] = 0;
  a_config.n_mqttBrokerIp[2] = 0;
  a_config.n_mqttBrokerIp[3] = 0;
  a_config.n_mqttBrokerPort = MQTT_PORT;
  a_config.n_mqttTimeout = MQTT_TIMEOUT;

  return f_parse(DEFULT_CONFIG, FALSE) + 5;
}

/**
* Generates the string for door configuration variables
*/
void c_config::f_update() {

  sprintf(
    s_config,
    "sys=%s|ver=%u.%u|rdt=%u|mtt=%u|rlt=%u|rlp=%u|srr=%u|srt=%u|aev=%u|aot=%u|ans=%u|ane=%u|tzo=%s|nme=%s",
    System.version().c_str(),
    a_config.n_versionMajor,
    a_config.n_versionMinor,
    a_config.n_readTime,
    a_config.n_motionTime,
    a_config.n_relayTime,
    a_config.n_relayPause,
    a_config.n_sensorReads,
    a_config.n_sensorThreshold,
    a_config.n_alertEvents,
    a_config.n_alertOpenTimeout,
    a_config.n_alertNightStart,
    a_config.n_alertNightEnd,
    a_config.s_timeZone,
    a_config.s_deviceName
  );
}

/**
* Parses received configuration string and updates the values
*/
int8_t c_config::f_parse(String s_newConfig, bool b_validate = false) {

  if (s_newConfig.equals("defaults")) {
    #ifdef APPDEBUG
      Serial.println("Loading default configuration");
    #endif
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

  if (n_updates) {
    f_update();
    f_save();
  }
  return n_updates;
}

int8_t c_config::f_setValue(String s_param, String s_value) {

  int n_value;

  if (s_param.equals("ver")) {
    return 0;
  }

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

  if (s_param.equals("srr")) {
    n_value = s_value.toInt();
    if (n_value < 1 || n_value > 20)
      return -1;
    if (n_value == a_config.n_sensorReads)
      return 0;
    a_config.n_sensorReads = n_value;
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
    #ifdef APPDEBUG
      Serial.print("Updated Timezone, time now: ");
      Serial.println(Time.timeStr());
    #endif
    s_value.toCharArray(a_config.s_timeZone, 25);
    return 1;
  }

  return -1;
}

void c_config::f_getJsonConfig(char* s_buffer) {

  WiFiAccessPoint a_accessPoints[1];
  int n_accessPoints = WiFi.getCredentials(a_accessPoints, 1);

  byte a_macAddress[6];
  WiFi.macAddress(a_macAddress);

  #ifdef APPDEBUG
  for (int i = 0; i < 6; i++)
    Serial.printf("%02x%s", a_macAddress[i], i != 5 ? ":" : "");
  #endif

  sprintf(
    s_buffer,
    "{\"sys\":\"%s\",\"ver\":\"%u.%u\",\"id\":\"%s\",\"mac\":\"%02X:%02X:%02X:%02X:%02X:%02X\",\"ssid\":\"%s\",\"rdt\":%u,\"mtt\":%u,\"rlt\":%u,\"rlp\":%u,\"srr\":%u,\"srt\":%u,\"nme\":\"%s\",\"mqon\":%u,\"mqip\":\"%u.%u.%u.%u\",\"mqpt\":%u,\"mqto\":%u}",
    System.version().c_str(), // +11b
    a_config.n_versionMajor,
    a_config.n_versionMinor,
    System.deviceID().c_str(), // +32b
    a_macAddress[0],
    a_macAddress[1],
    a_macAddress[2],
    a_macAddress[3],
    a_macAddress[4],
    a_macAddress[5], // + 26b
    n_accessPoints ? a_accessPoints[0].ssid : "", // +41b
    a_config.n_readTime,
    a_config.n_motionTime,
    a_config.n_relayTime,
    a_config.n_relayPause,
    a_config.n_sensorReads,
    a_config.n_sensorThreshold,
    a_config.s_deviceName,
    a_config.n_mqttOnly,
    a_config.n_mqttBrokerIp[0],
    a_config.n_mqttBrokerIp[1],
    a_config.n_mqttBrokerIp[2],
    a_config.n_mqttBrokerIp[3],
    a_config.n_mqttBrokerPort,
    a_config.n_mqttTimeout
  );
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
  s_name.replace('_', ' ').toCharArray(a_config.s_deviceName, MAXNAMESIZE);
  #ifdef APPDEBUG
    Serial.print("Renamed to ");
    Serial.println(s_name);
  #endif
}
