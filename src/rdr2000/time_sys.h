/*===--------------------------------------------------------------------------------------------===
 * time_sys.h
 *
 * Created by Amy Parent <amy@amyparent.com>
 * Copyright (c) 2024 Laminar Research. All rights reserved.
 *
 * Licensed under the MIT License
 *===--------------------------------------------------------------------------------------------===
*/
#ifndef _TIME_SYS_H_
#define _TIME_SYS_H_

#include <stdbool.h>

void time_sys_init(void);
void time_sys_fini(void);

double time_get_sim_utc(void);
double time_get_dt(void);
double time_get_clock(void);

#endif /* ifndef _TIME_SYS_H_ */

