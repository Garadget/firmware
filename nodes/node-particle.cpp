// $Id$
/**
 * @file node-particle.cpp
 * @brief Implements Particle cloud interface
 * @author Denis Grisak
 * @version 1.14
 */
// $Log$

#include "node-particle.h"

bool c_particle::f_init() {
  b_enabled = f_getConfig().a_config.n_protocols & 0b01;

  if (!b_enabled) {
    Log.info("Cloud - disabled");
    Particle.disconnect();
    return FALSE;
  }
  Log.info("Cloud - enabled");
  Particle.connect();
  b_init = TRUE;
  return TRUE;
}

/**
 * Waits for the server connection to stabilize
 * then initializes cloud features
 */
void c_particle::f_declare() {
  if (!b_init || !Particle.syncTimeDone())
    return;

  // initial time sync with server
  if (!Time.isValid()) {
    Particle.syncTime();
    return;
  }

  o_updateTimer.f_setDuration(TIMER_VARS);
  f_updateDoorConfig();

  if (b_boot) {
    Particle.function("setState", &c_particle::f_receiveState, this);
    Particle.function("setConfig", &c_particle::f_receiveConfig, this);
    Particle.variable("doorStatus", s_doorStatus, STRING);
    Particle.variable("doorConfig", s_doorConfig, STRING);
    Particle.variable("netConfig", s_netConfig, STRING);
    Particle.subscribe("spark/", &c_particle::f_handleEvent, this);
    Particle.publish("spark/device/name");
    b_boot = FALSE;
    Log.info("Cloud - ready");
  }
  b_init = FALSE;
}

void c_particle::f_process() {
  if (!b_enabled)
    return;
  Particle.process();
  if (!Particle.connected())
    return;

  f_declare();

  if (!o_updateTimer.f_isRunning()) {
    // update particle variables
    f_updateDoorStatus();
    f_updateNetConfig();
    o_updateTimer.f_start();

    // periodically sync time
    if (millis() - Particle.timeSyncedLast() > TIMER_SYNC) {
      Log.info("Cloud - syncing time");
      Particle.syncTime();
    }
  }
}

bool c_particle::f_receive(const c_message& a_message) {
  switch (a_message.n_type) {
    case MSG_INIT:

    // configuration update - reinit settings, update config variable
    case MSG_CONFIG:
      f_init();
      break;

    // state change - publish
    case MSG_STATUS:
      if (b_enabled)
        f_publishStatus(*(c_doorStatus*)a_message.p_payload);
      break;

    case MSG_ALERT:
      if (b_enabled)
        f_publishAlert((char*)a_message.p_payload);
      break;
  }
  return TRUE;
}

/**
 * Particle function "setConfig"
 */
int c_particle::f_receiveConfig(String s_config) {
  Log.info("Cloud - new config: %s", s_config.c_str());
  return f_getConfig().f_parse(s_config, FALSE);
}

/**
 * Particle function "setState"
 */
int c_particle::f_receiveState(String s_state) {

  c_doorStatus n_status = c_config::f_statusEnum(s_state);
  if (n_status == STATUS_UNKNOWN) {
    Log.error("Cloud - unknown command: %s", s_state.c_str());
    return FALSE;
  }

  c_message a_message = {
    s_source,
    MSG_COMMAND,
    &n_status
  };
  Log.info("Cloud - command: %s", s_state.c_str());
  return f_handle(a_message);
}

/**
 * Particle function event subscription
 */
void c_particle::f_handleEvent(const char* s_topic, const char* s_data) {
  Log.info("Cloud - topic: %s, Value: %s", s_topic, s_data);
  if (String(s_topic).equals("spark/device/name")) {
    f_getConfig().f_setName(String(s_data));
    f_publishStatus(STATUS_INIT);
  }
}

/**
 * Publishes given state to the cloud
 */
void c_particle::f_publishStatus(c_doorStatus n_status) {
  String s_status = c_config::f_statusString(n_status);
  Particle.publish("state", s_status, 60, PRIVATE);
  Log.info("Cloud - published status: %s", s_status.c_str());
}

/**
 * Publishes given alert to the cloud
 */
void c_particle::f_publishAlert(char* s_alertData) {
  Particle.publish("alert", s_alertData, 60, PRIVATE);
  Log.info("Cloud - published alert: %s", s_alertData);
}

void c_particle::f_updateDoorStatus() {
  char s_time[10];
  c_config& o_config = f_getConfig();
  o_config.f_timeInStatus(s_time);

  sprintf(
    s_doorStatus,
    "status=%s|time=%s|sensor=%u|base=%u|signal=%d",
    c_config::f_statusString(o_config.a_state.n_doorStatus),
    s_time,
    o_config.a_state.n_sensorReflection,
    o_config.a_state.n_sensorBase,
    WiFi.RSSI()
  );
}

/**
* Generates the string for door configuration variables
*/
void c_particle::f_updateDoorConfig() {
  c_config::c_doorConfig& a_config = f_getConfig().a_config;
  sprintf(
    s_doorConfig,
    "sys=%s|ver=%u.%u|rdt=%u|mtt=%u|rlt=%u|rlp=%u|srr=%u|srt=%u|aev=%u|aot=%u|ans=%u|ane=%u|tzo=%s|nme=%s",
    System.version().c_str(),
    a_config.n_versionMajor,
    a_config.n_versionMinor,
    a_config.n_readTime,
    a_config.n_motionTime,
    a_config.n_relayTime,
    a_config.n_relayPause,
    SENSOR_READS,
    a_config.n_sensorThreshold,
    a_config.n_alertEvents,
    a_config.n_alertOpenTimeout,
    a_config.n_alertNightStart,
    a_config.n_alertNightEnd,
    a_config.s_timeZone,
    a_config.s_deviceName
  );
}

void c_particle::f_updateNetConfig() {

  if (!WiFi.ready())
    return;

  IPAddress a_localIp = WiFi.localIP();
  IPAddress a_netMask = WiFi.subnetMask();
  IPAddress a_gateway = WiFi.gatewayIP();

  byte n_macAddress[6];
  WiFi.macAddress(n_macAddress);

  sprintf(
    s_netConfig,
    "ip=%d.%d.%d.%d|snet=%d.%d.%d.%d|gway=%d.%d.%d.%d|mac=%02X:%02X:%02X:%02X:%02X:%02X|ssid=%s",
    a_localIp[0], // 4+15 bytes
    a_localIp[1],
    a_localIp[2],
    a_localIp[3],
    a_netMask[0], // 6+15 bytes
    a_netMask[1],
    a_netMask[2],
    a_netMask[3],
    a_gateway[0], // 6+15bytes
    a_gateway[1],
    a_gateway[2],
    a_gateway[3],
    n_macAddress[0], // 5+17bytes
    n_macAddress[1],
    n_macAddress[2],
    n_macAddress[3],
    n_macAddress[4],
    n_macAddress[5],
    WiFi.SSID() // 6+32 bytes
  );
}
