// $Id$
/**
 * @file sensor.cpp
 * @brief Laser sensor related functionality
 * @author Denis Grisak
 * @version 1.9
 */
// $Log$

#include "sensor.h"

c_sensor::c_sensor() {
  pinMode(PIN_SENSOR, INPUT_PULLUP);
}

void c_sensor::f_setParams(uint8_t n_readsParam, uint8_t n_thresholdParam) {
}

uint8_t c_sensor::f_read() {
  n_reflection = digitalRead(PIN_SENSOR) ? SWITCH_REFLOPEN : SWITCH_REFLCLSD;
  return n_reflection;
}

bool c_sensor::f_isTripping() {
  return (f_read() > n_threshold);
}

uint8_t c_sensor::f_getReflection() {
  return n_reflection;
}

uint16_t c_sensor::f_getBase() {
  return n_base;
}
