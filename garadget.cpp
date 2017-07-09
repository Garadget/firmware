// $Id$
/**
 * @file application.ino
 * @brief Garadget main file
 * @version 1.13
 * @author Denis Grisak
 * @license GPL v3

  This code is firmware for Garadget - cloud-enabled garage door controller.
  For more information please visit http://www.garadget.com/
 */
// $Log$

// @todo: validate configuration settings when Loading
// @todo: debounce the status messages

#include "application.h"
#include "global.h"
#include "door.h"
#include "webconfig.h"
STARTUP(softap_set_application_page_handler(f_pageHandler, nullptr));

// Particle platform - product settings
#ifdef ORGMODE
  PRODUCT_ID(PROD_ID);
  PRODUCT_VERSION(VERSION_ID);
#endif
#ifdef ANT_MODE
  STARTUP(WiFi.selectAntenna(ANT_MODE));
#endif

c_door& o_door = c_door::f_getDoor();

void setup() {
  #ifdef APPDEBUG
    Serial.begin(115200);
    Serial.println("Setup");
  #endif
  o_door.f_init();
}

void loop() {
  o_door.f_process();
}
