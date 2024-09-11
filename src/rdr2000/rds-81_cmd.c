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

static inline float roundN(float val, int n) {
    float mult = pow(10, n);
    return roundf(val * mult) / mult;
}

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
        wxr->mode = CLAMP((int)wxr->mode + 1, 0, 3);
    }
    return 1;
}

static int handle_mode_dn(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase == xplm_CommandBegin) {
        wxr->mode = CLAMP((int)wxr->mode - 1, 0, 3);
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


static int handle_tilt_up(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase != xplm_CommandEnd) {
        float inc = phase == xplm_CommandBegin ? 0.1f : 0.05f;
        float tilt = XPLMGetDataf(wxr->dr_tilt);
        XPLMSetDataf(wxr->dr_tilt, CLAMP(tilt + inc, -15.f, 15.f));
    }
    return 1;
}

static int handle_tilt_dn(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase != xplm_CommandEnd) {
        float inc = phase == xplm_CommandBegin ? 0.1f : 0.05f;
        float tilt = XPLMGetDataf(wxr->dr_tilt);
        XPLMSetDataf(wxr->dr_tilt, CLAMP(tilt - inc, -15.f, 15.f));
    }
    return 1;
}

static int handle_gain_up(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase != xplm_CommandEnd) {
        float inc = phase == xplm_CommandBegin ? 0.05f : 0.02f;
        wxr->map_gain = CLAMP(wxr->map_gain + inc, 0.f, 1.f);
    }
    return 1;
}

static int handle_gain_dn(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase != xplm_CommandEnd) {
        float inc = phase == xplm_CommandBegin ? 0.05f : 0.02f;
        wxr->map_gain = CLAMP(wxr->map_gain - inc, 0.f, 1.f);
    }
    return 1;
}

static int handle_brt_up(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase != xplm_CommandEnd) {
        float inc = phase == xplm_CommandBegin ? 0.02f : 0.01f;
        float brt = XPLMGetAvionicsBrightnessRheo(wxr->device);
        XPLMSetAvionicsBrightnessRheo(wxr->device, CLAMP(brt + inc, 0.f, 1.f));
    }
    return 1;
}

static int handle_brt_dn(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase != xplm_CommandEnd) {
        float inc = phase == xplm_CommandBegin ? 0.02f : 0.01f;
        float brt = XPLMGetAvionicsBrightnessRheo(wxr->device);
        XPLMSetAvionicsBrightnessRheo(wxr->device, CLAMP(brt - inc, 0.f, 1.f));
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

// Dataref Handlers
static int get_mode(void *ptr) {
    UNUSED(ptr);
    if(!wxr)
        return 0;
    return wxr->mode;
}

static void set_mode(void *ptr, int val) {
    UNUSED(ptr);
    if(!wxr)
        return;
    wxr->mode = val;
}

static float get_tilt(void *ptr) {
    UNUSED(ptr);
    if(!wxr)
        return 0;
    return XPLMGetDataf(wxr->dr_tilt);
}

static void set_tilt(void *ptr, float val) {
    UNUSED(ptr);
    if(!wxr)
        return;
    XPLMSetDataf(wxr->dr_tilt, CLAMP(val, -15.f, 15.f));
}

static float get_gain(void *ptr) {
    UNUSED(ptr);
    if(!wxr)
        return 0;
    return wxr->map_gain;
}

static void set_gain(void *ptr, float val) {
    UNUSED(ptr);
    if(!wxr)
        return;
    wxr->map_gain = CLAMP(val, 0.f, 2.f);
}

static float get_brt(void *ptr) {
    UNUSED(ptr);
    if(!wxr)
        return 0;
    return XPLMGetAvionicsBrightnessRheo(wxr->device);
}

static void set_brt(void *ptr, float val) {
    UNUSED(ptr);
    if(!wxr)
        return;
    XPLMSetAvionicsBrightnessRheo(wxr->device, CLAMP(val, 0.f, 1.f));
}

void rds81_declare_cmd_dr() {
    wxr_out.cmd_popup = XPLMCreateCommand(DR_CMD_PREFIX "rdr2000/popup", "RDR2000 popup");
    wxr_out.cmd_popout = XPLMCreateCommand(DR_CMD_PREFIX "rdr2000/popout", "RDR2000 pop out window");
    
    wxr_out.cmd_mode_up = XPLMCreateCommand(DR_CMD_PREFIX "rdr2000/mode_up", "RDR2000 Mode Up");
    wxr_out.cmd_mode_dn = XPLMCreateCommand(DR_CMD_PREFIX "rdr2000/mode_down", "RDR2000 Mode Down");
    
    wxr_out.cmd_brt_up = XPLMCreateCommand(DR_CMD_PREFIX "rdr2000/brightness_up", "RDR2000 increase brightness");
    wxr_out.cmd_brt_dn = XPLMCreateCommand(DR_CMD_PREFIX "rdr2000/brightness_down", "RDR2000 decrease brightness");
    
    wxr_out.cmd_tilt_up = XPLMCreateCommand(DR_CMD_PREFIX "rdr2000/tilt_up", "RDR2000 increase tilt");
    wxr_out.cmd_tilt_dn = XPLMCreateCommand(DR_CMD_PREFIX "rdr2000/tilt_down", "RDR2000 decrease tilt");
    
    wxr_out.cmd_gain_up = XPLMCreateCommand(DR_CMD_PREFIX "rdr2000/gain_up", "RDR2000 increase gain");
    wxr_out.cmd_gain_dn = XPLMCreateCommand(DR_CMD_PREFIX "rdr2000/gain_down", "RDR2000 decrease gain");
    
    wxr_out.cmd_off = XPLMCreateCommand(DR_CMD_PREFIX "rdr2000/mode_off", "RDR2000 mode off");
    wxr_out.cmd_stby = XPLMCreateCommand(DR_CMD_PREFIX "rdr2000/mode_stby", "RDR2000 mode standby");
    wxr_out.cmd_test = XPLMCreateCommand(DR_CMD_PREFIX "rdr2000/mode_test", "RDR2000 mode test");
    wxr_out.cmd_on = XPLMCreateCommand(DR_CMD_PREFIX "rdr2000/mode_on", "RDR2000 mode on");
    
    wxr_out.cmd_wx = XPLMCreateCommand(DR_CMD_PREFIX "rdr2000/mode_wx", "RDR2000 mode Wx");
    wxr_out.cmd_wxa = XPLMCreateCommand(DR_CMD_PREFIX "rdr2000/mode_wxa", "RDR2000 mode WxA");
    wxr_out.cmd_map = XPLMCreateCommand(DR_CMD_PREFIX "rdr2000/mode_map", "RDR2000 mode Map");
    wxr_out.cmd_rng_up = XPLMCreateCommand(DR_CMD_PREFIX "rdr2000/range_up", "RDR2000 range up");
    wxr_out.cmd_rng_dn = XPLMCreateCommand(DR_CMD_PREFIX "rdr2000/range_down", "RDR2000 range down");
    wxr_out.cmd_stab = XPLMCreateCommand(DR_CMD_PREFIX "rdr2000/stab", "RDR2000 stab");
    
    wxr_out.dr_mode = create_dr_i(get_mode, set_mode, NULL, DR_CMD_PREFIX "rdr2000/mode");
    wxr_out.dr_brt = create_dr_f(get_brt, set_brt, NULL, DR_CMD_PREFIX "rdr2000/brightness");
    wxr_out.dr_tilt = create_dr_f(get_tilt, set_tilt, NULL, DR_CMD_PREFIX "rdr2000/tilt");
    wxr_out.dr_gain = create_dr_f(get_gain, set_gain, NULL, DR_CMD_PREFIX "rdr2000/gain");
}

void rds81_unbind_dr_cmd() {
    XPLMUnregisterDataAccessor(wxr_out.dr_mode);
    XPLMUnregisterDataAccessor(wxr_out.dr_gain);
    XPLMUnregisterDataAccessor(wxr_out.dr_tilt);
    XPLMUnregisterDataAccessor(wxr_out.dr_brt);
    
    wxr_out.dr_mode = NULL;
    wxr_out.dr_gain = NULL;
    wxr_out.dr_tilt = NULL;
    wxr_out.dr_brt = NULL;
    
    memset(&wxr_out, 0, sizeof(wxr_out));
}
