#ifndef RTC_clock_h
#define RTC_clock_h

#include "Arduino.h"

// Includes Atmel CMSIS
#include <chip.h>

#define SUPC_KEY     0xA5u
#define RESET_VALUE  0x01210720

#define RC           0
#define	XTAL         1
/*
// Unixtimeseconds from 1. Januar 1970  00:00:00 to 1. Januar 2000   00:00:00 UTC-0
#define SECONDS_FROM_1970_TO_2000 946684800
*/
#define SECONDS_PER_HOUR 3600

#define UTC 0
#define Germany 2000

#define CET  1   //MEZ
#define CEST 2   //MESZ

class RTC_clock
{
	public:
		RTC_clock (int source);
		void init ();
		void set_time (int hour, int minute, int second);
		void set_time (char* time);
		int get_hours ();
		int get_minutes ();
		int get_seconds ();
		void set_date (int day, int month, uint16_t year);
		void set_date (char* date);
		void set_clock (char* date, char* time);
		void set_clock (unsigned long timestamp, int timezone = 0);
		uint16_t get_years ();
		int get_months ();
		int date_already_set ();
		int get_days ();
		int get_day_of_week ();
		uint32_t get_valid_entry ();
		int calculate_day_of_week (uint16_t _year, int _month, int _day);
		int set_hours (int _hour);
		int set_minutes (int minute);
		int set_seconds (int second);
		int set_days (int day);
		int set_months (int month);
		int set_years (uint16_t year);
		void set_alarmtime (int hour, int minute, int second);
		void set_alarmdate (int month, int day);

		void attachalarm (void (*)(void));
		uint32_t unixtime (int timezone = 0);
		void get_time (int *hour, int *minute, int *second);
		void get_date (int *day_of_week, int *day, int *month, int *year);
		//int switch_years (uint16_t year);
		int summertime ();
		int UTC_abbreviation();
		void dst_followup();

	private:
//		int _source;
		int _hour;
		int _minute;
		int _second;
		int _day;
		int _month;
		uint16_t _year;
		int _day_of_week;
		int timezoneadjustment (int timezone);
//		int _abbreviation;
		uint32_t current_time ();
		uint32_t current_date ();
		uint32_t _current_time;
		uint32_t _current_date;
		uint32_t change_time (uint32_t _now);
		uint32_t change_date (uint32_t _now);
		uint32_t _now;
		uint32_t _changed;
		bool dst_winter_done = false;
};

#endif
