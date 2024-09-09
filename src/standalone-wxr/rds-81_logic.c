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

