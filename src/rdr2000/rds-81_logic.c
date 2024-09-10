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

// Command handlers

static int handle_submode_wx(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase == xplm_CommandBegin) {
        wxr->submode = RDS81_SUBMODE_WX;
    }
    return 1;
}

static int handle_submode_wxa(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase == xplm_CommandBegin) {
        wxr->submode = RDS81_SUBMODE_WXA;
    }
    return 1;
}

static int handle_submode_map(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase == xplm_CommandBegin) {
        wxr->submode = RDS81_SUBMODE_MAP;
    }
    return 1;
}

static int handle_stab(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase == xplm_CommandBegin) {
        int stab = !XPLMGetDatai(wxr->dr_stab);
        XPLMSetDatai(wxr->dr_stab, stab);
    }
    return 1;
}

static int handle_range_buttons(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase == xplm_CommandBegin) {
        int range = XPLMGetDatai(wxr->dr_range_idx);
        if(cmd == wxr_out.cmd_rng_up)
            range += 1;
        else if(cmd == wxr_out.cmd_rng_dn)
            range -= 1;
        range = CLAMP(range, 0, 6);
        XPLMSetDatai(wxr->dr_range_idx, range);
    }
    return 1;
}

static int handle_mode_up(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase == xplm_CommandBegin) {
        int mode = wxr->mode + 1;
        wxr->mode = CLAMP(mode, 0, 3);
    }
    return 1;
}

static int handle_mode_dn(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase == xplm_CommandBegin) {
        int mode = wxr->mode - 1;
        wxr->mode = CLAMP(mode, 0, 3);
    }
    return 1;
}

static int handle_off(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase == xplm_CommandBegin) {
        wxr->mode = RDS81_MODE_OFF;
    }
    return 1;
}

static int handle_stby(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase == xplm_CommandBegin) {
        wxr->mode = RDS81_MODE_STBY;
    }
    return 1;
}

static int handle_test(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase == xplm_CommandBegin) {
        wxr->mode = RDS81_MODE_TEST;
    }
    return 1;
}

static int handle_on(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase == xplm_CommandBegin) {
        wxr->mode = RDS81_MODE_ON;
    }
    return 1;
}

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

void rds81_bind_commands(rds81_t *wxr) {
    XPLMRegisterCommandHandler(wxr_out.cmd_wx, handle_submode_wx, 1, wxr);
    // XPLMRegisterCommandHandler(wxr_out.cmd_def_wx, handle_submode_map, 1, wxr);
    XPLMRegisterCommandHandler(wxr_out.cmd_wxa, handle_submode_wxa, 1, wxr);
    XPLMRegisterCommandHandler(wxr_out.cmd_map, handle_submode_map, 1, wxr);
    // XPLMRegisterCommandHandler(wxr_out.cmd_def_map, handle_submode_map, 1, wxr);
    
    XPLMRegisterCommandHandler(wxr_out.cmd_stab, handle_stab, 1, wxr);
    XPLMRegisterCommandHandler(wxr_out.cmd_rng_up, handle_range_buttons, 1, wxr);
    XPLMRegisterCommandHandler(wxr_out.cmd_rng_dn, handle_range_buttons, 1, wxr);
    
    XPLMRegisterCommandHandler(wxr_out.cmd_off, handle_off, 1, wxr);
    XPLMRegisterCommandHandler(wxr_out.cmd_stby, handle_stby, 1, wxr);
    XPLMRegisterCommandHandler(wxr_out.cmd_test, handle_test, 1, wxr);
    XPLMRegisterCommandHandler(wxr_out.cmd_on, handle_on, 1, wxr);
}

void rds81_unbind_commands(rds81_t *wxr) {
    XPLMUnregisterCommandHandler(wxr_out.cmd_wx, handle_submode_wx, 1, wxr);
    // XPLMUnregisterCommandHandler(wxr_out.cmd_def_wx, handle_submode_map, 1, wxr);
    XPLMUnregisterCommandHandler(wxr_out.cmd_wxa, handle_submode_wxa, 1, wxr);
    XPLMUnregisterCommandHandler(wxr_out.cmd_map, handle_submode_map, 1, wxr);
    // XPLMUnregisterCommandHandler(wxr_out.cmd_def_map, handle_submode_map, 1, wxr);
    
    XPLMUnregisterCommandHandler(wxr_out.cmd_stab, handle_stab, 1, wxr);
    XPLMUnregisterCommandHandler(wxr_out.cmd_rng_up, handle_range_buttons, 1, wxr);
    XPLMUnregisterCommandHandler(wxr_out.cmd_rng_dn, handle_range_buttons, 1, wxr);
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

