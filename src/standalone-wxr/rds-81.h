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

/*
 * Video sources:
 *
 * - https://www.youtube.com/shorts/GJSkowPGYfM
 * - https://www.youtube.com/watch?v=fuN-QK2EvbM
 * - https://www.youtube.com/watch?v=tZkr2HPbN6A
 * - https://www.youtube.com/watch?v=ND11vvYjB7Q
 */


#define DEVICE_SIGNATURE    "RDS_81_WXR"


void rds81_init();
void rds81_fini();

// Called when the plane has changed, to reset or disable the radar display based on
// what the plane is.
void rds81_reset();



#endif /* ifndef _RDS_81_H_ */


