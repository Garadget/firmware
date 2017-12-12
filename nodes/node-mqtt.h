// $Id$
/**
 * @file node-mqtt.h
 * @brief Implements MQTT client
 * @author Denis Grisak
 * @version 1.18
 */
// $Log$

#ifndef MQTT_H
#define MQTT_H

#include "node.h"
#include "lib-mqtt/MQTT.h"

// if connection failed, how soon to try re-connect (mS)
#define N_CONNECT_PERIOD 1e4

void f_mqttCallback(char* s_topic, byte* s_payload, unsigned int n_length);

class c_mqtt : public c_node {

  public:
    bool f_init();
    void f_process();
    bool f_receive(const c_message& a_message);
    static void f_callback(char* s_topic, byte* s_payload, unsigned int n_length);

  protected:
    bool f_connect();
    void f_publishStatus(c_doorStatus n_status);
    const char* f_getTopicId();
    uint32_t n_lastConnect = 0;
    static MQTT* o_client;
};

#endif
