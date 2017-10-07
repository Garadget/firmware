// $Id$
/**
 * @file node-sensor.h
 * @brief Implements laser sensor
 * @author Denis Grisak
 * @version 1.14
 */
// $Log$

#ifndef SENSOR_H
#define SENSOR_H

#include "node.h"

class c_sensor : public c_node {

  public:
    c_sensor();
    void f_process();
    c_doorStatus f_getState();
    bool f_receive(const c_message& a_message);

  protected:
    uint8_t f_read();
    bool f_isTripping();
    bool f_onChange(c_doorStatus n_newStatus);
    void f_onMotionEnd();

    c_timeout o_scanTimer;
    c_timeout o_motionTimer;
    c_doorStatus* n_status;
    uint8_t* n_reflection;
    uint16_t* n_base;
    uint8_t* n_threshold;
    uint16_t* n_readTime;
    const char s_source[8] = "sensor";
};

#endif
