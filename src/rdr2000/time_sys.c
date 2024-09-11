/*===--------------------------------------------------------------------------------------------===
 * time_sys.c
 *
 * Created by Amy Parent <amy@amyparent.com>
 * Copyright (c) 2024 Laminar Research. All rights reserved
 *
 * Licensed under the MIT License
 *===--------------------------------------------------------------------------------------------===
*/
#include "time_sys.h"
#include "xplane.h"
#include <math.h>
#if	IBM
#include <windows.h>
#else	/* !IBM */
#include <sys/time.h>
#endif	/* !IBM */
#include <stdlib.h>
#include <time.h>
#include <helpers/helpers.h>
#include <XPLMProcessing.h>

static XPLMDataRef  dr_time_days_local; 
static XPLMDataRef  dr_time_sec_local;
static XPLMDataRef  dr_time_sec_zulu;
static XPLMDataRef  dr_running_time;

static bool         is_inited = false;
static double       last_update = 0;
static double       dt = 0;
static double       sim_utc = 0;

/*
 * lacf_gmtime_r from Saso Kiselkov's libacfutils
 *
 * CDDL HEADER START
 *
 * This file and its contents are supplied under the terms of the
 * Common Development and Distribution License ("CDDL"), version 1.0.
 * You may only use this file in accordance with the terms of version
 * 1.0 of the CDDL.
 *
 * A full copy of the text of the CDDL should have accompanied this
 * source.  A copy of the CDDL is also available via the Internet at
 * http://www.illumos.org/license/CDDL.
 *
 * CDDL HEADER END
*/
/*
 * Copyright 2017 Saso Kiselkov. All rights reserved.
 */
static inline int32_t
is_leap(int32_t year)
{
	if (year % 400 == 0)
		return (1);
	if (year % 100 == 0)
		return (0);
	if (year % 4 == 0)
		return (1);
	return (0);
}

static inline int32_t
days_from_0(int32_t year)
{
	year--;
	return (365 * year + (year / 400) - (year / 100) + (year / 4));
}

static inline int32_t
days_from_1970(int32_t year)
{
	int days_from_0_to_1970 = days_from_0(1970);
	return (days_from_0(year) - days_from_0_to_1970);
}

static inline int32_t
days_from_1jan(int32_t year, int32_t month, int32_t day)
{
	static const int32_t days[2][12] = {
	    {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},
	    {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335}
	};
	return (days[is_leap(year)][month - 1] + day - 1);
}

time_t
lacf_timegm(const struct tm *t)
{
	int year = t->tm_year + 1900;
	int month = t->tm_mon;
	time_t seconds_in_day, result;
	int day_of_year, days_since_epoch;

	if (month > 11) {
		year += month / 12;
		month %= 12;
	} else if (month < 0) {
		int years_diff = (-month + 11) / 12;
		year -= years_diff;
		month += 12 * years_diff;
	}
	month++;
	if (t->tm_mday != 0) {
		int day = t->tm_mday;
		day_of_year = days_from_1jan(year, month, day);
	} else {
		day_of_year = t->tm_yday;
	}
	days_since_epoch = days_from_1970(year) + day_of_year;

	seconds_in_day = 3600 * 24;
	result = seconds_in_day * days_since_epoch + 3600 * t->tm_hour +
	    60 * t->tm_min + t->tm_sec;

	return (result);
}


static inline bool
lacf_gmtime_r(const time_t *tim, struct tm *tm)
{
#if	defined(__STDC_LIB_EXT1__) || IBM
	return (_gmtime64_s(tm, tim) == 0);
#else	/* !defined(__STDC_LIB_EXT1__) && !IBM */
	return (gmtime_r(tim, tm) != NULL);
#endif	/* !defined(__STDC_LIB_EXT1__) && !IBM */
}


static double compute_sim_utc() {
	int zulu_days, zulu_offset, local_days;
	double local_days_fract, zulu_secs, local_secs, result;
	time_t now = time(NULL);
	time_t year_start_unixtime;
	struct tm nowtm;
	struct tm newtm = { .tm_mday = 1 };

	lacf_gmtime_r(&now, &nowtm);
	local_days = XPLMGetDatai(dr_time_days_local);
	local_secs = XPLMGetDataf(dr_time_sec_local);
	zulu_secs = XPLMGetDataf(dr_time_sec_zulu);
	/*
	 * First we determine the zulu offset. That's because we need the
	 * day of the year in zulu, not local.
	 */
	zulu_offset = local_secs - zulu_secs;
	/*
	 * We add 2 seconds to add a bit of fudge factor, otherwise we can
	 * get rollover when very near the date line.
	 */
	while (zulu_offset >= 86400 / 2 - 2)
		zulu_offset -= 86400;
	while (zulu_offset <= -86400 / 2 + 2)
		zulu_offset += 86400;
	/*
	 * With the zulu offset in hand, subtract it as a fraction of
	 * the day length from the local day. Leap years don't exist
	 * in X-Plane.
	 */
	local_days_fract = local_days + local_secs / 86400.0;
	zulu_days = floor(local_days_fract - zulu_offset / 86400.0);

	newtm.tm_year = nowtm.tm_year;
	year_start_unixtime = lacf_timegm(&newtm);
	result = year_start_unixtime + zulu_days * 86400 + zulu_secs;
	ASSERT(result > 946681200);	/* year 2000 */

	return (result);
}

static float time_floop(float elapsed, float since_last_fl, int counter, void *user_data) {
    UNUSED(elapsed);
    UNUSED(since_last_fl);
    UNUSED(counter);
    UNUSED(user_data);
    
    double now = XPLMGetDataf(dr_running_time);
    dt = now - last_update;
    last_update = now;
    sim_utc = compute_sim_utc();
    return -1;
}

void time_sys_init() {
    if(is_inited)
        return;
    
    dr_running_time = find_dr_safe("sim/time/total_running_time_sec");
    dr_time_days_local = find_dr_safe("sim/time/local_date_days");
    dr_time_sec_zulu = find_dr_safe("sim/time/zulu_time_sec");
    dr_time_sec_local = find_dr_safe("sim/time/local_time_sec");
    
    last_update = 0;
    dt = 0;
    sim_utc = 0;
    
    XPLMRegisterFlightLoopCallback(time_floop, -1, NULL);
    
    is_inited = true;
}

void time_sys_fini() {
    if(!is_inited) return;
    XPLMUnregisterFlightLoopCallback(time_floop, NULL);
    is_inited = false;
}

double time_get_sim_utc(void) {
	return sim_utc;
}

double time_get_dt(void) {
    return dt;
}

double time_get_clock(void) {
    return last_update;
}

