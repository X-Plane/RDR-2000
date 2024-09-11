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
    // Hard-set some of the weather radar datarefs so we don't end up in weird, non realistic
    // situations
    XPLMSetDataf(wxr->dr_sector_brg, 0);
    XPLMSetDataf(wxr->dr_sector_width, 45);
    XPLMSetDataf(wxr->dr_antenna_limit, 45);
    XPLMSetDatai(wxr->dr_auto_tilt, 0);
    XPLMSetDatai(wxr->dr_gcs, 0);
    XPLMSetDatai(wxr->dr_pws, 0);
    XPLMSetDatai(wxr->dr_multiscan, 0);
    
    // The RDS-81 only uses the pilot-set gain when it's in GND MAP mode. So outside that mode,
    // we just set it to 1.f (which is the "use calibrated gain" value for XP WXR).
    if(wxr->mode == RDS81_MODE_ON && wxr->submode == RDS81_SUBMODE_MAP) {
        XPLMSetDataf(wxr->dr_gain, wxr->map_gain * 2.f);
    } else {
        XPLMSetDataf(wxr->dr_gain, 1.f);
    }
    
    // Set the XP WXR's mode to the value that matches our internal mode.
    double time_since_on = time_get_clock() - wxr->on_time;
    if(time_since_on > RDS_WARMUP_ANTENNA) {
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
    } else {
        XPLMSetDatai(wxr->dr_mode, 0);
    }
    
    if(wxr->mode != RDS81_MODE_ON) {
        wxr->submode = RDS81_SUBMODE_WX;
    }
    
    
    // If we're off, we always reset the "On" time to now, else we set the "off" time. It sounds
    // counter-intuitive, but this means as soon as we're anything but off, the time stops updating,
    // and we have a marker for when the off->on transition happened.
    if(wxr->mode == RDS81_MODE_OFF)
        wxr->on_time = time_get_clock();
    else
        wxr->off_time = time_get_clock();
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

