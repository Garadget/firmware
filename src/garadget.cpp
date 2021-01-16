// $Id$
/**
 * @file application.ino
 * @brief Garadget main file
 * @version 1.24
 * @author Denis Grisak
 * @license GPL v3

  This code is firmware for Garadget - cloud-enabled garage door controller.
  For more information visit http://www.garadget.com/
 */
// $Log$


#include "application.h"
#include "global.h"
#include "config.h"
#include "nodes/node-particle.h"
#include "nodes/node-mqtt.h"
#include "nodes/node-sensor.h"
#include "nodes/node-relay.h"
#include "nodes/node-alert.h"
#include "softap/webconfig.h"
#ifdef TESTPORT
  #include "nodes/port-test.h"
#endif


SYSTEM_MODE(MANUAL);
// SYSTEM_THREAD(ENABLED);

STARTUP(
  // Photon module - antenna preference
  #ifdef ANT_MODE
    WiFi.selectAntenna(ANT_MODE);
  #endif
  softap_set_application_page_handler(f_pageHandler, nullptr);
);

#ifdef APPDEBUG
  SerialLogHandler logHandler(
    LOG_LEVEL_WARN, { // Logging level for non-application messages
      { "app", LOG_LEVEL_ALL } // Logging level for application messages
    }
  );
#endif

c_config& o_config = c_config::f_getInstance();
c_sensor& o_sensor = c_sensor::f_getInstance();
c_relay o_relay = c_relay();
c_particle o_cloud = c_particle();
c_mqtt o_mqtt = c_mqtt();
c_alert o_alert = c_alert();
#ifdef TESTPORT
  c_test o_test = c_test();
#endif

void setup() {
#ifdef APPDEBUG
  // wait 3 seconds for "M" button in debug mode to start serial interface
  while (!System.buttonPushed() && millis() < 3000);
#endif
o_config.f_init();
o_sensor.f_init();
o_relay.f_init();
#ifdef TESTPORT
  o_test.f_init();
#endif
  WiFi.connect();
  o_cloud.f_init();
  o_mqtt.f_init();
  o_alert.f_init();
}

void loop() {
  o_config.f_process();
  o_cloud.f_process();
  o_mqtt.f_process();
  o_relay.f_process();
  o_sensor.f_process();
  o_alert.f_process();
}

bool f_handle(const c_message& a_message) {
  o_relay.f_receive(a_message);
  o_sensor.f_receive(a_message);
  o_alert.f_receive(a_message);
  o_cloud.f_receive(a_message);
  o_mqtt.f_receive(a_message);
  return TRUE;
}
