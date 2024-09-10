/*===--------------------------------------------------------------------------------------------===
 * rds-81_cmd.c
 *
 * Created by Amy Parent <amy@amyparent.com>
 * Copyright (c) 2024 Laminar Research. All rights reserved
 *
 * Licensed under the MIT License
 *===--------------------------------------------------------------------------------------------===
*/
#include "rds-81_impl.h"


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
        wxr_out.mode = wxr->mode;
    }
    return 1;
}

static int handle_mode_dn(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase == xplm_CommandBegin) {
        int mode = wxr->mode - 1;
        wxr->mode = CLAMP(mode, 0, 3);
        wxr_out.mode = wxr->mode;
    }
    return 1;
}

static int handle_off(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase == xplm_CommandBegin) {
        wxr->mode = RDS81_MODE_OFF;
        wxr_out.mode = wxr->mode;
    }
    return 1;
}

static int handle_stby(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase == xplm_CommandBegin) {
        wxr->mode = RDS81_MODE_STBY;
        wxr_out.mode = wxr->mode;
    }
    return 1;
}

static int handle_test(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase == xplm_CommandBegin) {
        wxr->mode = RDS81_MODE_TEST;
        wxr_out.mode = wxr->mode;
    }
    return 1;
}

static int handle_on(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase == xplm_CommandBegin) {
        wxr->mode = RDS81_MODE_ON;
        wxr_out.mode = wxr->mode;
    }
    return 1;
}


static int handle_tilt_up(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase == xplm_CommandBegin) {
        wxr_out.tilt = CLAMP(wxr_out.tilt + 0.1f, -15.f, 15.f);
    }
    return 1;
}

static int handle_tilt_dn(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase == xplm_CommandBegin) {
        wxr_out.tilt = CLAMP(wxr_out.tilt - 0.1f, -15.f, 15.f);
    }
    return 1;
}

static int handle_gain_up(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase == xplm_CommandBegin) {
        wxr_out.gain = CLAMP(wxr_out.gain + 0.1f, 0.f, 2.f);
    }
    return 1;
}

static int handle_gain_dn(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase == xplm_CommandBegin) {
        wxr_out.gain = CLAMP(wxr_out.gain - 0.1f, 0.f, 2.f);
    }
    return 1;
}

static int handle_brt_up(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase == xplm_CommandBegin) {
        wxr_out.gain = CLAMP(wxr_out.brt + 0.1f, 0.f, 2.f);
    }
    return 1;
}

static int handle_brt_dn(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase == xplm_CommandBegin) {
        wxr_out.gain = CLAMP(wxr_out.brt - 0.1f, 0.f, 2.f);
    }
    return 1;
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
    
    XPLMRegisterCommandHandler(wxr_out.cmd_mode_up, handle_mode_up, 1, wxr);
    XPLMRegisterCommandHandler(wxr_out.cmd_mode_dn, handle_mode_dn, 1, wxr);
    
    XPLMRegisterCommandHandler(wxr_out.cmd_tilt_up, handle_tilt_up, 1, wxr);
    XPLMRegisterCommandHandler(wxr_out.cmd_tilt_dn, handle_tilt_dn, 1, wxr);
    
    XPLMRegisterCommandHandler(wxr_out.cmd_gain_up, handle_gain_up, 1, wxr);
    XPLMRegisterCommandHandler(wxr_out.cmd_gain_dn, handle_gain_dn, 1, wxr);
    
    XPLMRegisterCommandHandler(wxr_out.cmd_brt_up, handle_brt_up, 1, wxr);
    XPLMRegisterCommandHandler(wxr_out.cmd_brt_dn, handle_brt_dn, 1, wxr);
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
    
    XPLMUnregisterCommandHandler(wxr_out.cmd_off, handle_off, 1, wxr);
    XPLMUnregisterCommandHandler(wxr_out.cmd_stby, handle_stby, 1, wxr);
    XPLMUnregisterCommandHandler(wxr_out.cmd_test, handle_test, 1, wxr);
    XPLMUnregisterCommandHandler(wxr_out.cmd_on, handle_on, 1, wxr);
    
    XPLMUnregisterCommandHandler(wxr_out.cmd_mode_up, handle_mode_up, 1, wxr);
    XPLMUnregisterCommandHandler(wxr_out.cmd_mode_dn, handle_mode_dn, 1, wxr);
    
    XPLMUnregisterCommandHandler(wxr_out.cmd_tilt_up, handle_tilt_up, 1, wxr);
    XPLMUnregisterCommandHandler(wxr_out.cmd_tilt_dn, handle_tilt_dn, 1, wxr);
    
    XPLMUnregisterCommandHandler(wxr_out.cmd_gain_up, handle_gain_up, 1, wxr);
    XPLMUnregisterCommandHandler(wxr_out.cmd_gain_dn, handle_gain_dn, 1, wxr);
    
    XPLMUnregisterCommandHandler(wxr_out.cmd_brt_up, handle_brt_up, 1, wxr);
    XPLMUnregisterCommandHandler(wxr_out.cmd_brt_dn, handle_brt_dn, 1, wxr);
}
