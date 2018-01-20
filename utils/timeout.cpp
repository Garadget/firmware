// $Id$
/**
 * @file timeout.cpp
 * @brief Provides timer related functions to facilitate non-breaking delays
 * @author Denis Grisak
 * @version 1.20
 */
// $Log$

#include "timeout.h"

c_timeout::c_timeout(uint32_t n_initialDuration) {
  if (n_initialDuration)
    f_setDuration(n_initialDuration);
}

c_timeout::c_timeout(uint32_t* p_duration) {
   f_setDuration(p_duration);
}

void c_timeout::f_setDuration(uint32_t n_duration) {
  n_durationLong = n_duration;
  f_setDuration(&n_durationLong);
}

void c_timeout::f_setDuration(uint32_t* p_duration) {
  p_durationLong = p_duration;
  p_durationShort = NULL;
}

void c_timeout::f_setDuration(uint16_t* p_duration) {
  p_durationShort = p_duration;
  p_durationLong = NULL;
}

void c_timeout::f_start() {
  if (p_durationLong != NULL && *p_durationLong) {
    n_durationLong = *p_durationLong;
  }
  else if (p_durationShort != NULL && *p_durationShort) {
    n_durationLong = (uint32_t)*p_durationShort;
  }
  else {
    f_stop();
    return;
  }
  b_running = TRUE;
  n_timerStart = millis();
  n_timerEnd = n_timerStart + n_durationLong;
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
