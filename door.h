// $Id$
/**
 * @file door.h
 * @brief Garagio door class implementation
 * @author Denis Grisak
 * @version 1.6
 */
// $Log$

#ifndef DOOR_H
#define DOOR_H

#include "application.h"
#include "config.h"
#include "timeout.h"
#include "sensor.h"
#include "global.h"

class c_door {

    enum doorState {
        STATE_CLOSED,
        STATE_OPEN,
        STATE_CLOSING,
        STATE_OPENING,
        STATE_STOPPED,
        STATE_INIT,
        STATE_ONLINE,
        STATE_OFFLINE,
        STATE_CONFIG,
        STATE_UNKNOWN
    };

protected:
    char s_doorStatus[MAXVARSIZE];
    char s_netConfig[MAXVARSIZE];
    long n_lastEvent;
    doorState n_doorState = STATE_OPEN;
    uint8_t n_relayClicksLeft;
    bool b_alertFiredTimeout = false;
    bool b_alertFiredNight = false;

    c_config  *o_config = new c_config();
    c_sensor  *o_sensor = new c_sensor();
    c_timeout *o_scanTimeout = new c_timeout();
    c_timeout *o_relayOnTimeout = new c_timeout();
    c_timeout *o_relayOffTimeout = new c_timeout();
    c_timeout *o_motionTimeout = new c_timeout();

    void f_motionTimeout();
    void f_relayOn(uint8_t n_clicks = 0);
    void f_relayOff();
    doorState f_translateState(String s_state);
    String f_translateState(doorState n_state);
    void f_publishEvent(doorState n_event);
    void f_publishState();
    void f_publishAlert(const char* s_type, const char* s_data);
    bool f_prepNetConfig();
    void f_prepStatus();
    void f_formatTime(uint32_t n_time, char* s_time);
    void f_processAlertTimeout();
    void f_processAlertNight();

 public:
    c_door();
    void f_process();
    doorState f_getState();
    doorState f_setState(doorState n_requestedState);
    signed char f_setState(String s_request);
    int8_t f_setConfig(String s_config);
    void f_handleEvent(const char* s_topic, const char* s_data);
};

#endif
