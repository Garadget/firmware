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
    pinMode(PIN_LASER, OUTPUT);
    digitalWrite(PIN_LASER, LOW);
}

void c_sensor::f_setParams(uint8_t n_readsParam, uint8_t n_thresholdParam) {
    n_reads = n_readsParam;
    n_threshold = n_thresholdParam;
}

uint8_t c_sensor::f_read() {

  uint32_t n_sumBase = 0,
      n_sumScan = 0;

  for (uint8_t n_read = n_reads; n_read > 0; n_read--) {
    n_base = analogRead(PIN_PHOTO);
    n_sumBase += n_base - SENSOR_BIAS;
    digitalWriteFast(PIN_LASER, HIGH);
    delay(1);
    n_sumScan += n_base - analogRead(PIN_PHOTO);
    digitalWriteFast(PIN_LASER, LOW);
    delay(1);
  }
  // scan valid - recalculate the reflection
  if (n_sumBase && n_sumScan <= n_sumBase)
    n_reflection = n_sumScan * 100 / n_sumBase;

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
