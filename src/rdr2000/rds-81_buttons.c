/*===--------------------------------------------------------------------------------------------===
 * rds-81_buttons.c
 *
 * Created by Amy Parent <amy@amyparent.com>
 * Copyright (c) 2024 Laminar Research. All rights reserved
 *
 * Licensed under the MIT License
 *===--------------------------------------------------------------------------------------------===
*/
#include "rds-81_impl.h"


#define BUTT(str, x, y, w, h)   (button_desc_t){.cmd=str, .pos={x, y}, .size={w, h}}

static const button_desc_t button_desc[BUTTON_COUNT] = {
    BUTT(DR_CMD_PREFIX "rdr2000/mode_wx", 45, 433, 76, 55),
    BUTT(DR_CMD_PREFIX "rdr2000/mode_wxa", 45, 348, 76, 55),
    BUTT(DR_CMD_PREFIX "rdr2000/mode_map", 45, 265, 76, 55),
    

    BUTT(DR_CMD_PREFIX "rdr2000/range_up", 904, 433, 76, 55),
    BUTT(DR_CMD_PREFIX "rdr2000/range_down", 904, 348, 76, 55),
    BUTT(DR_CMD_PREFIX "rdr2000/stab", 904, 265, 76, 55),
};

static const knob_desc_t knob_desc[KNOB_COUNT] = {
    (knob_desc_t){
        .cmd_up=DR_CMD_PREFIX "rdr2000/brightness_up",
        .cmd_dn=DR_CMD_PREFIX "rdr2000/brightness_down",
        .dref=DR_CMD_PREFIX "rdr2000/brightness",
        .tex="kn_arrow.png",
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
        .cmd_up=DR_CMD_PREFIX "rdr2000/gain_up",
        .cmd_dn=DR_CMD_PREFIX "rdr2000/gain_down",
        .dref=DR_CMD_PREFIX "rdr2000/gain",
        .tex="kn_arrow.png",
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
        .cmd_up=DR_CMD_PREFIX "rdr2000/tilt_up",
        .cmd_dn=DR_CMD_PREFIX "rdr2000/tilt_down",
        .dref=DR_CMD_PREFIX "rdr2000/tilt",
        .tex="kn_tilt.png",
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
        .cmd_up=DR_CMD_PREFIX "rdr2000/mode_up",
        .cmd_dn=DR_CMD_PREFIX "rdr2000/mode_down",
        .dref=DR_CMD_PREFIX "rdr2000/mode",
        .tex="kn_mode.png",
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

void rds81_init_kn_butt(rds81_t *wxr) {
    wxr->act_knob = NULL;
    wxr->act_button = NULL;
    
    for(int i = 0; i < BUTTON_COUNT; ++i) {
        const button_desc_t *desc = &button_desc[i];
        button_t *butt = &wxr->buttons[i];

        butt->desc = desc;
        butt->cmd = XPLMFindCommand(desc->cmd);
        ASSERT(butt->cmd != NULL);
    }
    
    for(int i = 0; i < KNOB_COUNT; ++i) {
        const knob_desc_t *desc = &knob_desc[i];
        knob_t *knob = &wxr->knobs[i];
        
        knob->desc = desc;
        knob->cmd_up = XPLMFindCommand(desc->cmd_up);
        knob->cmd_dn = XPLMFindCommand(desc->cmd_dn);
        knob->val = XPLMFindDataRef(desc->dref);
        
        ASSERT(knob->cmd_up != NULL);
        ASSERT(knob->cmd_dn != NULL);
        // ASSERT(knob->val != NULL);
        
        knob->tex = rds81_load_tex(desc->tex);
        knob->quad = quad_new(knob->tex, 0);
    }
}

void rds81_fini_kn_butt(rds81_t *wxr) {
    wxr->act_knob = NULL;
    wxr->act_button = NULL;
    
    for(int i = 0; i < KNOB_COUNT; ++i) {
        knob_t *knob = &wxr->knobs[i];
        quad_destroy(knob->quad);
        glDeleteTextures(1, &knob->tex);
    }
}
