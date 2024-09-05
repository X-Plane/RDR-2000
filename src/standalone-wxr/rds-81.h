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


#define DEVICE_ID       "RDS_81_WXR"
#define DEVICE_NAME     "RDS-81 Weather Radar Display"

typedef struct rds81_t rds81_t;

void rds81_declare_cmd_dr();

rds81_t *rds81_new(bool copilot);
void rds81_destroy(rds81_t *wxr);

#endif /* ifndef _RDS_81_H_ */
