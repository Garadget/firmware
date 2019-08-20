// $Id$
/**
 * @file node-sensor.h
 * @brief Implements laser sensor
 * @author Denis Grisak
 * @version 1.22
 */
// $Log$

#ifndef SENSOR_H
#define SENSOR_H

#include "node.h"

class c_sensor : public c_node {

  public:

    static c_sensor& f_getInstance();
    c_sensor(c_sensor const&);
    void operator=(c_sensor const&);

    bool f_init();
    void f_process();
    c_doorStatus f_getState();
    bool f_receive(const c_message& a_message);
    uint8_t f_read();
    uint16_t f_getBase();

  protected:
    c_sensor();
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
};

#endif
