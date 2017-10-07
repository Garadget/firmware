// $Id$
/**
 * @file node-mqtt.h
 * @brief Implements MQTT client
 * @author Denis Grisak
 * @version 1.14
 */
// $Log$

#ifndef MQTT_H
#define MQTT_H

#include "node.h"

class c_mqtt : public c_node {

  public:
    c_mqtt();
    bool f_init();
    void f_process();
    bool f_receive(const c_message& a_message);

  protected:
    const char s_source[8] = "mqtt";
};

#endif
