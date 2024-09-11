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
    
    if(wxr->mode == RDS81_MODE_ON && wxr->submode == RDS81_SUBMODE_MAP) {
        XPLMSetDataf(wxr->dr_gain, wxr->map_gain * 2.f);
    } else {
        XPLMSetDataf(wxr->dr_gain, 1.f);
    }
    
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

