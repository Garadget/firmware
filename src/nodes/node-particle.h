// $Id$
/**
 * @file node-particle.h
 * @brief Implements Particle cloud interface
 * @author Denis Grisak
 * @version 1.24
 */
// $Log$

#ifndef NODE_PARTICLE_H
#define NODE_PARTICLE_H

// Particle platform - product settings
#ifdef ORGMODE
  PRODUCT_ID(PROD_ID);
  PRODUCT_VERSION(VERSION_ID);
#endif

#include "node.h"
class c_particle : public c_node {

  public:
    bool f_init();
    void f_process();
    bool f_receive(const c_message& a_message);

    int f_receiveConfig(String s_config);
    int f_receiveState(String s_config);
    void f_handleEvent(const char* s_topic, const char* s_data);

  protected:
    void f_declare();
    void f_updateDoorStatus();
    void f_updateDoorConfig();
    void f_updateNetConfig();
    void f_publishStatus(c_doorStatus n_status);
    void f_publishAlert(char* s_alertData);

    c_timeout o_updateTimer;
    char s_doorConfig[MAXVARSIZE];
    char s_doorStatus[MAXVARSIZE];
    char s_netConfig[MAXVARSIZE];
    bool b_boot = TRUE;
    bool b_init = TRUE;
};

#endif
