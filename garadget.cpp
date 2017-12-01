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

#include "nodes/node-particle.h"
#include "nodes/node-relay.h"
#include "nodes/node-sensor.h"
#include "nodes/node-alert.h"
#include "nodes/node-mqtt.h"
#include "softap/webconfig.h"

SYSTEM_MODE(MANUAL);
STARTUP(softap_set_application_page_handler(f_pageHandler, nullptr));

// Photon module - antenna preference
#ifdef ANT_MODE
  STARTUP(WiFi.selectAntenna(ANT_MODE));
#endif

#ifdef APPDEBUG
  SerialLogHandler logHandler(
    LOG_LEVEL_WARN, { // Logging level for non-application messages
      { "app", LOG_LEVEL_ALL } // Logging level for application messages
    }
  );
#endif

c_particle o_cloud = c_particle();
c_relay o_relay = c_relay();
c_sensor o_sensor = c_sensor();
c_alert o_alert = c_alert();
c_mqtt o_mqtt = c_mqtt();

void setup() {
#ifdef APPDEBUG
  // wait 5 seconds for "M" button in debug mode to start serial interface
  while (!System.buttonPushed() && millis() < 5000);
#endif
  o_cloud.f_init();
  o_mqtt.f_init();
}

void loop() {
  o_cloud.f_process();
  o_mqtt.f_process();
  o_relay.f_process();
  o_sensor.f_process();
  o_alert.f_process();
}

bool f_handle(const c_message& a_message) {
  o_cloud.f_receive(a_message);
  o_mqtt.f_receive(a_message);
  o_relay.f_receive(a_message);
  o_sensor.f_receive(a_message);
  o_alert.f_receive(a_message);
  return TRUE;
}
