/*===--------------------------------------------------------------------------------------------===
 * rds-81.c
 *
 * Created by Amy Parent <amy@amyparent.com>
 * Copyright (c) 2024 Laminar Research. All rights reserved
 *
 * Licensed under the MIT License
 *===--------------------------------------------------------------------------------------------===
*/
#include "rds-81_impl.h"
#include <XPLMGraphics.h>
#include <cglm/mat4.h>

static const char *vert_shader =
    "#version 120\n"
    "uniform mat4       pvm;\n"
    "attribute vec3     vtx_pos;\n"
    "attribute vec2     vtx_tex0;\n"
    "varying vec2       tex_coord;\n"
    "void main() {\n"
    "   tex_coord = vtx_tex0;\n"
    "   gl_Position = pvm * vec4(vtx_pos, 1.0);\n"
    "}\n";

static const char *frag_shader =
    "#version 120\n"
    "uniform sampler2D  tex;\n"
    "uniform sampler2D  mask;\n"
    "uniform float      alpha;\n"
    "varying vec2       tex_coord;\n"
    "void main() {\n"
    "   vec4 glow = vec4(0.1, 0.12, 0.15, 1.0);\n"
    "   vec4 col = glow + texture2D(tex, tex_coord);\n"
    // "   float brt = (col.r + col.g + col.b) / 3.f;\n"
    "   vec4 p = texture2D(mask, tex_coord);\n"
    "   float p_dot = (p.r + p.g + p.b) / 3.f;\n"
    "   gl_FragColor = col;\n"
    "   gl_FragColor.a *= p_dot;\n"
    "}\n";

static void rds_get_xp_pvm(rds81_t *wxr, mat4 pvm) {
    mat4 proj_mat, mv_mat;
    ASSERT(XPLMGetDatavf(wxr->dr_proj_mat, (float *)proj_mat, 0, 16) == 16);
    ASSERT(XPLMGetDatavf(wxr->dr_mv_mat, (float *)mv_mat, 0, 16) == 16);
    glm_mat4_mul(proj_mat, mv_mat, pvm);
}

// MARK: - Command Handlers

static int handle_popup(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    UNUSED(cmd);
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase == xplm_CommandBegin)
        XPLMSetAvionicsPopupVisible(wxr->device, true);
    return 1;
}

static int handle_popout(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon) {
    UNUSED(cmd);
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    if(phase == xplm_CommandBegin)
        XPLMPopOutAvionics(wxr->device);
    return 1;
}

// MARK: - Device callbacks

static void rds_draw_bezel(float r, float g, float b, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    XPLMSetGraphicsState(0, 1, 0, 1, 1, 0, 0);
    mat4 pvm;
    rds_get_xp_pvm(wxr, pvm);
    glCullFace(GL_BACK);
    quad_render(pvm, wxr->bezel_quad, VEC2(0, 0), VEC2(RDS_BEZEL_W * RDS_SCALE, RDS_BEZEL_H * RDS_SCALE), 1.f);
}

#define WXR_CTR_X   (160*2)
#define WXR_CTR_Y   (17*2)

#define WXR_H       (205*2)
#define WXR_W       (290*2)

#define WXR_POS_X   (WXR_CTR_X - (WXR_W/2))
#define WXR_POS_Y   (WXR_CTR_Y)

static void draw_fbo(rds81_t *wxr, NVGcontext *vg, mat4 pvm) {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    quad_render(pvm, wxr->wxr_quad, VEC2(WXR_POS_X, WXR_POS_Y), VEC2(WXR_W, WXR_H), 1.f);
    // quad_render(pvm, wxr->wxr_quad, VECT2(0, 0), VECT2(RDS_SCREEN_W, RDS_SCREEN_H), 1.f);
    quad_render(pvm, wxr->dots_quad, VEC2(0, 0), VEC2(RDS_SCREEN_W, RDS_SCREEN_H), 1.f);
}

static void rds_draw_screen(void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    
    // Save XP data
    int old_vp[4];
    int old_fbo = XPLMGetDatai(wxr->dr_fbo);
    XPLMGetDatavi(wxr->dr_viewport, old_vp, 0, 4);

    XPLMSetGraphicsState(0, 2, 0, 1, 1, 0, 0);
    glCullFace(GL_BACK);
    
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    int wxr_img = XPLMGetTexture(xplm_Tex_Radar_Pilot);
    quad_set_tex(wxr->wxr_quad, wxr_img);
    
    glBindFramebuffer(GL_FRAMEBUFFER, wxr->screen_fbo);
    
    mat4 ortho;
    glm_ortho(0, RDS_SCREEN_W, 0, RDS_SCREEN_H, -1, 1, ortho);
    glViewport(0, 0, RDS_SCREEN_W/2, RDS_SCREEN_H/2);
    draw_fbo(wxr, wxr->vg, ortho);
    
    // Revert to how things were before we mucked with OpenGL state
    glBindFramebuffer(GL_FRAMEBUFFER, old_fbo);
    glViewport(old_vp[0], old_vp[1], old_vp[2], old_vp[3]);
    
    mat4 pvm;
    rds_get_xp_pvm(wxr, pvm);
    glUseProgram(wxr->screen_shader);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, wxr->crt_mask_tex);
    glUniform1i(glGetUniformLocation(wxr->screen_shader, "mask"), 1);
    quad_render(pvm, wxr->screen_quad, VEC2(0, 0), VEC2(RDS_SCREEN_W * RDS_SCALE, RDS_SCREEN_H * RDS_SCALE), 1.f);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// MARK: - "public" API

static GLuint rds_load_tex(const char *name) {
    char *path = fs_make_path(get_plugin_dir(), "resources", name, NULL);
    int w = 0, h = 0;
    GLuint tex = gl_load_tex(path, &w, &h);
    if(tex == 0)
        log_msg("could not load texture `%s'", path);
    free(path);
    return tex;
}

// This *must* run in XPPluginStart, not enable, so OBJ can bind to our commands and datarefs.
void rds81_declare_cmd_dr() {
    XPLMCreateCommand("laminar/standalone_wxr/popup", "Weather Radar popup");
    XPLMCreateCommand("laminar/standalone_wxr/popout", "Weather Radar pop out window");
}

static XPLMCommandRef bind_cmd(const char *path, XPLMCommandCallback_f cb, int before, void *refcon) {
    XPLMCommandRef cmd = XPLMFindCommand(path);
    if(cmd == NULL)
        return NULL;
    XPLMRegisterCommandHandler(cmd, cb, before, refcon);
    return cmd;
}

rds81_t *rds81_new(bool copilot) {
    rds81_t *wxr = safe_calloc(1, sizeof(*wxr));
    wxr->vg = nvgCreateGL2(NVG_ANTIALIAS);
    
    // Gather all the datarefs we need to make things work
    const char *side_str = copilot ? "_copilot" : "";
    wxr->dr_proj_mat = find_dr_safe("sim/graphics/view/projection_matrix");
    wxr->dr_mv_mat = find_dr_safe("sim/graphics/view/modelview_matrix");
    wxr->dr_fbo = find_dr_safe("sim/graphics/view/current_gl_fbo");
    wxr->dr_viewport = find_dr_safe("sim/graphics/view/viewport");
    
    wxr->dr_mode = find_dr_safe("sim/cockpit2/EFIS/EFIS_weather_mode%s", side_str);
    wxr->dr_tilt = find_dr_safe("sim/cockpit2/EFIS/EFIS_weather_tilt%s", side_str);
    wxr->dr_tilt_antenna = find_dr_safe("sim/cockpit2/EFIS/EFIS_weather_tilt_antenna%s", side_str);
    wxr->dr_auto_tilt = find_dr_safe("sim/cockpit2/EFIS/EFIS_weather_auto_tilt%s", side_str);
    wxr->dr_gain = find_dr_safe("sim/cockpit2/EFIS/EFIS_weather_gain%s", side_str);
    
    wxr->dr_stab = find_dr_safe("sim/cockpit2/EFIS/EFIS_weather_stab%s", side_str);
    wxr->dr_gcs = find_dr_safe("sim/cockpit2/EFIS/EFIS_weather_gcs%s", side_str);
    wxr->dr_pws = find_dr_safe("sim/cockpit2/EFIS/EFIS_weather_pws%s", side_str);
    wxr->dr_multiscan = find_dr_safe("sim/cockpit2/EFIS/EFIS_weather_multiscan%s", side_str);
    
    wxr->dr_sector_brg = find_dr_safe("sim/cockpit2/EFIS/EFIS_weather_sector_brg%s", side_str);
    wxr->dr_sector_width = find_dr_safe("sim/cockpit2/EFIS/EFIS_weather_sector_width%s", side_str);
    wxr->dr_antenna_limit = find_dr_safe("sim/cockpit2/EFIS/EFIS_weather_antenna_limit%s", side_str);
    
    wxr->dr_range_idx = find_dr_safe("sim/cockpit2/EFIS/map_range%s", side_str);
    wxr->dr_range = find_dr_safe("sim/cockpit2/EFIS/map_range_nm%s", side_str);
    
    // Bind our own commands
    wxr->cmd_popup = bind_cmd("laminar/standalone_wxr/popup", handle_popup, 0, wxr);
    wxr->cmd_popout = bind_cmd("laminar/standalone_wxr/popout", handle_popout, 0, wxr);
    
    // Allocate the OpenGL resources we need
    wxr->screen_shader = gl_program_new(vert_shader, frag_shader);
    
    wxr->screen_fbo = gl_fbo_new(RDS_SCREEN_W/2, RDS_SCREEN_H/2, &wxr->screen_tex);
    wxr->bezel_tex = rds_load_tex("bezel.png");
    wxr->dots_tex = rds_load_tex("dots.png");
    wxr->crt_mask_tex = rds_load_tex("crt_mask.png");
    
    wxr->bezel_quad = quad_new(wxr->bezel_tex, 0);
    wxr->screen_quad = quad_new(wxr->screen_tex, wxr->screen_shader);
    wxr->dots_quad = quad_new(wxr->dots_tex, 0);
    wxr->wxr_quad = quad_new(0, 0);
    
    // Create the XP avionics device
    XPLMCreateAvionics_t desc = {
        .structSize = sizeof(XPLMCreateAvionics_t),
        .screenWidth = RDS_SCREEN_W * RDS_SCALE,
        .screenHeight = RDS_SCREEN_H * RDS_SCALE,
        .bezelWidth = RDS_BEZEL_W * RDS_SCALE,
        .bezelHeight = RDS_BEZEL_H * RDS_SCALE,
        .screenOffsetX = RDS_SCREEN_OFF_X * RDS_SCALE,
        .screenOffsetY = RDS_SCREEN_OFF_Y * RDS_SCALE,
        
        .bezelDrawCallback = rds_draw_bezel,
        .drawCallback = rds_draw_screen,
        .deviceID = DEVICE_ID,
        .deviceName = DEVICE_NAME,
        .refcon = wxr,
    };
    wxr->device = XPLMCreateAvionicsEx(&desc);
    ASSERT(wxr->device != NULL);
    return wxr;
}

void rds81_destroy(rds81_t *wxr) {
    ASSERT(wxr != NULL);
    
    XPLMUnregisterCommandHandler(wxr->cmd_popup, handle_popup, 0, wxr);
    XPLMUnregisterCommandHandler(wxr->cmd_popout, handle_popout, 0, wxr);
    
    quad_destroy(wxr->bezel_quad);
    quad_destroy(wxr->screen_quad);
    quad_destroy(wxr->dots_quad);
    quad_destroy(wxr->wxr_quad);
    
    glDeleteTextures(1, &wxr->dots_tex);
    glDeleteTextures(1, &wxr->screen_tex);
    glDeleteTextures(1, &wxr->bezel_tex);
    glDeleteTextures(1, &wxr->crt_mask_tex);
    glDeleteFramebuffers(1, &wxr->screen_fbo);
    
    XPLMDestroyAvionics(wxr->device);
    nvgDeleteGL2(wxr->vg);
    free(wxr);
}
