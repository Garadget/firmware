// $Id$
/**
 * @file sensor.cpp
 * @brief Laser sensor related functionality
 * @author Denis Grisak
 * @version 1.0
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

    int n_value;
    long n_sum1 = 0,
        n_sum2 = 0;

    for (uint8_t n_read = n_reads; n_read > 0; n_read--) {
        n_value = analogRead(PIN_PHOTO);
        n_sum1 += n_value;
        digitalWriteFast(PIN_LASER, HIGH);
        delayMicroseconds(500);
        n_sum2 += n_value - analogRead(PIN_PHOTO);
        digitalWriteFast(PIN_LASER, LOW);
        delayMicroseconds(1000);
    }
    return n_sum1 ? (float)n_sum2 * 100 / n_sum1 : 0;
}

bool c_sensor::f_isTripping() {
    n_lastReadValue = f_read();
    return n_lastReadValue > n_threshold;
}

uint8_t c_sensor::f_getLastReading() {
    return n_lastReadValue;
}
