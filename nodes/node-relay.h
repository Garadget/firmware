// $Id$
/**
 * @file node-relay.h
 * @brief Implements relay functions
 * @author Denis Grisak
 * @version 1.14
 */
// $Log$

#ifndef RELAY_H
#define RELAY_H

#include "node.h"

class c_relay : public c_node {

  public:
    c_relay();
    void f_process();
    bool f_receive(const c_message& a_message);

  protected:
    void f_on(uint8_t n_clicks = 0);
    void f_off();

    c_timeout o_onTimer;
    c_timeout o_offTimer;
    c_doorStatus* n_status;
    uint8_t n_clicksLeft;
};

#endif
