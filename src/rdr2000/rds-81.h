/*===--------------------------------------------------------------------------------------------===
 * rds-81.h
 *
 * Created by Amy Parent <amy@amyparent.com>
 * Copyright (c) 2024 Laminar Research. All rights reserved
 *
 * Licensed under the MIT License
 *===--------------------------------------------------------------------------------------------===
*/
#ifndef _RDS_81_H_
#define _RDS_81_H_

#include <stdbool.h>

/*
 * Video sources:
 *
 * - https://www.youtube.com/shorts/GJSkowPGYfM
 * - https://www.youtube.com/watch?v=fuN-QK2EvbM
 * - https://www.youtube.com/watch?v=tZkr2HPbN6A
 * - https://www.youtube.com/watch?v=ND11vvYjB7Q
 */


#define DEVICE_ID       "RDR2000_WXR"
#define DEVICE_NAME     "RDS-81 Weather Radar Display"

typedef struct rds81_t rds81_t;
typedef enum rds81_side_t {
    RDS81_SIDE_PILOT,
    RDS81_SIDE_COPILOT,
    RDS81_SIDE_NONE,
} rds81_side_t;

void rds81_declare_cmd_dr();
void rds81_unbind_dr_cmd();
rds81_side_t rds81_find_best_side();

void rds81_init(rds81_side_t side);
void rds81_fini();

#endif /* ifndef _RDS_81_H_ */
