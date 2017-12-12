// $Id$
/**
 * @file node.cpp
 * @brief Abstract class for node implementations
 * @author Denis Grisak
 * @version 1.18
 */
// $Log$

#include "node.h"

c_config& c_node::f_getConfig() {
  return c_config::f_getInstance();
}

bool c_node::f_init() {
  return b_enabled;
}

void c_node::f_process() {
}

bool c_node::f_receive(const c_message &a_message) {
  return TRUE;
}
