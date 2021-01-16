// $Id$
/**
 * @file node.h
 * @brief Abstract class for I2C port accessory
 * @author Denis Grisak
 * @version 1.24
 */
// $Log$

#ifndef PORT_H
#define PORT_H

#include "node.h"

class c_port : public c_node {

  public:

    static uint8_t n_devices;

    bool f_init();
    void f_process();
//    bool f_receive(const c_message& a_message);
    void f_close();

  protected:

    uint8_t n_address = 0x00;
    const bool b_input = FALSE;
    const bool b_output = FALSE;
    const uint16_t n_scanPeriod = 1000; // by default scan every second

    uint8_t a_lastResponse[PORT_BUFFER];
    uint8_t n_lastResponse = 0;
    PinMode n_savedPinModeSda;
    PinMode n_savedPinModeScl;
    boolean b_savedPinLevelSda;
    boolean b_savedPinLevelScl;

    c_timeout o_scanTimer;
    int8_t f_busSend(uint8_t* a_buffer, uint8_t n_length);
    int8_t f_busRequest(uint8_t* a_buffer, uint8_t n_maxLength = PORT_BUFFER);
    const char* f_getSource();
};

#endif
