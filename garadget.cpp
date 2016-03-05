// $Id$
/**
 * @file application.ino
 * @brief Garagio main file
 * @version 1.3
 * @author Denis Grisak
 * @license CC0 1.0 Universal

  This code is firmware for Garagio - a WiFi garage door controller.
  For more information please visit http://garag.io/
 */
// $Log$

#include "application.h"
#include "global.h"
#include "door.h"

// Particle platform - product settings
PRODUCT_ID(PROD_ID);
PRODUCT_VERSION(VERSION_MAJOR*100+VERSION_MINOR);


c_door* o_door;

int f_doorSetState(String s_command) {
    return o_door->f_setState(s_command);
}

int f_setConfig(String s_config) {
    int n_updates = o_door->f_setConfig(s_config);
    if (n_updates > 0)
        Particle.publish("config", String(n_updates), 60, PRIVATE);
    #ifdef APPDEBUG
        Serial.print("Config update result: ");
        Serial.println(n_updates);
    #endif
    return n_updates;
}

void setup() {
    #ifdef APPDEBUG
        Serial.begin(115200);
    #endif
    o_door = new c_door();
    Particle.function("setState", f_doorSetState);
    Particle.function("setConfig", f_setConfig);
}

void loop() {
    o_door->f_process();
}
