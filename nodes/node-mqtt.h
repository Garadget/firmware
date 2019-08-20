// $Id$
/**
 * @file node-mqtt.h
 * @brief Implements MQTT client
 * @author Denis Grisak
 * @version 1.20
 */
// $Log$

#ifndef MQTT_H
#define MQTT_H

#include "node.h"
#include "command.h"
#include "lib-mqtt/MQTT.h"

class c_mqtt : public c_node, public MQTT {

  public:
    bool f_init();
    void f_process();
    bool f_receive(const c_message& a_message);

    void f_onConnectHanlder(uint8_t n_result);
    void f_onReceiveHandler(char* s_topic, uint8_t* s_payload, unsigned int n_length);

  protected:
    bool f_connect();
    void f_subscribe();
    void f_publishStatus();
    void f_publishConfig();
    const char* f_getClientId();
    //MQTT* o_client = NULL;
};

#endif
