// $Id$
/**
 * @file door.cpp
 * @brief Garagio door class implementation
 * @author Denis Grisak
 * @version 1.6
 */
// $Log$

#include "door.h"

/** constructor */
c_door::c_door() {

  // setup hardware
  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, LOW);

  // configure sensor
  o_sensor->f_setParams(
    o_config->a_config.values.n_sensorReads,
    o_config->a_config.values.n_sensorThreshold
  );

  // configure timers
  o_scanTimeout->f_setDuration(&o_config->a_config.values.n_readTime);
  o_motionTimeout->f_setDuration(&o_config->a_config.values.n_motionTime);
  o_relayOnTimeout->f_setDuration(&o_config->a_config.values.n_relayTime);
  o_relayOffTimeout->f_setDuration(&o_config->a_config.values.n_relayPause);

// configure variables
  n_lastEvent = Time.now();
  f_prepStatus();
  f_prepNetConfig();
  Particle.variable("doorStatus", s_doorStatus, STRING);
  Particle.variable("netConfig", s_netConfig, STRING);
  o_config->f_requestName();
}

/**
 * Provides non-blocking handling of door tasks relaying on timers
 * This method has to be called periodically from the main loop
 */
void c_door::f_process() {

  Particle.process();

  // handle relay clicks
  if (o_relayOnTimeout->f_isTimeout())
    f_relayOff();
  else if (o_relayOffTimeout->f_isTimeout())
    f_relayOn();

  // handle regular state scans
  if (!o_scanTimeout->f_isRunning()) {
    f_getState();
    f_prepStatus();
    f_prepNetConfig();
    f_processAlertTimeout();
    f_processAlertNight();
    o_scanTimeout->f_start();
  }

  // hanle motion timeout
  if (o_motionTimeout->f_isTimeout())
    f_motionTimeout();
}

/**
 * Handle Open door timeout alert
 */
void c_door::f_processAlertTimeout() {

  //  skip if door closed, already fired or disabled
  if (n_doorState == STATE_CLOSED || b_alertFiredTimeout || !o_config->a_config.values.n_alertOpenTimeout)
    return;

  uint32_t n_time = Time.now() - n_lastEvent;
  if (n_time < o_config->a_config.values.n_alertOpenTimeout)
    return;

  char s_time[10];
  f_formatTime(n_time, s_time);
  f_publishAlert("timeout", s_time);
  b_alertFiredTimeout = true;
}

/**
 * Handle night time open door alert
 */
void c_door::f_processAlertNight() {

  //  skip if door closed, already fired or disabled
  if (n_doorState == STATE_CLOSED || b_alertFiredNight || o_config->a_config.values.n_alertNightStart == o_config->a_config.values.n_alertNightEnd)
    return;

  uint16_t n_time = Time.hour() * 60 + Time.minute();
  uint16_t n_start = o_config->a_config.values.n_alertNightStart;
  uint16_t n_end = o_config->a_config.values.n_alertNightEnd;

  // period crossing overnight
  if (n_start > n_end && n_time < n_start && n_time > n_end)
    return;
  // period not crossing overnight
  if (n_start < n_end && (n_time < n_start || n_time > n_end))
    return;

  char s_time[10];
  sprintf(s_time, "%u-%u", n_start, n_end);
  f_publishAlert("night", s_time);
  b_alertFiredNight = true;
}

/**
 * Garage button push (close relay)
 * Initially called with parameter by the command the by timer without parameter
 * @param[in] uint8_t n_clicks - In initial call specifies number of clicks
 */
void c_door::f_relayOn(uint8_t n_clicks) {
  if (n_clicks)
    n_relayClicksLeft = n_clicks;
  digitalWrite(PIN_RELAY, HIGH);
  o_relayOnTimeout->f_start();
}

/**
 * Garage button release (open relay)
 * Called by timer
 */
void c_door::f_relayOff() {
  digitalWrite(PIN_RELAY, LOW);
  n_relayClicksLeft--;
  if (n_relayClicksLeft)
    o_relayOffTimeout->f_start();
}

/**
 * Handles motion timeout, called by timer
 */
void c_door::f_motionTimeout() {
  switch (n_doorState) {
    case STATE_OPENING:
      n_doorState = STATE_OPEN;
      f_publishState();
      break;

    case STATE_CLOSING:
      #if APPVIRTUAL
        n_doorState = STATE_CLOSED;
        b_alertFiredTimeout = false;
        b_alertFiredNight = false;
        f_publishState();
      #else
        if (f_getState() != STATE_CLOSED) {
          n_doorState = STATE_STOPPED;
          f_publishState();
        }
      #endif
      break;
  }
}

/**
 * Translates string state to enum
 */
c_door::doorState c_door::f_translateState(String s_state) {
  if (s_state.equals("closed") || s_state.equals("close"))
    return STATE_CLOSED;
  if (s_state.equals("open"))
    return STATE_OPEN;
  if (s_state.equals("closing"))
    return STATE_CLOSING;
  if (s_state.equals("opening"))
    return STATE_OPENING;
  if (s_state.equals("stopped"))
    return STATE_STOPPED;
  return STATE_UNKNOWN;
}

/**
 * Translates enum state to string
 */
String c_door::f_translateState(doorState n_state) {
  switch (n_state) {
    case STATE_CLOSED:
      return "closed";
    case STATE_OPEN:
      return "open";
    case STATE_CLOSING:
      return "closing";
    case STATE_OPENING:
      return "opening";
    case STATE_STOPPED:
      return "stopped";
    case STATE_CONFIG:
      return "configured";
    case STATE_INIT:
      return "initialized";
    default:
      return "unknown";
  }
}

/**
 * Using laser sensor determines if door state has changed,
 *  handles the logic and updates
 */
c_door::doorState c_door::f_getState() {

  #if APPVIRTUAL
    return n_doorState == STATE_CLOSED ? STATE_CLOSED : STATE_OPEN;
  #endif
  bool b_closed = o_sensor->f_isTripping();

  // re-set state based on sensor
  if (b_closed && n_doorState != STATE_CLOSED) {
    n_doorState = STATE_CLOSED;
    b_alertFiredTimeout = false;
    b_alertFiredNight = false;
    o_motionTimeout->f_stop();
    f_publishState();
  }
  // opening initiated
  else if (!b_closed && n_doorState == STATE_CLOSED) {
    n_doorState = STATE_OPENING;
    o_motionTimeout->f_start();
    f_publishState();
  }
  return n_doorState;
}

/**
 * Processes the external state change request
 */
signed char c_door::f_setState(String s_state) {

  #ifdef APPDEBUG
    Serial.print("Received State Request: ");
    Serial.println(s_state);
  #endif

  doorState n_requestedState = f_translateState(s_state);
  if (n_requestedState == STATE_UNKNOWN)
    return -1;
  f_setState(n_requestedState);
  return 0;
}

/**
 * Handles the logic of state change request
 */
c_door::doorState c_door::f_setState(doorState n_requestedState) {

  doorState n_newDoorState;
  uint8_t n_clicks = 0;

  switch (n_requestedState) {

    // open command
    case STATE_OPEN:
    case STATE_OPENING:
      switch (n_doorState) {
        case STATE_OPEN:
        case STATE_OPENING:
          return n_doorState;
        case STATE_CLOSING:
          n_newDoorState = STATE_OPENING;
        case STATE_CLOSED:
          // rely on sensor to detect the change of state
          #ifdef APPDEBUG
            n_newDoorState = STATE_OPENING;
          #endif
          n_clicks = 1;
          break;
        case STATE_STOPPED:
          n_newDoorState = STATE_OPENING;
          n_clicks = 2;
          break;
        }
        o_motionTimeout->f_start();
        break;

    // close command
    case STATE_CLOSED:
    case STATE_CLOSING:
      switch (n_doorState) {
        case STATE_CLOSED:
        case STATE_CLOSING:
          return n_doorState;
        case STATE_OPEN:
          n_clicks = 1;
          break;
        case STATE_OPENING:
        case STATE_STOPPED:
          n_clicks = 2;
          break;
      }
      o_motionTimeout->f_start();
      n_newDoorState = STATE_CLOSING;
      break;

    // stop command
    case STATE_STOPPED:
      switch(n_doorState) {
        case STATE_STOPPED:
        case STATE_CLOSED:
        case STATE_OPEN:
          return n_doorState;
        case STATE_OPENING:
          n_clicks = 1;
          break;
        case STATE_CLOSING:
          n_clicks = 2;
          break;
      }
      o_motionTimeout->f_stop();
      n_newDoorState = STATE_STOPPED;
      break;

    // unknown state
    default:
      return n_doorState;
  }

  f_relayOn(n_clicks);
  n_doorState = n_newDoorState;
  f_publishState();

  #ifdef APPDEBUG
    Serial.print("Doing button clicks: ");
    Serial.println(n_clicks);
  #endif
  return n_doorState;
}

/**
 * Publishes given state to the cloud
 */
void c_door::f_publishEvent(doorState n_event) {
  String s_event = f_translateState(n_event);
  Particle.publish("state", s_event, 60, PRIVATE);
  #ifdef APPDEBUG
    Serial.print("Published New State: ");
    Serial.println(s_event);
  #endif
  if (o_config->a_config.values.n_alertEvents & 0x01 << n_event)
    f_publishAlert("state", s_event.c_str());
}

/**
 * Publishes current door's state to cloud
 */
void c_door::f_publishState() {
  f_publishEvent(n_doorState);
  n_lastEvent = Time.now();
  f_prepStatus();
}

/**
 * Publishes alert to the cloud
 */
void c_door::f_publishAlert(const char* s_type, const char* s_data) {
  char s_alertData[128];
  sprintf(
    s_alertData,
    "{\"name\": \"%s\", \"type\": \"%s\", \"data\": \"%s\"}",
    o_config->a_config.values.s_deviceName,
    s_type,
    s_data
  );
  Particle.publish("alert", s_alertData, 60, PRIVATE);
  #ifdef APPDEBUG
    Serial.print("Published New Alert: ");
    Serial.println(s_alertData);
  #endif
}

/**
 * Generates the string for network configuration variable
 */
bool c_door::f_prepNetConfig() {

  if (!WiFi.ready())
    return FALSE;

  IPAddress a_localIp = WiFi.localIP();
  IPAddress a_netMask = WiFi.subnetMask();
  IPAddress a_gateway = WiFi.gatewayIP();

  byte n_macAddress[6];
  WiFi.macAddress(n_macAddress);

  sprintf(
    s_netConfig,
    "ip=%d.%d.%d.%d|snet=%d.%d.%d.%d|gway=%d.%d.%d.%d|mac=%02X:%02X:%02X:%02X:%02X:%02X|ssid=%s",
    a_localIp[0], // 4+15 bytes
    a_localIp[1],
    a_localIp[2],
    a_localIp[3],
    a_netMask[0], // 6+15 bytes
    a_netMask[1],
    a_netMask[2],
    a_netMask[3],
    a_gateway[0], // 6+15bytes
    a_gateway[1],
    a_gateway[2],
    a_gateway[3],
    n_macAddress[0], // 5+17bytes
    n_macAddress[1],
    n_macAddress[2],
    n_macAddress[3],
    n_macAddress[4],
    n_macAddress[5],
    WiFi.SSID() // 6+32 bytes
  );
  return TRUE;
}

/**
 * Generates the string for door state variables
 */
void c_door::f_prepStatus() {

  char s_time[10];
  uint32_t n_time = Time.now() - n_lastEvent;

  f_formatTime(n_time, s_time);
  sprintf(
    s_doorStatus,
    "status=%s|time=%s|sensor=%u|signal=%d",
    f_translateState(n_doorState).c_str(),
    s_time,
    o_sensor->f_getLastReading(),
    WiFi.RSSI()
  );
}

void c_door::f_formatTime(uint32_t n_time, char* s_time) {
  char s_units = 's';
  if (n_time >= 120) {
    s_units = 'm';
    n_time /= 60;
    if (n_time >= 120) {
      s_units = 'h';
      n_time /= 60;
      if (n_time >= 48) {
        s_units = 'd';
        n_time /= 24;
      }
    }
  }
  sprintf(s_time, "%lu%c", n_time, s_units);
}

/**
 * Processes the config update request
 */
signed char c_door::f_setConfig(String s_config) {
  int8_t n_updates = o_config->f_set(s_config);
  #ifdef APPDEBUG
    Serial.print("Config update: ");
    Serial.print(s_config);
    Serial.print(", EEPROM bytes updated: ");
    Serial.println(n_updates);
  #endif

  if (!n_updates)
    return 0;

  // configure sensor
  o_sensor->f_setParams(
    o_config->a_config.values.n_sensorReads,
    o_config->a_config.values.n_sensorThreshold
  );
  f_publishAlert("config", s_doorStatus);
  return n_updates;
}

/**
 * Handles cloud event
 */
void c_door::f_handleEvent(const char* s_topic, const char* s_data) {
  #ifdef APPDEBUG
    Serial.print("Handling Topic: ");
    Serial.print(s_topic);
    Serial.print(" Value: ");
    Serial.println(s_data);
  #endif
  if (String(s_topic).equals("spark/device/name")) {
    o_config->f_setName(String(s_data));
    if (!b_initialized) {
      f_publishEvent(STATE_INIT);
      b_initialized = true;
    }
  }
 }
