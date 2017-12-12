// $Id$
/**
 * @file node-alert.cpp
 * @brief Implements alerts functionality
 * @author Denis Grisak
 * @version 1.18
 */
// $Log$

#include "node-alert.h"

bool c_alert::f_init() {

  // link config and status values
  c_config& o_config = f_getConfig();

  n_status = &o_config.a_state.n_doorStatus;
  n_events = &o_config.a_config.n_alertEvents;
  n_timeout = &o_config.a_config.n_alertOpenTimeout;
  n_nightStart = &o_config.a_config.n_alertNightStart;
  n_nightEnd = &o_config.a_config.n_alertNightEnd;

  f_handleStatus();
  return TRUE;
}

void c_alert::f_process() {

  if (*n_status == STATUS_CLOSED)
    return;

  // send timeout alert
  if (!n_timeoutAlertsFired && o_timeout.f_isTimeout())
    f_timeoutAlert();

  // send night alert
  if (!n_nightAlertsFired && f_isNight())
    f_nightAlert();
}

bool c_alert::f_isNight() {
  if (!Time.isValid() || *n_nightStart == *n_nightEnd)
    return false;

  uint16_t n_time = Time.hour() * 60 + Time.minute();

  // period crossing overnight
  if (*n_nightStart > *n_nightEnd && n_time < *n_nightStart && n_time > *n_nightEnd)
    return false;
  // period not crossing overnight
  if (*n_nightStart < *n_nightEnd && (n_time < *n_nightStart || n_time > *n_nightEnd))
    return false;

  return true;
}

void c_alert::f_fireAlert(const char* s_type, const char* s_data) {
  Log.info("Alert - type: %s, data: %s", s_type, s_data);

  char s_alertData[MAXVARSIZE + 128];
  sprintf(
    s_alertData,
    "{\"name\": \"%s\", \"type\": \"%s\", \"data\": \"%s\"}",
    f_getConfig().a_config.s_deviceName,
    s_type,
    s_data
  );

  c_message a_message = {
    "alert",
    MSG_ALERT,
    s_alertData
  };
  f_handle(a_message);
}

/**
 * Handle status alert
 */
void c_alert::f_statusAlert() {
  f_fireAlert("state", c_config::f_statusString(*n_status));
}

/**
 * Handle Open door timeout alert
 */
void c_alert::f_timeoutAlert() {
  char s_time[10];
  c_config::f_formatTime(millis() - n_timeOpened, s_time);
  f_fireAlert("timeout", s_time);
  n_timeoutAlertsFired = 1;
}

/**
 * Handle night time open door alert
 */
void c_alert::f_nightAlert() {
  char s_time[10];
  sprintf(s_time, "%u-%u", *n_nightStart, *n_nightEnd);
  f_fireAlert("night", s_time);
  n_nightAlertsFired = 1;
}


bool c_alert::f_receive(const c_message& a_message) {
  if (a_message.n_type != MSG_STATUS)
    return true;
  f_handleStatus();
  return true;
}

void c_alert::f_handleStatus() {

  // handle status alerts for enabled statuses
  if (*n_events & (0x01 << (*n_status - 1)))
    f_statusAlert();

  switch (*n_status) {
    case STATUS_OPENING:
    case STATUS_CLOSING:
    case STATUS_OPEN:
    case STATUS_STOPPED:
      if (*n_timeout && !o_timeout.f_isRunning()) {
        o_timeout.f_setDuration(*n_timeout * 1000 * 60);
        o_timeout.f_start();
        n_timeOpened = millis();
      }
      break;

    case STATUS_CLOSED:
      o_timeout.f_stop();
      n_timeoutAlertsFired = 0;
      n_nightAlertsFired = 0;
      break;
  }
}
