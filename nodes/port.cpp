// $Id$
/**
 * @file node.cpp
 * @brief Abstract class for I2C port accessory
 * @author Denis Grisak
 * @version 1.20
 */
// $Log$

#include "port.h"

uint8_t c_port::n_devices = 0;

const char* c_port::f_getSource() {
  return "port";
}

bool c_port::f_init() {
  Log.info("%s - scanning for connected devices", f_getSource());
  Wire.begin();
  uint8_t a_buffer[PORT_BUFFER];
  if (f_busRequest(a_buffer) > 0) {
    b_enabled = TRUE;
    o_scanTimer.f_setDuration(n_scanPeriod);
    c_port::n_devices++;
    Log.info("%s - device connected", f_getSource());
  }
  if (!c_port::n_devices)
    Wire.end();
  return b_enabled;
}

void c_port::f_process() {
  if (!b_enabled || !b_input || o_scanTimer.f_isRunning())
    return;

  uint8_t a_buffer[PORT_BUFFER];
  uint8_t n_size = f_busRequest(a_buffer, PORT_BUFFER);

  // check if response is any different from previous
  if (n_size == n_lastResponse && memcmp(a_lastResponse, a_buffer, n_size))
    return;

  memcpy(a_lastResponse, a_buffer, n_size);
  n_lastResponse = n_size;

  c_message a_message = {
    f_getSource(),
    MSG_PORT,
    a_lastResponse
  };

  f_handle(a_message);
  o_scanTimer.f_start();
}

/**
 * Sends data from memory pointer to I2C device
 * @param[in] n_data Pointer to the memory address
 * @param[in] n_length Number of bytes to send
 * @return Length of sent data
 */
int8_t c_port::f_busSend(uint8_t* a_buffer, uint8_t n_length) {
  Wire.beginTransmission(n_address);
  Wire.write(a_buffer, n_length);
  Wire.endTransmission();
  return n_length;
}

/**
 * Request data from connected client and receive it into buffer
 * @param[in] n_address Address of slave I2C device
 * @return Length of received response
 */
int8_t c_port::f_busRequest(uint8_t* a_buffer, uint8_t n_maxLength) {
  Log.info("%s - requesting data from %u", f_getSource(), n_address);
  Wire.requestFrom(n_address, n_maxLength);
  uint32_t n_timeout = millis() + PORT_TIMEOUT;
  while (!Wire.available()) {
    if (millis() > n_timeout) {
      Log.info("%s - no response from %u", f_getSource(), n_address);
      return -1;
    }
  }
  Log.info("%s - received response from %u", f_getSource(), n_address);
  uint8_t n_length = 0;
  for (; Wire.available() && n_length < n_maxLength; n_length++)
    a_buffer[n_length] = Wire.read();
  return n_length;
}
