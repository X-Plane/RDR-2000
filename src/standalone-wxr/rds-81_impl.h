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
#define NANOVG_GL2_IMPLEMENTATION
#include <nanovg_gl.h>
#include <XPLMDisplay.h>

#define RDS_SCALE           1

#define RDS_SCREEN_W        640
#define RDS_SCREEN_H        480
#define RDS_BEZEL_W         1024
#define RDS_BEZEL_H         660

#define RDS_SCREEN_OFF_X    192
#define RDS_SCREEN_OFF_Y    100

typedef enum {
    BUTTON_WX,
    BUTTON_WXA,
    BUTTON_MAP,
    BUTTON_RNG_UP,
    BUTTON_RNG_DN,
    BUTTON_STAB
} button_id_t;

typedef enum {
    KNOB_BRT,
    KNOB_GAIN,
    KNOB_MODE,
    KNOB_TILT,
} knob_id_t;

typedef struct rds81_t {
    GLuint          screen_fbo;
    GLuint          screen_tex;
    GLuint          screen_shader;
    GLuint          bezel_tex;
    GLuint          dots_tex;
    GLuint          crt_mask_tex;
    
    gl_quad_t       *bezel_quad;
    gl_quad_t       *screen_quad;
    gl_quad_t       *dots_quad;
    gl_quad_t       *wxr_quad;
    
    XPLMAvionicsID  device;
    
    NVGcontext      *vg;
    
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
    
    XPLMCommandRef  cmd_popup;
    XPLMCommandRef  cmd_popout;
} rds81_t;

#endif /* ifndef _RDS_81_IMPL_H_ */


