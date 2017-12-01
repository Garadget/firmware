// $Id$
/**
 * @file node-mqtt.cpp
 * @brief Implements MQTT client
 * @author Denis Grisak
 * @version 1.14
 */
// $Log$

#include "node-mqtt.h"

MQTT* c_mqtt::o_client = NULL;

void c_mqtt::f_callback(char* s_topic, byte* s_payload, unsigned int n_length) {
  Log.info("MQTT - topic: %s, payload: %s", s_topic, s_payload);
  // @todo: handle the commands
}

bool c_mqtt::f_init() {

  c_config& o_config = f_getConfig();
  delete o_client;

  b_enabled = o_config.a_config.n_protocols & 0b10;
  if (!b_enabled) {
    Log.info("MQTT - disabled");
    return FALSE;
  }
  Log.info("MQTT - enabled");

  // initialize client
  o_client = new MQTT(
    o_config.a_config.n_mqttBrokerIp,
    o_config.a_config.n_mqttBrokerPort,
    o_config.a_config.n_mqttTimeout,
    c_mqtt::f_callback
  );
  WiFi.connect();

  // connect to the server
  return f_connect();
}

bool c_mqtt::f_connect() {

  if (!WiFi.ready())

  if (o_client->isConnected())
    return TRUE;

  if ((millis() - n_lastConnect < N_CONNECT_PERIOD) && n_lastConnect)
    return FALSE;

  n_lastConnect = millis();
  const char* s_deviceId = System.deviceID().c_str();
  o_client->connect(s_deviceId);
  if (!o_client->isConnected())
    return FALSE;

  // (re)subscribe to topics
  char s_topic[sizeof("garadget//command") + 24];
  sprintf(
    s_topic,
    "garadget/%s/command",
    s_deviceId
  );
  o_client->subscribe(s_topic);
  sprintf(
    s_topic,
    "garadget/%s/config",
    s_deviceId
  );
  o_client->subscribe(s_topic);
  Log.info("MQTT - connected");
  return TRUE;
}

void c_mqtt::f_process() {
  if (!b_enabled) return;
  f_connect();
  o_client->loop();
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
      f_publishStatus(*(c_doorStatus*)a_message.p_payload);
      break;

  }
  return TRUE;
}

/**
 * Publishes given state to the cloud
 */
void c_mqtt::f_publishStatus(c_doorStatus n_status) {

  // make sure that device is connected to the broker
  n_lastConnect = 0;
  if (!b_enabled || !f_connect())
    return;

  // subscribe to commands
  char s_topic[sizeof("garadget//status") + 24];
  sprintf(
    s_topic,
    "garadget/%s/command",
    System.deviceID().c_str()
  );
  const char* s_status = c_config::f_statusString(n_status);
  o_client->publish(s_topic, (uint8_t*)s_status, strlen(s_status), TRUE);
  Log.info("MQTT - published status: %s", s_status);
}
