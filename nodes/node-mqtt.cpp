// $Id$
/**
 * @file node-mqtt.cpp
 * @brief Implements MQTT client
 * @author Denis Grisak
 * @version 1.14
 */
// $Log$

#include "node-mqtt.h"

c_mqtt::c_mqtt() {
  // link config and status values
//  c_config& o_config = f_getConfig();
//  n_status = &o_config.a_state.n_doorStatus;
}

bool c_mqtt::f_init() {
  return true;
}

void c_mqtt::f_process() {
}

/**
 * Control the timers based on simulated status updates
 */
bool c_mqtt::f_receive(const c_message& a_message) {
  return true;
}
