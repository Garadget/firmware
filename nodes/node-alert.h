// $Id$
/**
 * @file node-alert.h
 * @brief Implements alerts
 * @author Denis Grisak
 * @version 1.20
 */
// $Log$

#ifndef ALERT_H
#define ALERT_H

#include "node.h"

class c_alert : public c_node {

  public:
    bool f_init();
    void f_process();
    bool f_receive(const c_message& a_message);

  protected:
    bool f_isNight();
    void f_handleStatus();
    void f_fireAlert(const char* s_type, const char* s_data);
    void f_statusAlert();
    void f_timeoutAlert();
    void f_nightAlert();

    c_doorStatus* n_status;
    uint16_t* n_events;
    uint16_t* n_timeout;
    uint16_t* n_nightStart;
    uint16_t* n_nightEnd;
    c_timeout o_timeout;

    uint32_t n_timeOpened = 0;
    uint8_t n_timeoutAlertsFired = 0;
    uint8_t n_nightAlertsFired = 0;
};

#endif
