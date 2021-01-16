// $Id$
/**
 * @file node-relay.cpp
 * @brief Implements relay functions
 * @author Denis Grisak
 * @version 1.24
 */
// $Log$

#include "node-relay.h"

bool c_relay::f_init() {
  // link config and status values
  c_config& o_config = f_getConfig();
  n_status = &o_config.a_state.n_doorStatus;

  // configure timers
  o_onTimer.f_setDuration(&o_config.a_config.n_relayTime);
  o_offTimer.f_setDuration(&o_config.a_config.n_relayPause);

  // configure hardware
  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, LOW);

  return TRUE;
}

void c_relay::f_process() {

  // handle relay clicks
  if (o_onTimer.f_isTimeout())
    f_off();
  else if (o_offTimer.f_isTimeout())
    f_on();
}

/**
 * Garage button push (close relay)
 * Initially called with parameter by the command the by timer without parameter
 * @param[in] uint8_t n_clicks - In initial call specifies number of clicks
 */
void c_relay::f_on(uint8_t n_clicks) {
  if (n_clicks)
    n_clicksLeft = n_clicks;
  digitalWrite(PIN_RELAY, HIGH);
  o_onTimer.f_start();
}

/**
 * Garage button release (open relay)
 * Called by timer
 */
void c_relay::f_off() {
  digitalWrite(PIN_RELAY, LOW);
  n_clicksLeft--;
  if (n_clicksLeft)
    o_offTimer.f_start();
}

bool c_relay::f_receive(const c_message& a_message) {
  if (a_message.n_type != MSG_COMMAND)
    return true;

  uint8_t n_clicks = 0;
  c_doorStatus n_command = *(c_doorStatus*)a_message.p_payload;
  c_doorStatus n_newStatus = STATUS_UNKNOWN;

  switch (n_command) {

    // open command
    case STATUS_OPEN:
    case STATUS_OPENING:
      switch (*n_status) {
        case STATUS_OPEN:
        case STATUS_OPENING:
          return true;
        case STATUS_CLOSING:
          n_newStatus = STATUS_OPENING;
        case STATUS_CLOSED:
          // rely on sensor to detect the change of state
          #ifdef APPVIRTUAL
            n_newStatus = STATUS_OPENING;
          #endif
          n_clicks = 1;
          break;
        case STATUS_STOPPED:
          n_newStatus = STATUS_OPENING;
          n_clicks = 2;
          break;
        }
        break;

    // close command
    case STATUS_CLOSED:
    case STATUS_CLOSING:
      switch (*n_status) {
        case STATUS_CLOSED:
        case STATUS_CLOSING:
          return true;
        case STATUS_OPEN:
        case STATUS_STOPPED:
          n_clicks = 1;
          break;
        case STATUS_OPENING:
          n_clicks = 2;
          break;
      }
      n_newStatus = STATUS_CLOSING;
      break;

    // stop command
    case STATUS_STOPPED:
      switch (*n_status) {
        case STATUS_STOPPED:
        case STATUS_CLOSED:
        case STATUS_OPEN:
          return true;
        case STATUS_OPENING:
          n_clicks = 1;
          break;
        case STATUS_CLOSING:
          n_clicks = 2;
          break;
      }
      n_newStatus = STATUS_STOPPED;
      break;

    // unknown state
    default:
      return true;
  }

  Log.info("Relay - clicks: %u", n_clicks);
  f_on(n_clicks);
  if (n_newStatus == STATUS_UNKNOWN)
    return true;

  Log.info("Relay - new status: %s", c_config::f_statusString(n_newStatus));
  *n_status = n_newStatus;
  c_message a_newMessage = {
    "relay",
    MSG_STATUS,
    n_status
  };

  return f_handle(a_newMessage);
}
