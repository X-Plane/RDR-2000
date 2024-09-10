/*===--------------------------------------------------------------------------------------------===
 * rds-81_impl.h
 *
 * Created by Amy Parent <amy@amyparent.com>
 * Copyright (c) 2024 Laminar Research. All rights reserved
 *
 * Licensed under the MIT License
 *===--------------------------------------------------------------------------------------------===
*/
#ifndef _RDS_81_IMPL_H_
#define _RDS_81_IMPL_H_

#include "rds-81.h"
#include "xplane.h"
#include <glutils/gl.h>
#include <glutils/renderer.h>
#include <helpers/helpers.h>

#include <nanovg.h>
#include <XPLMDisplay.h>
#include <XPLMGraphics.h>

#define RDS_SCALE           1

#define RDS_SCREEN_W        640
#define RDS_SCREEN_H        480
#define RDS_BEZEL_W         1024
#define RDS_BEZEL_H         660

#define RDS_SCREEN_OFF_X    192
#define RDS_SCREEN_OFF_Y    100

// typedef enum {
//     BUTTON_WX,
//     BUTTON_WXA,
//     BUTTON_MAP,
//     BUTTON_RNG_UP,
//     BUTTON_RNG_DN,
//     BUTTON_STAB
// } button_id_t;
//
// typedef enum {
//     KNOB_BRT,
//     KNOB_GAIN,
//     KNOB_MODE,
//     KNOB_TILT,
// } knob_id_t;

typedef enum {
    RDS81_MODE_OFF,
    RDS81_MODE_STBY,
    RDS81_MODE_TEST,
    RDS81_MODE_ON,
} rds81_mode_t;

typedef enum {
    RDS81_SUBMODE_WX,
    RDS81_SUBMODE_WXA,
    RDS81_SUBMODE_MAP
} rds81_submode_t;

typedef struct {
    const char  *cmd;
    vec2        pos;
    vec2        size;
} button_desc_t;

typedef struct {
    const button_desc_t *desc;
    XPLMCommandRef      cmd;
} button_t;

typedef struct {
    const char  *cmd_up;
    const char  *cmd_dn;
    const char  *dref;
    vec2        pos;
    vec2        size;
    
    enum {
        KBOB_INT,
        KNOB_FLOAT,
    }           type;
    
    union {
        struct {
            float   min;
            float   max;
            float   min_angle;
            float   max_angle;
        } f32;
        
        struct {
            int     min;
            int     max;
            int     min_angle;
            int     max_angle;
        } i32;
    };
} knob_desc_t;

typedef struct {
    const knob_desc_t   *desc;
    XPLMCommandRef  cmd_up;
    XPLMCommandRef  cmd_dn;
    XPLMDataRef     val;
} knob_t;

typedef struct rds81_out_t {
    int             mode;
    float           brightness;
    
    XPLMCommandRef  cmd_popup;
    XPLMCommandRef  cmd_popout;
    
    XPLMCommandRef  cmd_brt_up;
    XPLMCommandRef  cmd_brt_dn;
    XPLMCommandRef  cmd_wx;
    XPLMCommandRef  cmd_wxa;
    XPLMCommandRef  cmd_map;
    XPLMCommandRef  cmd_rng_up;
    XPLMCommandRef  cmd_rng_dn;
    XPLMCommandRef  cmd_stab;
    
    XPLMCommandRef  cmd_mode_up;
    XPLMCommandRef  cmd_mode_dn;
    XPLMCommandRef  cmd_off;
    XPLMCommandRef  cmd_stby;
    XPLMCommandRef  cmd_test;
    XPLMCommandRef  cmd_on;
} rds81_out_t;

typedef struct rds81_t {
    GLuint          screen_fbo;
    GLuint          screen_tex;
    GLuint          screen_shader;
    GLuint          wxr_shader;
    GLuint          bezel_tex;
    GLuint          dots_tex;
    GLuint          crt_mask_tex;
    
    gl_quad_t       *bezel_quad;
    gl_quad_t       *screen_quad;
    gl_quad_t       *dots_quad;
    gl_quad_t       *wxr_quad;
    
    XPLMAvionicsID  device;
    
    NVGcontext      *vg;
    
    XPLMTextureID   wxr_tex_id;
    
    XPLMDataRef     dr_proj_mat;
    XPLMDataRef     dr_mv_mat;
    XPLMDataRef     dr_fbo;
    XPLMDataRef     dr_viewport;
    
    XPLMDataRef     dr_mode;
    XPLMDataRef     dr_tilt;
    XPLMDataRef     dr_tilt_antenna;
    XPLMDataRef     dr_auto_tilt;
    XPLMDataRef     dr_gain;
    XPLMDataRef     dr_stab;
    
    XPLMDataRef     dr_gcs;
    XPLMDataRef     dr_pws;
    XPLMDataRef     dr_sector_brg;
    XPLMDataRef     dr_sector_width;
    XPLMDataRef     dr_antenna_limit;
    XPLMDataRef     dr_multiscan;
    
    XPLMDataRef     dr_range_idx;
    XPLMDataRef     dr_range;
    
    // Logic data
    rds81_mode_t    mode;
    rds81_submode_t submode;
    bool            stab;
} rds81_t;

extern rds81_out_t wxr_out;

void rds81_bind_commands(rds81_t *wxr);
void rds81_unbind_commands(rds81_t *wxr);
void rds81_reset_datarefs(rds81_t *wxr);
void rds81_update(rds81_t *wxr);

#endif /* ifndef _RDS_81_IMPL_H_ */


