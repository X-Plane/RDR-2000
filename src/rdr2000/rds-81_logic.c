/*===--------------------------------------------------------------------------------------------===
 * rds-81_logic.c
 *
 * Created by Amy Parent <amy@amyparent.com>
 * Copyright (c) 2024 Laminar Research. All rights reserved
 *
 * Licensed under the MIT License
 *===--------------------------------------------------------------------------------------------===
*/
#include "rds-81_impl.h"

rds81_out_t wxr_out;

#define BUTT(str, x, y, w, h)   (button_desc_t){.cmd=str, .pos={x, y}, .size={w, h}}

static const button_desc_t button_desc[BUTTON_COUNT] = {
    BUTT("rdr2000/mode_wx", 45, 433, 76, 55),
    BUTT("rdr2000/mode_wxa", 45, 348, 76, 55),
    BUTT("rdr2000/mode_map", 45, 265, 76, 55),
    

    BUTT("rdr2000/range_up", 904, 433, 76, 55),
    BUTT("rdr2000/range_down", 904, 348, 76, 55),
    BUTT("rdr2000/stab", 904, 265, 76, 55),
};

static const knob_desc_t knob_desc[KNOB_COUNT] = {
    (knob_desc_t){
        .cmd_up="rdr2000/brightness_up", .cmd_dn="rdr2000/brightness_dn",
        .dref="rdr2000/brightness",
        .pos={47, 554}, .size={69, 69},
        .type=KNOB_FLOAT,
        .f32={
            .min=0.f,
            .max=1.f,
            .min_angle=0,
            .max_angle=315,
        }
    },
    (knob_desc_t){
        .cmd_up="rdr2000/gain_up", .cmd_dn="rdr2000/gain_dn",
        .dref="rdr2000/gain",
        .pos={47, 83}, .size={69, 69},
        .type=KNOB_FLOAT,
        .f32={
            .min=0.f,
            .max=2.f,
            .min_angle=0,
            .max_angle=315,
        }
    },
    (knob_desc_t){
        .cmd_up="rdr2000/tilt_up", .cmd_dn="rdr2000/tilt_dn",
        .dref="rdr2000/tilt",
        .pos={904, 58}, .size={95, 95},
        .type=KNOB_FLOAT,
        .f32={
            .min=-15.f,
            .max=15.f,
            .min_angle=-135,
            .max_angle=135,
        }
    },
    (knob_desc_t){
        .cmd_up="rdr2000/mode_up", .cmd_dn="rdr2000/mode_dn",
        .dref="rdr2000/mode",
        .pos={904, 526}, .size={95, 95},
        .type=KNOB_INT,
        .i32={
            .min=0,
            .max=3,
            .min_angle=-52,
            .max_angle=26,
        }
    },
    
};

// Picks the best available side for the weather radar to be bound to.
rds81_side_t rds81_find_best_side() {
    if(XPLMGetTexture(xplm_Tex_Radar_Copilot))
        return RDS81_SIDE_COPILOT;
    if(XPLMGetTexture(xplm_Tex_Radar_Pilot))
        return RDS81_SIDE_PILOT;
    return RDS81_SIDE_NONE;
}


void rds81_update(rds81_t *wxr) {
    XPLMSetDataf(wxr->dr_sector_brg, 0);
    XPLMSetDataf(wxr->dr_sector_width, 45);
    XPLMSetDataf(wxr->dr_antenna_limit, 45);
    XPLMSetDatai(wxr->dr_auto_tilt, 0);
    XPLMSetDatai(wxr->dr_gcs, 0);
    XPLMSetDatai(wxr->dr_pws, 0);
    XPLMSetDatai(wxr->dr_multiscan, 0);
    
    
    switch(wxr->mode) {
    case RDS81_MODE_OFF:
    case RDS81_MODE_STBY:
        XPLMSetDatai(wxr->dr_mode, 0);
        break;
    case RDS81_MODE_TEST:
        XPLMSetDatai(wxr->dr_mode, 1);
        break;
    case RDS81_MODE_ON:
        XPLMSetDatai(wxr->dr_mode, wxr->submode == RDS81_SUBMODE_MAP ? 4 : 2);
        break;
    }
}

void rds81_reset_datarefs(rds81_t *wxr) {
    XPLMSetDatai(wxr->dr_mode, 0);
    XPLMSetDatai(wxr->dr_stab, 1);
    
    
    XPLMSetDataf(wxr->dr_sector_brg, 0);
    XPLMSetDataf(wxr->dr_sector_width, 90);
    XPLMSetDataf(wxr->dr_antenna_limit, 90);
    
    XPLMSetDatai(wxr->dr_auto_tilt, 0);
    XPLMSetDatai(wxr->dr_gcs, 0);
    XPLMSetDatai(wxr->dr_pws, 0);
    XPLMSetDatai(wxr->dr_multiscan, 0);
}

