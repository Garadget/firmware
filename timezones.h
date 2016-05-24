// $Id$
/**
 * @file timezones.h
 * @brief Implements DST aware time zone adjustment
 * @author Denis Grisak
 * @version 1.6
 */
// $Log$

#ifndef TIMEZONES_H
#define TIMEZONES_H

#include "application.h"
#define SECS_PER_DAY 24 * 60 * 60;

class c_timezones {

  // for reference only
/*  enum weekNum { Last = 0, First, Second, Third, Fourth };
  enum weekDay { Sun = 0, Mon, Tue, Wed, Thu, Fri, Sat };
  enum monthNum { Jan = 1, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec };*/

public:

  typedef struct {
    uint8_t n_weekNum;
    uint8_t n_weekDay;
    uint8_t n_month;
    uint8_t n_hour;
    int16_t n_offset;
  } dstRule_t;

  /**
   * Class constructor - adjusts system's timezone accoring to provided DST rule structure
   * @param[in] s_timeZone timezone and DST rules string
   */
  c_timezones(const char* s_timeZone);

  /**
   * Sets DST rules from the string
   * @param[in] s_rule String representation of DST rule
   * @return true if parsed successfully
   */
  bool f_setConfig(const String s_rule);

  /**
   * Updates system's time based on DST rules loaded at initialization
   * @return offset from UTC in minutes
   */
  int16_t f_getOffset();

  /**
   * Updates system's time based on DST rules loaded at initialization
   */
  void f_updateTimezone();

protected:
  dstRule_t a_dstRuleOff;
  dstRule_t a_dstRuleOn;
  int16_t n_prevOffset = 0x07FF;

  /**
   * Parses string representation of DST rule into structure
   * @param[in] s_rule DST rule string
   * @param[out] a_result reference to resulting DST rule structure
   * @return true if parsed successfully
   */
  bool f_parseRule(const String s_rule, dstRule_t* a_result);

  /**
   * Determines unix time for the DST time switch for current year
   * @param[in] s_rule DST rule for switch
   * @return unix time of the switch
   */
  time_t f_switchTime(dstRule_t* a_dstRule);
};

#endif
