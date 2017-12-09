// $Id$
/**
* @file port-test.cpp
* @brief Implements manufacturing test procedures
* @author Denis Grisak
* @version 1.0
*/
// $Log$

#include "port-test.h"

const char* c_test::f_getSource() {
  return "test";
}

bool c_test::f_init() {
  n_address = 0x11;
  if (!c_port::f_init())
    return FALSE;

  Particle.disconnect();
  RGB.control(true);
  f_sendInfo();
  f_sendId();

  // start diagnostic loop
  for (int n_loop = 0; true; n_loop++) {
    f_nextColor();

    // every second loop flip the relay
    if (n_loop % 2)
      f_toggleRelay();
    f_sendState();

    if (System.buttonPushed() > 500) {
      RGB.control(false);
      f_sendState();
      Particle.connect();
      WiFi.listen();
      break;
    }
    delay(200);
  }

  for (int n_loop = 0; n_loop < 10; n_loop++) {
      f_sendState();
      delay(1000);
  }
  return true;
}

void c_test::f_sendInfo() {
  c_bufferInfo a_data;
  a_data.a_fields.n_type = 'V';
  a_data.a_fields.n_systemVer = System.versionNumber();
  a_data.a_fields.n_firmwareVerMajor = VERSION_MAJOR;
  a_data.a_fields.n_firmwareVerMinor = VERSION_MINOR;
  WiFi.macAddress(a_data.a_fields.a_macAddress);
  f_busSend(
    a_data.a_bytes,
    sizeof(c_bufferInfo)
  );
}

void c_test::f_sendId() {
  String s_deviceId = "I" + System.deviceID().toUpperCase();
  f_busSend(
    (uint8_t*)s_deviceId.c_str(),
    DEVICEIDSIZE
  );
}

void c_test::f_sendState() {

    c_bufferState a_data;
    a_data.a_fields.n_type = 'S';

    c_sensor& o_sensor = c_sensor::f_getInstance();

    // get laser sensor data
    a_data.a_fields.n_sensorRate = o_sensor.f_read();
    a_data.a_fields.n_sensorBase = o_sensor.f_getBase();
    a_data.a_fields.n_radioSignal = WiFi.RSSI();
    a_data.a_fields.n_buttonPushed = System.buttonPushed();
    a_data.a_fields.n_ledStep = n_ledStep;
    a_data.a_fields.b_relay = b_relay;
    a_data.a_fields.n_padding = 0;

    f_busSend(
        a_data.a_bytes,
        sizeof(c_bufferState)
    );
}

void c_test::f_nextColor() {
    n_ledStep = (n_ledStep + 1) % 3;
    RGB.color(
        n_ledStep == 0 ? 0xFF : 0x00,
        n_ledStep == 1 ? 0xFF : 0x00,
        n_ledStep == 2 ? 0xFF : 0x00
    );
}

void c_test::f_toggleRelay() {
    b_relay = !b_relay;
    digitalWrite(PIN_RELAY, b_relay);
    delay(20);
}
