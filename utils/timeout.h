// $Id$
/**
 * @file timeout.h
 * @brief Provides timer related functions to facilitate non-breaking delays
 * @author Denis Grisak
 * @version 1.19
 */
// $Log$

#ifndef TIMEOUT_H
#define TIMEOUT_H

#include "application.h"

class c_timeout {

  protected:
    uint16_t n_duration = 0;
    uint16_t *p_duration;
    uint32_t n_timerStart;
    uint32_t n_timerEnd;
    boolean b_running = FALSE;

  public:

/**
 * Timeout constructor
 * @param[in] uint16_t n_initialDuration Optional initial timer duration in millisecods
 */
    c_timeout(uint16_t n_initialDuration = 0);

/**
 * Timeout constructor
 * @param[in] uint16_t* p_newDuration Pointer to variable containing timer duraction in
 *  milliseconds
 */
    c_timeout(uint16_t* p_newDuration);

/**
 * Sets duration for the timeout. The duration is preserved across the timer
 *  runs until changed.
 * @param[in] uint16_t n_newDuration Timer duration in milliseconds
 */
    void f_setDuration(uint16_t n_newDuration);

/**
 * References external timer duration variable. This is useful when duration
 *  parameter is stored in external configuration structure
 * @param[in] uint16_t* p_newDuration Pointer to variable containing timer duraction in
 *  milliseconds
 */
    void f_setDuration(uint16_t *p_newDuration);

/**
 * Starts or re-starts the timer
 */
    void f_start();

/**
 * Cancels the timer without generating any events
 * @param[in] p_newDuration Pointer to variable containing timer duraction in
 *  milliseconds
 * @return Description of returned value.
 */
    void f_stop();

/**
 * Reports current status of the timer
 * Useful for the processes that have to continue as long as the timer is
 * running. Either this method or @see f_isTimeout() must be called periodically
 * to determine when to trigger timed events.
 * @return Status of the timer: TRUE for running, FALSE for timed out or stopped
 */
    boolean f_isRunning();

/**
 * Reports when timer transitions from running to stopped
 * Useful for the processed which have to be triggered one time at the end
 * of the delay. Either this method or @see f_isRunning() must be called
 * periodically to check the status of the timer.
 * @return Returns TRUE one time after timer ran out
 */
    boolean f_isTimeout();

/**
 * Reports remaining time
 * @return Remaining time in milliseconds or zero if timer is not running
 */
    uint32_t f_timeLeft();
};

#endif
