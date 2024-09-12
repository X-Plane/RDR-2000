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

#include "cursor.h"
#include "time_sys.h"
#include "xplane.h"

#include <glutils/gl.h>
#include <glutils/renderer.h>
#include <helpers/helpers.h>

#include <nanovg.h>
#include <XPLMDisplay.h>
#include <XPLMGraphics.h>

#define DR_CMD_PREFIX ""

#define RDS_SCALE           1

#define RDS_WARMUP_ALPHA    5.f
#define RDS_WARMUP_SCALE    8.f
#define RDS_WARMUP_ANTENNA  8.f

#define RDS_SCREEN_W        640
#define RDS_SCREEN_H        480
#define RDS_BEZEL_W         1024
#define RDS_BEZEL_H         660

#define RDS_SCREEN_OFF_X    192
#define RDS_SCREEN_OFF_Y    100

#define BUTTON_COUNT    (6)
#define KNOB_COUNT      (4)

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
    const char  *tex;
    vec2        pos;
    vec2        size;
    
    enum {
        KNOB_INT,
        KNOB_FLOAT,
    }       type;

    float   min;
    float   max;
    float   min_angle;
    float   max_angle;
} knob_desc_t;

typedef struct {
    const knob_desc_t   *desc;
    XPLMCommandRef      cmd_up;
    XPLMCommandRef      cmd_dn;
    XPLMDataRef         val;
    
    GLuint              tex;
    gl_quad_t           *quad;
} knob_t;

typedef struct rds81_out_t {
    XPLMCommandRef  cmd_popup;
    XPLMCommandRef  cmd_popout;
    
    XPLMCommandRef  cmd_brt_up;
    XPLMCommandRef  cmd_brt_dn;
    
    XPLMCommandRef  cmd_tilt_up;
    XPLMCommandRef  cmd_tilt_dn;
    
    XPLMCommandRef  cmd_gain_up;
    XPLMCommandRef  cmd_gain_dn;
    
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
    
    XPLMDataRef     dr_mode;
    XPLMDataRef     dr_gain;
    XPLMDataRef     dr_tilt;
    XPLMDataRef     dr_brt;
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
    
    XPLMDataRef     dr_avionics_power;
    
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
        
    // UI elements
    knob_t          knobs[KNOB_COUNT];
    button_t        buttons[BUTTON_COUNT];
    XPLMCommandRef  act_cmd;
    
    cursor_t        *cur_click;
    cursor_t        *cur_rotate_left;
    cursor_t        *cur_rotate_right;
    
    // Logic data
    rds81_mode_t    mode;
    rds81_submode_t submode;
    bool            stab;
    double          on_time;
    double          off_time;
    
    // The RDS-81/RDR-2000 only let the pilot change the gain in MAP mode, so we need to keep track
    // of it aside from the default XP weather radar's datarefs.
    float           map_gain;
} rds81_t;

extern rds81_out_t wxr_out;
extern rds81_t *wxr;

GLuint rds81_load_tex(const char *name);

void rds81_init_kn_butt(rds81_t *wxr);
void rds81_fini_kn_butt(rds81_t *wxr);

void rds81_bind_commands(rds81_t *wxr);
void rds81_unbind_commands(rds81_t *wxr);
bool rds81_click_down(rds81_t *wxr, vec2 pos);
bool rds81_click_release(rds81_t *wxr);
bool rds81_scroll(rds81_t *wxr, vec2 pos, int clicks);
bool rds81_cursor(rds81_t *wxr, vec2 pos);

void rds81_reset_datarefs(rds81_t *wxr);
void rds81_update(rds81_t *wxr);
bool rds81_has_power(rds81_t *wxr);

#endif /* ifndef _RDS_81_IMPL_H_ */


