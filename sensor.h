// $Id$
/**
 * @file sensor.h
 * @brief Laser sensor related functionality
 * @author Denis Grisak
 * @version 1.9
 */
// $Log$

#ifndef SENSOR_H
#define SENSOR_H

#include "application.h"
#include "global.h"

class c_sensor {

protected:
  uint8_t n_reads = 3;
  uint8_t n_threshold = 25;
  uint8_t n_reflection;
  uint16_t n_base;

public:
  c_sensor();
  void f_setParams(uint8_t n_readsParam, uint8_t n_thresholdParam);
  bool f_isTripping();
  uint8_t f_read();
  uint8_t f_getReflection();
  uint16_t f_getBase();
};

#endif
