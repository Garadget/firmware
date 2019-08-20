// $Id$
/**
 * @file timezones.h
 * @brief Implements DST aware time zone adjustment
 * @author Denis Grisak
 * @version 1.20
 */
// $Log$

#include "timezones.h"

/** constructor */
c_timezones::c_timezones() {
}

// For areas with DST - 2 DST rules, example: 10112-7.00,20032-6.00
// For areas without DST - one float for offset, example -7.00
bool c_timezones::f_setConfig(const String s_dstRules) {

  uint8_t s_start = 0;
  int8_t n_end = s_dstRules.indexOf(',');

  if (n_end == -1) {
    a_dstRuleOff.n_offset = (int)(s_dstRules.toFloat() * 60);
    a_dstRuleOff.n_month = 0;
    f_updateTimezone();
    return true;
  }
  String s_dtsRule = s_dstRules.substring(0, n_end);
  if (!f_parseRule(s_dtsRule, &a_dstRuleOff))
    return false;

  s_start = n_end + 1;
  n_end = s_dstRules.indexOf(',', s_start);
  s_dtsRule = n_end == -1
    ? s_dstRules.substring(s_start)
    : s_dstRules.substring(s_start, n_end);
  if (!f_parseRule(s_dtsRule, &a_dstRuleOn))
    return false;

  f_updateTimezone();
  return true;
}

//  example 20032-6 : second (2) sunday (0) in march(03) at 2am (2) offset -6hrs (-6)
bool c_timezones::f_parseRule(const String s_dtsRule, dstRule_t* a_result) {

  a_result->n_weekNum = s_dtsRule.substring(0, 1).toInt();
  if (a_result->n_weekNum > 4)
    return false;

  a_result->n_weekDay = s_dtsRule.substring(1, 2).toInt();
  if (a_result->n_weekDay > 6)
    return false;

  a_result->n_month = s_dtsRule.substring(2, 4).toInt();
  if (a_result->n_month < 1 || a_result->n_month > 12)
    return false;

  a_result->n_hour = s_dtsRule.substring(4, 5).toInt();

  a_result->n_offset = (int)(s_dtsRule.substring(5).toFloat() * 60);
  if (a_result->n_offset < -780 || a_result->n_offset > 780)
    return false;

  return true;
}

time_t c_timezones::f_switchTime(dstRule_t* a_dstRule) {
  uint8_t n_year = Time.year() - 1900;
  uint8_t n_month = a_dstRule->n_month - 1;
  uint8_t n_weekNum = a_dstRule->n_weekNum;
  struct tm a_time;

  // handle rule for last week
  if (n_weekNum == 0) {
    if (++n_month > 11) {
      n_month = 0;
      n_year++;
    }
    n_weekNum = 1;
  }

  a_time.tm_year = n_year;
  a_time.tm_mon = n_month;
  a_time.tm_mday = 1;
  a_time.tm_hour = a_dstRule->n_hour;
  a_time.tm_min = 0;
  a_time.tm_sec = 0;
  a_time.tm_isdst = 0;
  time_t n_time = mktime(&a_time);

  n_time += (
    7 * (n_weekNum - 1) +
    (a_dstRule->n_weekDay - Time.weekday(n_time) + 8) % 7
  ) * SECS_PER_DAY;
  if (a_dstRule->n_weekNum == 0)
    n_time -= 7 * SECS_PER_DAY;
  return n_time;
}

int16_t c_timezones::f_getOffset() {

  // no DST rules
  if (!a_dstRuleOff.n_month)
    return a_dstRuleOff.n_offset;

  // fast procedure
  uint8_t n_month = Time.month();
  if (n_month < a_dstRuleOn.n_month || n_month > a_dstRuleOff.n_month)
    return a_dstRuleOff.n_offset;
  if (n_month > a_dstRuleOn.n_month && n_month < a_dstRuleOff.n_month)
    return a_dstRuleOn.n_offset;

  // long procedure
  Time.zone(0);
  time_t n_now = Time.now();
  time_t n_timeOn = f_switchTime(&a_dstRuleOn) - a_dstRuleOff.n_offset * 60;
  if (n_now <= n_timeOn)
    return a_dstRuleOff.n_offset;

  time_t n_timeOff = f_switchTime(&a_dstRuleOff) - a_dstRuleOn.n_offset * 60;
  if (n_now < n_timeOff)
    return a_dstRuleOn.n_offset;
  return a_dstRuleOff.n_offset;
}

void c_timezones::f_updateTimezone() {
  Time.zone(f_getOffset() / 60);
}
