// $Id$
/**
 * @file timeout.cpp
 * @brief Provides timer related functions to facilitate non-breaking delays
 * @author Denis Grisak
 * @version 1.18
 */
// $Log$

#include "timeout.h"

c_timeout::c_timeout(uint16_t n_initialDuration) {
  if (n_initialDuration)
    f_setDuration(n_initialDuration);
}

c_timeout::c_timeout(uint16_t* p_newDuration) {
   f_setDuration(p_newDuration);
}

void c_timeout::f_setDuration(uint16_t n_newDuration) {
    n_duration = n_newDuration;
    f_setDuration(&n_duration);
}

void c_timeout::f_setDuration(uint16_t* p_newDuration) {
    p_duration = p_newDuration;
}

void c_timeout::f_start() {
  if (!*p_duration) {
    f_stop();
    return;
  }
  b_running = TRUE;
  n_timerStart = millis();
  n_timerEnd = n_timerStart + *p_duration;
}

void c_timeout::f_stop() {
  b_running = FALSE;
};

boolean c_timeout::f_isRunning() {
  if (!b_running)
    return FALSE;
  uint32_t n_nowTime = millis();
  if (n_nowTime >= n_timerEnd &&
    (n_timerEnd >= n_timerStart || n_nowTime < n_timerStart)) {
      f_stop();
  }
  return b_running;
};

boolean c_timeout::f_isTimeout() {
  return b_running ? !f_isRunning() : FALSE;
}

uint32_t c_timeout::f_timeLeft() {
  return b_running ? n_timerEnd - millis() : 0;
}
