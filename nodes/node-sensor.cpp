// $Id$
/**
 * @file node-sensor.cpp
 * @brief Implements laser sensor
 * @author Denis Grisak
 * @version 1.14
 */
// $Log$

#include "node-sensor.h"

c_sensor::c_sensor() {
  // link config and status values
  c_config& o_config = f_getConfig();
  n_status = &o_config.a_state.n_doorStatus;
  n_reflection = &o_config.a_state.n_sensorReflection;
  n_base = &o_config.a_state.n_sensorBase;
  n_threshold = &o_config.a_config.n_sensorThreshold;

  // configure timers
  o_scanTimer.f_setDuration(&o_config.a_config.n_readTime);
  o_motionTimer.f_setDuration(&o_config.a_config.n_motionTime);
  *n_status = STATUS_OPEN;

  // configure hardware
  pinMode(PIN_LASER, OUTPUT);
  digitalWrite(PIN_LASER, LOW);

  // perform the initial scan
  f_getState();
}

void c_sensor::f_process() {
  if (!o_scanTimer.f_isRunning()) {
    f_getState();
    o_scanTimer.f_start();
  }
  if (o_motionTimer.f_isTimeout())
    f_onMotionEnd();
}

uint8_t c_sensor::f_read() {

  uint32_t
    n_sumBase = 0,
    n_sumScan = 0;

  for (uint8_t n_read = SENSOR_READS; n_read > 0; n_read--) {
    *n_base = analogRead(PIN_PHOTO);
    n_sumBase += *n_base - SENSOR_BIAS;
    digitalWriteFast(PIN_LASER, HIGH);
    delay(1);
    n_sumScan += *n_base - analogRead(PIN_PHOTO);
    digitalWriteFast(PIN_LASER, LOW);
    delay(1);
  }

  // scan valid - recalculate the reflection
  if (n_sumBase && n_sumScan <= n_sumBase)
    *n_reflection = n_sumScan * 100 / n_sumBase;

  return *n_reflection;
}

bool c_sensor::f_isTripping() {
  return (f_read() > *n_threshold);
}

/**
 * Using laser sensor determines if door state has changed,
 *  handles the logic and updates
 */
c_doorStatus c_sensor::f_getState() {

  #if APPVIRTUAL
    return *n_status == STATUS_CLOSED
      ? STATUS_CLOSED
      : STATUS_OPEN;
  #endif
  bool b_closed = f_isTripping();

  // re-set state based on sensor
  if (b_closed && *n_status != STATUS_CLOSED) {
    f_onChange(STATUS_CLOSED);
  }
  // opening initiated
  else if (!b_closed && *n_status == STATUS_CLOSED) {
    f_onChange(STATUS_OPENING);
  }
  return *n_status;
}

/**
 * Handles expected end of door motion
 */
void c_sensor::f_onMotionEnd() {
  switch (*n_status) {
    case STATUS_OPENING:
      f_onChange(STATUS_OPEN);
      break;

    case STATUS_CLOSING:
      #if APPVIRTUAL
        f_onChange(STATUS_CLOSED);
      #else
        if (f_getState() != STATUS_CLOSED)
          f_onChange(STATUS_STOPPED);
      #endif
      break;
  }
}

/**
 * Register the change of door state
 */
bool c_sensor::f_onChange(c_doorStatus n_newStatus) {
  Log.info("Sensor - new status: %s", c_config::f_statusString(n_newStatus));
  *n_status = n_newStatus;
  f_getConfig().a_state.n_lastEvent = millis();
  c_message a_message = {
    s_source,
    MSG_STATUS,
    n_status
  };
  return f_handle(a_message);
}

/**
 * Control the timers based on simulated status updates
 */
bool c_sensor::f_receive(const c_message& a_message) {
  if (a_message.n_type != MSG_STATUS)
    return true;

  switch (*n_status) {
    case STATUS_OPENING:
    case STATUS_CLOSING:
      f_getConfig().a_state.n_lastEvent = millis();
      o_motionTimer.f_start();
      break;

    case STATUS_CLOSED:
    case STATUS_OPEN:
    case STATUS_STOPPED:
      o_motionTimer.f_stop();
      break;
  }
  return true;
}
