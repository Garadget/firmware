// $Id$
/**
 * @file node-mqtt.cpp
 * @brief Implements MQTT client
 * @author Denis Grisak
 * @version 1.19
 */
// $Log$

#include "node-mqtt.h"

void c_mqtt::f_onConnectHanlder(uint8_t n_result) {
  if (n_result != CONN_ACCEPT)
    return;

  // (re)subscribe to topics
  const char* s_topicId = f_getClientId();
  char s_topic[sizeof("garadget//command") + MAXNAMESIZE];

  sprintf(
    s_topic,
    "garadget/%s/command",
    s_topicId
  );
  MQTT::f_subscribe(s_topic, QOS0);
  Log.info("MQTT - subscribed to topic: %s", s_topic);

  sprintf(
    s_topic,
    "garadget/%s/config",
    s_topicId
  );
  MQTT::f_subscribe(s_topic, QOS0);
  Log.info("MQTT - subscribed to topic: %s", s_topic);
  f_publishStatus();
  f_publishConfig();
  Log.info("MQTT - ready");
}

void c_mqtt::f_onReceiveHandler(char* s_topic, uint8_t* s_payload, unsigned int n_length) {
  String s_topicString = String(s_topic);
  s_payload[n_length] = '\0';

  Log.info("MQTT - topic: %s, payload: %s", s_topic, s_payload);
  if (s_topicString.endsWith("/command")) {
    if (!strcmp((char*)s_payload, "get-status"))
      f_publishStatus();
    else if (!strcmp((char*)s_payload, "get-config"))
      f_publishConfig();
    else {
      c_doorStatus n_status = c_config::f_statusEnum((char*)s_payload);
      if (n_status == STATUS_UNKNOWN) {
        Log.error("MQTT - unknown command: %s", s_payload);
        return;
      }
      c_message a_message = {
        "mqtt",
        MSG_COMMAND,
        &n_status
      };
      Log.info("MQTT - command: %s", s_payload);
      f_handle(a_message);
    }
  }
  else if (s_topicString.endsWith("/config")) {
    SetConfigCommand o_command = SetConfigCommand();
    o_command.execute((char*)s_payload);
  }
  else if (s_topicString.endsWith("/status")) {
    f_publishStatus();
  }
}

bool c_mqtt::f_init() {

  c_config& o_config = f_getConfig();
  b_enabled = o_config.a_config.n_protocols & 0b10;

  f_disconnect();
  if (!b_enabled) {
    Log.info("MQTT - disabled");
    return FALSE;
  }
  Log.info("MQTT - enabled");

  // initialize client
  MQTT::f_init(
    NULL,
    o_config.a_config.n_mqttBrokerIp,
    o_config.a_config.n_mqttBrokerPort,
    o_config.a_config.n_mqttTimeout,
    MQTT_MAX_PACKET_SIZE
  );
  return TRUE;
}

bool c_mqtt::f_connect() {

  if (!WiFi.ready())
    return FALSE;

  if (f_isConnected())
    return TRUE;

  const char* s_topicId = f_getClientId();
  c_config& o_config = f_getConfig();

  MQTT::f_connect(
    s_topicId,
    o_config.a_config.s_mqttBrokerUser,
    o_config.a_config.s_mqttBrokerPass
  );
  return TRUE;
}

void c_mqtt::f_process() {
  if (!b_enabled || !f_connect()) return;
  MQTT::f_process();
}

/**
 * Control the timers based on simulated status updates
 */
bool c_mqtt::f_receive(const c_message& a_message) {
  switch (a_message.n_type) {

    // configuration update - reinit settings, update config variable
    case MSG_CONFIG:
      f_init();
      break;

    // state change - publish
    case MSG_STATUS:
      f_publishStatus();
      break;

  }
  return TRUE;
}

/**
 * Publishes given state to the cloud
 */
void c_mqtt::f_publishStatus() {
  char s_topic[sizeof("garadget//status") + MAXNAMESIZE];
  sprintf(
    s_topic,
    "garadget/%s/status",
    f_getClientId()
  );
  char s_status[100];
  f_getConfig().f_getJsonStatus(s_status);
  f_publish(s_topic, (uint8_t*)s_status, strlen(s_status), TRUE);
  Log.info("MQTT - published status: %s", s_status);
}

void c_mqtt::f_publishConfig() {
  char s_topic[sizeof("garadget//config") + MAXNAMESIZE];
  sprintf(
    s_topic,
    "garadget/%s/status",
    f_getClientId()
  );
  char s_config[n_maxPacketSize - 6];
  f_getConfig().f_getJsonConfig(s_config);
  f_publish(s_topic, (uint8_t*)s_config, strlen(s_config), TRUE);
  Log.info("MQTT - published config: %s", s_config);
}

const char* c_mqtt::f_getClientId() {
  const char* s_topicId = f_getConfig().a_config.s_deviceName;
  return strlen(s_topicId)
    ? s_topicId
    : System.deviceID().c_str();
}
