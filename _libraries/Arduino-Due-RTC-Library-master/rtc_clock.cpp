#include "rtc_clock.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Arduino.h"

uint8_t daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
uint8_t meztime;

// Based on https://github.com/PaulStoffregen/Time.cpp
// for 4 digit years
#define switch_years(Y) ( !(Y % 4) && ( (Y % 100) || !(Y % 400) ) )

RTC_clock::RTC_clock (int source)
{
//  _source = source;
  if (source) {
    pmc_switch_sclk_to_32kxtal(0);
	
	while (!pmc_osc_is_ready_32kxtal());
  }
}

void RTC_clock::init ()
{
  RTC_SetHourMode(RTC, 0);
  
  NVIC_DisableIRQ(RTC_IRQn);
  NVIC_ClearPendingIRQ(RTC_IRQn);
  NVIC_SetPriority(RTC_IRQn, 0);
//  NVIC_EnableIRQ(RTC_IRQn);
//  RTC_EnableIt(RTC, RTC_IER_SECEN | RTC_IER_ALREN);
//  RTC_EnableIt(RTC, RTC_IER_SECEN);
}

void RTC_clock::set_time (int hour, int minute, int second)
{
  _hour = hour;
  _minute = minute;
  _second = second;
	
  RTC_SetTime (RTC, _hour, _minute, _second);
}

/*
// Based on https://github.com/adafruit/RTClib/blob/master/RTClib.cpp
int conv2d(char* p)
{
  int v = 0;
  if ('0' <= *p && *p <= '9')
  	v = *p - '0';
	
  return 10 * v + *++p - '0';
}
*/

// Based on http://www.geeksforgeeks.org/write-your-own-atoi/
// Better version converts until none number shows up
int conv2d(char* p)
{
  int v = 0; // Initialize result
  
  // Iterate through all characters of input string and update result
  for (int i = 0; p[i] != '\0'; ++i) {
    if ('0' <= p[i] && p[i] <= '9')
      v = v*10 + p[i] - '0';
    else
      break;
  }
  // return result.
  return v;
}

void RTC_clock::set_time (char* time)
{
  _hour   = conv2d(time);
  _minute = conv2d(time + 3);
  _second = conv2d(time + 6);
  
  RTC_SetTime (RTC, _hour, _minute, _second);
}

// Based on https://github.com/PaulStoffregen/Time.cpp
void RTC_clock::set_clock (unsigned long timestamp, int timezone)
{
  int monthLength;
  unsigned long time, days;
  
  // Sunday, 01-Jan-40 00:00:00 UTC 70 years after the beginning of the unix timestamp so
  // if the timestamp bigger than this the "offset" will automatic removed
  if (timestamp >= 2208988800UL)
    time -= 2208988800UL;
  
  time = timestamp + (unsigned long)timezoneadjustment(timezone);
  _second = time % 60;
  time /= 60; // now it is minutes
  _minute = time % 60;
  time /= 60; // now it is hours
  _hour = time % 24;
  time /= 24; // now it is days
  
  _year = 1970;
  days = 0;
  while(( days += (365 + switch_years(_year)) ) <= time)
    _year++;
  
  days -= (365 + switch_years(_year));
  
  time -= days; // now it is days in this year, starting at 0
  
  days = 0;
  for (_month = 0; _month < 12; _month++) {
    if (_month == 1) // february
      monthLength = daysInMonth[_month] + switch_years(_year);
    else
      monthLength = daysInMonth[_month];
    
    if (time >= monthLength)
      time -= monthLength;
    else
      break;
  }
  
  _month++;  								// jan is month 1  
  _day = (int)time + 1;     // day of month
  _day_of_week = calculate_day_of_week (_year, _month, _day);
  
  RTC_SetTime (RTC, _hour, _minute, _second);
  RTC_SetDate (RTC, (uint16_t)_year, (uint8_t)_month, (uint8_t)_day, (uint8_t)_day_of_week);
}

uint32_t RTC_clock::current_time ()
{
  uint32_t dwTime;
  
  /* Get current RTC time */
  dwTime = RTC->RTC_TIMR ;
  while ( dwTime != RTC->RTC_TIMR ) {
    dwTime = RTC->RTC_TIMR ;
  }
  
  return (dwTime);
}

void RTC_clock::get_time (int *hour, int *minute, int *second)
{
  RTC_GetTime(RTC, (uint8_t*)hour, (uint8_t*)minute, (uint8_t*)second);
}

int RTC_clock::get_hours ()
{
  _current_time = current_time();
  
  return (((_current_time & 0x00300000) >> 20) * 10 + ((_current_time & 0x000F0000) >> 16));
//  return RTC_TIMR_HOUR ( current_time() );
}

int RTC_clock::get_minutes ()
{
  _current_time = current_time();
  
  return (((_current_time & 0x00007000) >> 12) * 10 + ((_current_time & 0x00000F00) >> 8));
//  return RTC_TIMR_MIN ( current_time() );
}

int RTC_clock::get_seconds ()
{
  _current_time = current_time();
  
  return (((_current_time & 0x00000070) >> 4) * 10 + ((_current_time & 0x0000000F)));
//  return RTC_TIMR_SEC ( current_time() );
}

/**
 * \brief Calculate day_of_week from year, month, day.
 * Based on SAM3X rtc_example.c from Atmel Software Framework (Real-Time Clock (RTC) example for SAM) also available
 * https://github.com/eewiki/asf/blob/master/sam/drivers/rtc/example/rtc_example.c
 */
int RTC_clock::calculate_day_of_week (uint16_t _year, int _month, int _day)
{
  int _week;
  
  if (_month == 1 || _month == 2) {
    _month += 12;
    --_year;
  }
  
  _week = (_day + 2 * _month + 3 * (_month + 1) / 5 + _year + _year / 4 - _year / 100 + _year / 400) % 7;
  
  ++_week;
  
  return _week;
}

void RTC_clock::set_date (int day, int month, uint16_t year)
{
  _day   = day;
  _month = month;
  _year  = year;
  _day_of_week = calculate_day_of_week(_year, _month, _day);
  
  RTC_SetDate (RTC, (uint16_t)_year, (uint8_t)_month, (uint8_t)_day, (uint8_t)_day_of_week);
}

// Based on https://github.com/adafruit/RTClib/blob/master/RTClib.cpp
void RTC_clock::set_date (char* date)
{
  _day = conv2d(date + 4);
  
  //Month
  switch (date[0]) {
    case 'J': _month = date[1] == 'a' ? 1 : _month = date[2] == 'n' ? 6 : 7; break;
    case 'F': _month =  2; break;
    case 'A': _month = date[2] == 'r' ? 4 : 8; break;
    case 'M': _month = date[2] == 'r' ? 3 : 5; break;
    case 'S': _month =  9; break;
    case 'O': _month = 10; break;
    case 'N': _month = 11; break;
    case 'D': _month = 12; break;
  }
  
  _year = conv2d(date + 9);
  _day_of_week = calculate_day_of_week(_year, _month, _day);
  
  RTC_SetDate (RTC, (uint16_t)_year, (uint8_t)_month, (uint8_t)_day, (uint8_t)_day_of_week);
}

uint32_t RTC_clock::current_date ()
{
  uint32_t dwTime;
  
  /* Get current RTC date */
  dwTime = RTC->RTC_CALR ;
  while ( dwTime != RTC->RTC_CALR ) {
    dwTime = RTC->RTC_CALR ;
  }
  
  return (dwTime);
}

int RTC_clock::date_already_set ()
{
  uint32_t dateregister;
  
  /* Get current RTC date */
  dateregister = current_date ();
  
  if ( RESET_VALUE != dateregister ) {
    return 1;
  } else {
    return 0;
  }
}

void RTC_clock::get_date (int *day_of_week, int *day, int *month, int *year)
{
  RTC_GetDate(RTC, (uint16_t*)year, (uint8_t*)month, (uint8_t*)day, (uint8_t*)day_of_week);
}

uint16_t RTC_clock::get_years ()
{
  _current_date = current_date();
  
  return ((((_current_date >> 4) & 0x7) * 1000) + ((_current_date & 0xF) * 100)
  						+ (((_current_date >> 12) & 0xF) * 10) + ((_current_date >> 8) & 0xF));
//  return ( RTC_CALR_CENT ( current_date() ) * 100 + RTC_CALR_YEAR ( current_date() ) );
}

int RTC_clock::get_months ()
{
  _current_date = current_date();
  
  return ((((_current_date >> 20) & 1) * 10) + ((_current_date >> 16) & 0xF));
//  return RTC_CALR_MONTH ( current_date() );
}

int RTC_clock::get_days ()
{
  _current_date = current_date();
  
  return ((((_current_date >> 28) & 0x3) * 10) + ((_current_date >> 24) & 0xF));
//  return RTC_CALR_DATE ( current_date() );
}

int RTC_clock::get_day_of_week ()
{
  _current_date = current_date();
  
  return (((_current_date >> 21) & 0x7));
//  return RTC_CALR_DAY ( current_date() );
}

uint32_t RTC_clock::get_valid_entry ()
{
	return (RTC->RTC_VER);
}

int RTC_clock::set_hours (int hour)
{
  _hour = hour;
  uint32_t _current_time = current_time();
  uint32_t _changed;
  
  _changed = ((_hour%10) | ((_hour/10)<<4))<<16;
  
  _current_time = (_current_time & 0xFFC0FFFF) ^ _changed;
  
  change_time(_current_time);
}

int RTC_clock::set_minutes (int minute)
{
  _minute = minute;
  uint32_t _current_time = current_time();
  uint32_t _changed;
  
  _changed = ((_minute%10) | ((_minute/10)<<4))<<8;
  
  _current_time = (_current_time & 0xFFFF80FF) ^ _changed;
  
  change_time(_current_time);
}

int RTC_clock::set_seconds (int second)
{
  _second = second;
  uint32_t _current_time = current_time();
  uint32_t _changed;
  
  _changed = ((_second%10) | ((_second/10)<<4));
  
  _current_time = (_current_time & 0xFFFFFF80) ^ _changed;
  
  change_time(_current_time);
}

uint32_t RTC_clock::change_time (uint32_t now)
{
  _now = now;
  
  RTC->RTC_CR |= RTC_CR_UPDTIM ;
  while ((RTC->RTC_SR & RTC_SR_ACKUPD) != RTC_SR_ACKUPD);
  
  RTC->RTC_SCCR = RTC_SCCR_ACKCLR;
  RTC->RTC_TIMR = _now;
  RTC->RTC_CR &= (uint32_t)(~RTC_CR_UPDTIM);
  RTC->RTC_SCCR |= RTC_SCCR_SECCLR;
  
  return (int)(RTC->RTC_VER & RTC_VER_NVTIM);
}

int RTC_clock::set_days (int day)
{
  _day = day;
  uint32_t _current_date = current_date();
  uint32_t _changed;
  
  _day_of_week = calculate_day_of_week(get_years(), get_months(), _day);
  _day_of_week = ((_day_of_week%10) | (_day_of_week/10)<<4)<<21;
  
  _changed = ((_day%10) | (_day/10)<<4)<<24;
  
  _current_date = (_current_date & (0xC0FFFFFF & 0xFF1FFFFF) ) ^ ( _changed | _day_of_week );
  
  change_date(_current_date);
}

int RTC_clock::set_months (int month)
{
  _month = month;
  uint32_t _current_date = current_date();
  uint32_t _changed;
  
  _day_of_week = calculate_day_of_week(get_years(), _month, get_days());
  _day_of_week = ((_day_of_week%10) | (_day_of_week/10)<<4)<<21;
  
  _changed = ((_month%10) | (_month/10)<<4)<<16;
  
  _current_date = (_current_date & (0xFFE0FFFF & 0xFF1FFFFF) ) ^ ( _changed | _day_of_week );
  
  change_date(_current_date);
}

int RTC_clock::set_years (uint16_t year)
{
  _year = year;
  uint32_t _current_date = current_date();
  uint32_t _changed;
  
  _day_of_week = calculate_day_of_week(_year, get_months(), get_days());
  _day_of_week = ((_day_of_week%10) | (_day_of_week/10)<<4)<<21;
  
  _changed = (((_year/100)%10) | ((_year/1000)<<4)) | ((_year%10) | (((_year/10)%10))<<4)<<8;
  
  _current_date = (_current_date & (0xFFFF0080 & 0xFF1FFFFF) ) ^ ( _changed | _day_of_week );
  
  change_date(_current_date);
}

uint32_t RTC_clock::change_date (uint32_t now)
{
  _now = now;
  
  RTC->RTC_CR |= RTC_CR_UPDCAL;
  while ((RTC->RTC_SR & RTC_SR_ACKUPD) != RTC_SR_ACKUPD);
  
  RTC->RTC_SCCR = RTC_SCCR_ACKCLR;
  RTC->RTC_CALR = _now;
  RTC->RTC_CR &= (uint32_t)(~RTC_CR_UPDCAL);
  RTC->RTC_SCCR |= RTC_SCCR_SECCLR;
  
  return (int)(RTC->RTC_VER & RTC_VER_NVCAL);
}

// Based on https://github.com/adafruit/RTClib/blob/master/RTClib.cpp
void RTC_clock::set_clock (char* date, char* time)
{
  set_date(date);
  set_time(time);
}

int RTC_clock::UTC_abbreviation ()
{
  if ( summertime () )
    return CEST;
  else
    return CET;
}

void (*useralarmFunc)(void);

void RTC_clock::attachalarm(void (*userFunction)(void))
{
  useralarmFunc = userFunction;
}

void RTC_Handler(void)
{
  uint32_t status = RTC->RTC_SR;
  
  /* Time or date alarm */
  if ((status & RTC_SR_ALARM) == RTC_SR_ALARM) {
    /* Disable RTC interrupt */
    RTC_DisableIt(RTC, RTC_IDR_ALRDIS);
    
    /* Execute function */
    useralarmFunc();
    
    /* Clear notification */
    RTC_ClearSCCR(RTC, RTC_SCCR_ALRCLR);
    RTC_EnableIt(RTC, RTC_IER_ALREN);
  }
}

void RTC_clock::set_alarmtime (int hour, int minute, int second)
{
  uint8_t _hour = hour;
  uint8_t _minute = minute;
  uint8_t _second = second;
  
  RTC_EnableIt(RTC, RTC_IER_ALREN);
  RTC_SetTimeAlarm(RTC, &_hour, &_minute, &_second);
  NVIC_EnableIRQ(RTC_IRQn);
}

void RTC_clock::set_alarmdate (int month, int day)
{
  uint8_t _month = month;
  uint8_t _day = day;
  
  RTC_EnableIt(RTC, RTC_IER_ALREN);
  RTC_SetDateAlarm(RTC, &_month, &_day);
  NVIC_EnableIRQ(RTC_IRQn);
}

uint32_t RTC_clock::unixtime(int timezone)
{
  uint32_t _ticks;
  uint16_t _days;
  _current_date = current_date();
  _current_time = current_time();
  
  _second = (((_current_time & 0x00000070) >>  4) * 10 + ((_current_time & 0x0000000F)));
  _minute = (((_current_time & 0x00007000) >> 12) * 10 + ((_current_time & 0x00000F00) >> 8));
  _hour   = (((_current_time & 0x00300000) >> 20) * 10 + ((_current_time & 0x000F0000) >> 16));
  
  _day    = ((((_current_date >> 28) & 0x3) *   10) + ((_current_date >> 24) & 0xF));
  //_day_of_week = ((_current_date >> 21) & 0x7);
  _month  = ((((_current_date >> 20) &   1) *   10) + ((_current_date >> 16) & 0xF));
	
  //_year 4 digits
  _year   = ((((_current_date >>  4) & 0x7) * 1000) + ((_current_date & 0xF) * 100)
  						+ (((_current_date >> 12) & 0xF) * 10) + ((_current_date >> 8) & 0xF));
  
  //_year 2 digits
  //_year   = (((_current_date >> 12) & 0xF) * 10) + ((_current_date >> 8) & 0xF);
  
  // Based on https://github.com/punkiller/workspace/blob/master/string2UnixTimeStamp.cpp
  // days of the years between start of unixtime and now
  _days = 365 * (_year - 1970);
  
  // add days from switch years in between except year from date
  for( int i = 1970; i < _year ; i++){
    if( switch_years (_year) ) {
      _days++;
    }
  }
  
  // Based on https://github.com/adafruit/RTClib/blob/master/RTClib.cpp
  for (int i = 1; i < _month; ++i)
    _days += daysInMonth[i - 1];
  
  if ( _month > 2 && switch_years (_year) )
    ++_days;
  
  _days += _day - 1;
  
  _ticks = ((_days * 24 + _hour) * 60 + _minute) * 60 + _second;
  
  _ticks = _ticks - (int)timezoneadjustment(timezone);
  
  return _ticks;
}

int RTC_clock::timezoneadjustment (int timezone)
{
  float adjustment;
  
  if (timezone == Germany)
    timezone = 1 + summertime();
  
  switch (timezone) {
    case -12:
            adjustment = -12    * SECONDS_PER_HOUR; break;
    case -11:
            adjustment = -11    * SECONDS_PER_HOUR; break;
    case -10:
	    adjustment = -10    * SECONDS_PER_HOUR; break;
    case -930:
	    adjustment =  -9.5  * SECONDS_PER_HOUR; break;
    case -9:
	    adjustment =  -9    * SECONDS_PER_HOUR; break;
    case -8:
	    adjustment =  -8    * SECONDS_PER_HOUR; break;
    case -7:
	    adjustment =  -7    * SECONDS_PER_HOUR; break;
    case -6:
	    adjustment =  -6    * SECONDS_PER_HOUR; break;
    case -5:
	    adjustment =  -5    * SECONDS_PER_HOUR; break;
    case -4:
	    adjustment =  -4    * SECONDS_PER_HOUR; break;
    case -330:
	    adjustment =  -3.5  * SECONDS_PER_HOUR; break;
    case -3:
	    adjustment =  -3    * SECONDS_PER_HOUR; break;
    case -2:
	    adjustment =  -2    * SECONDS_PER_HOUR; break;
    case -1:
	    adjustment =  -1    * SECONDS_PER_HOUR; break;
    case 0:
    default:
	    adjustment =   0; break;
    case 1:
	    adjustment =   1    * SECONDS_PER_HOUR; break;
    case 2:
	    adjustment =   2    * SECONDS_PER_HOUR; break;
    case 3:
	    adjustment =   3    * SECONDS_PER_HOUR; break;
    case 330:
	    adjustment =   3.5  * SECONDS_PER_HOUR; break;
    case 4:
	    adjustment =   4    * SECONDS_PER_HOUR; break;
    case 430:
	    adjustment =   4.5  * SECONDS_PER_HOUR; break;
    case 5:
	    adjustment =   5    * SECONDS_PER_HOUR; break;
    case 530:
	    adjustment =   5.5  * SECONDS_PER_HOUR; break;
    case 545:
	    adjustment =   5.75 * SECONDS_PER_HOUR; break;
    case 6:
	    adjustment =   6    * SECONDS_PER_HOUR; break;
    case 630:
	    adjustment =   6.5  * SECONDS_PER_HOUR; break;
    case 7:
	    adjustment =   7    * SECONDS_PER_HOUR; break;
    case 8:
	    adjustment =   8    * SECONDS_PER_HOUR; break;
    case 845:
	    adjustment =   8.75 * SECONDS_PER_HOUR; break;
    case 9:
	    adjustment =   9    * SECONDS_PER_HOUR; break;
    case 930:
	    adjustment =   9.5  * SECONDS_PER_HOUR; break;
    case 10:
	    adjustment =  10    * SECONDS_PER_HOUR; break;	
    case 1030:
	    adjustment =  10.5  * SECONDS_PER_HOUR; break;
    case 11:
	    adjustment =  11    * SECONDS_PER_HOUR; break;
    case 1130:
	    adjustment =  11.5  * SECONDS_PER_HOUR; break;
    case 12:
	    adjustment =  12    * SECONDS_PER_HOUR; break;
    case 1245:
	    adjustment =  12.75 * SECONDS_PER_HOUR; break;
    case 13:
	    adjustment =  13    * SECONDS_PER_HOUR; break;
    case 14:
	    adjustment =  14    * SECONDS_PER_HOUR; break;
  }
  
  return adjustment;
}

/* 
int RTC_clock::switch_years (uint16_t year)
{
	if ( ((year %4 == 0) && (year % 100 != 0)) || (year % 400 == 0) ) {
		return 1;
	} else {
		return 0;
	}
}
 */

int RTC_clock::summertime ()
{
  int sundaysommertime, sundaywintertime, today, sundaysommertimehours, sundaywintertimehours, todayhours;
  
  _current_date = current_date();
  _current_time = current_time();
  
  _hour   = (((_current_time & 0x00300000) >> 20) * 10 + ((_current_time & 0x000F0000) >> 16));
  _day    = ((((_current_date >> 28) & 0x3) *   10) + ((_current_date >> 24) & 0xF));
  _month  = ((((_current_date >> 20) &   1) *   10) + ((_current_date >> 16) & 0xF));
  //_year 4 digits
  _year   = ((((_current_date >>  4) & 0x7) * 1000) + ((_current_date & 0xF) * 100)
  						+ (((_current_date >> 12) & 0xF) * 10) + ((_current_date >> 8) & 0xF));
  
  //_year 2 digits
  //_year   = (((_current_date >> 12) & 0xF) * 10) + ((_current_date >> 8) & 0xF);
  
  // Based on http://www.webexhibits.org/daylightsaving/i.html
  // Equations by Wei-Hwa Huang (US), and Robert H. van Gent (EC)
  // Slightly modified for use in micro controller for integer use
  // also found there http://manfred.wilzeck.de/Datum_berechnen.html#Auch_mit_Osterdatum_berechnen
  // Number 5 (Works for the Years 2000 - 2099)
  sundaysommertime = 31 - ( 5 + _year * 5 / 4 ) % 7;
  sundaywintertime = 31 - ( 2 + _year * 5 / 4 ) % 7;
  today = _day;
  
  // Summertime begin in March
  for (int i = 1; i < 2; ++i) {
    if ( (i - 1) == 1)
      sundaysommertime += daysInMonth[i - 1] + switch_years(_year);
    else
      sundaysommertime += daysInMonth[i - 1];
  }
  
  // Wintertime begin in October
  for (int i = 1; i < 9; ++i) {
    if ( (i - 1) == 1)
      sundaywintertime += daysInMonth[i - 1] + switch_years(_year);
    else
      sundaywintertime += daysInMonth[i - 1];
  }
  
  // Total actually days
  for (int i = 1; i < (_month - 1); ++i) {
    if ( (i - 1) == 1)
      today += daysInMonth[i - 1] + switch_years(_year);
    else
      today += daysInMonth[i - 1];
  }
  
  sundaysommertimehours = sundaysommertime * 24 + 2;
  sundaywintertimehours = sundaywintertime * 24 + 3;
  todayhours = today * 24 + _hour;
  
  if ( todayhours >= sundaysommertimehours && (todayhours + 1) < sundaywintertimehours )
    return 1;
  else
    return 0;
}

void RTC_clock::dst_followup ()
{
  int sundaysommertime, sundaywintertime;
  
  _current_date = current_date();
  
  //_year 4 digits
  _year   = ((((_current_date >>  4) & 0x7) * 1000) + ((_current_date & 0xF) * 100)
  						+ (((_current_date >> 12) & 0xF) * 10) + ((_current_date >> 8) & 0xF));
  
  //_year 2 digits
  //_year   = (((_current_date >> 12) & 0xF) * 10) + ((_current_date >> 8) & 0xF);
  
  sundaysommertime = 31 - ( 5 + _year * 5 / 4 ) % 7;
  sundaywintertime = 31 - ( 2 + _year * 5 / 4 ) % 7;
  
  if (get_months () == 3 && get_days () == sundaysommertime) {
    if (get_hours () == 2 && get_minutes () == 0 && get_seconds () == 0) {
      set_hours (3);
      dst_winter_done = false;
    }
  }
  
  if (!dst_winter_done) {
    if (get_months () == 10 && get_days () == sundaywintertime ) {
      if (get_hours () == 3 && get_minutes () == 0 && get_seconds () == 0) {
        set_hours (2);
        dst_winter_done = true;
      }
    }
  }
}
