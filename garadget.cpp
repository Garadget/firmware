// $Id$
/**
 * @file application.ino
 * @brief Garadget main file
 * @version 1.7
 * @author Denis Grisak
 * @license GPL v3

  This code is firmware for Garadget - cloud-enabled garage door controller.
  For more information please visit http://www.garadget.com/
 */
// $Log$

#include "application.h"
#include "global.h"
#include "door.h"

// Particle platform - product settings
#ifdef ORGMODE
  PRODUCT_ID(PROD_ID);
  PRODUCT_VERSION(VERSION_MAJOR * 100 + VERSION_MINOR);
#endif
STARTUP(WiFi.selectAntenna(ANT_MODE));

c_door* o_door;

int f_doorSetState(String s_command) {
  return o_door->f_setState(s_command);
}

int f_setConfig(String s_config) {
  return o_door->f_setConfig(s_config);
}

void f_handleEvent(const char* s_topic, const char* s_data) {
  o_door->f_handleEvent(s_topic, s_data);
}

void setup() {
  #ifdef APPDEBUG
    Serial.begin(115200);
  #endif
  o_door = new c_door();
  Particle.function("setState", f_doorSetState);
  Particle.function("setConfig", f_setConfig);
  Particle.subscribe("spark/", f_handleEvent);
}

void loop() {
  o_door->f_process();
}
