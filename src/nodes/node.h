// $Id$
/**
 * @file node.h
 * @brief Abstract class for node implementations
 * @author Denis Grisak
 * @version 1.20
 */
// $Log$

#ifndef NODE_H
#define NODE_H

#include "application.h"
#include "../global.h"
#include "../config.h"
#include "../utils/timeout.h"

enum c_messageType {
  MSG_NONE = 0,
  MSG_INIT,
  MSG_COMMAND,
  MSG_STATUS,
  MSG_CONFIG,
  MSG_ALERT,
  MSG_PORT
};

typedef struct {
  const char* s_source;
  c_messageType n_type;
  const void* p_payload;
} c_message;

// forward declaration of global event handler function
bool f_handle(const c_message& a_message);
class c_config;

class c_node {
  public:
    virtual bool f_init();
    virtual void f_process();
    virtual bool f_receive(const c_message& a_message);

  protected:
    static c_config& f_getConfig();
    bool b_enabled = FALSE;
};

#endif
