// $Id$
/**
 * @file port-test.h
 * @brief Testing Equipment Support
 * @author Denis Grisak
 * @version 1.22
 */
// $Log$

#ifndef TEST_H
#define TEST_H

#define TEST_MINRFL 25
#define DEVICEIDSIZE 25 // with extra byte for line terminator

#include "port.h"
#include "node-sensor.h"

class c_test : public c_port {

  public:

    // used for sending version info to the testing device
    typedef struct {
      char n_type;
      uint32_t n_systemVer;
      uint8_t n_firmwareVerMajor;
      uint8_t n_firmwareVerMinor;
//      uint8_t a_macAddress[6];
    } c_deviceInfo;

    union c_bufferInfo {
      c_deviceInfo a_fields;
      uint8_t a_bytes[sizeof(c_deviceInfo)];
    };

    // used for sending status info to the testing device
    typedef struct {
      char n_type;
      uint8_t n_sensorRate;
      uint16_t n_sensorBase;
      uint16_t n_buttonPushed;
      int8_t n_radioSignal;
      uint8_t n_ledStep;
      uint8_t b_relay;
      uint8_t n_padding;
    } c_deviceState;

    union c_bufferState {
      c_deviceState a_fields;
      uint8_t a_bytes[sizeof(c_deviceState)];
    };

    bool f_init();
    void f_sendInfo();
    void f_sendId();
    void f_sendState();

  protected:
    uint8_t n_ledStep = 0;
    bool b_relay = 0;
    c_sensor* o_sensor;

    void f_nextColor();
    void f_toggleRelay();
    const char* f_getSource();
};

#endif
