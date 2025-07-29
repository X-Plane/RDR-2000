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
#include <XPLMMenus.h>
#include <cglm/mat4.h>
#define NANOVG_GL2_IMPLEMENTATION
#include <nanovg_gl.h>
#include <time.h>

rds81_t *wxr = NULL;

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

static float remap(float x, float a1, float b1, float a2, float b2) {
    return a2 + (x - a1) * (b2 - a2) / (b1 - a1);
}

static void rds_draw_knobs(rds81_t *wxr, mat4 pvm) {
    for(int i = 0; i < KNOB_COUNT; ++i) {
        knob_t *knob = &wxr->knobs[i];

        float val = knob->desc->type == KNOB_INT ?
            XPLMGetDatai(knob->val) : XPLMGetDataf(knob->val);
        float angle = remap(val,
            knob->desc->min, knob->desc->max,
            knob->desc->min_angle, knob->desc->max_angle);
        
        
        vec2 pos = {knob->desc->pos[0], knob->desc->pos[1]};
        vec2 size = {knob->desc->size[0] * RDS_SCALE, knob->desc->size[1] * RDS_SCALE};
        quad_render(pvm, knob->quad, pos, size, -angle, 1.f);
    }
}

static void rds_draw_bezel(float r, float g, float b, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    XPLMSetGraphicsState(0, 1, 0, 1, 1, 0, 0);
    mat4 pvm;
    rds_get_xp_pvm(wxr, pvm);
    // glCullFace(GL_BACK);
    quad_render(pvm, wxr->bezel_quad, VEC2(0, 0), VEC2(RDS_BEZEL_W * RDS_SCALE, RDS_BEZEL_H * RDS_SCALE), 0.f, 1.f);
    rds_draw_knobs(wxr, pvm);
}

#define WXR_CTR_X   (160*2)
#define WXR_CTR_Y   (17*2)

#define WXR_H       (205*2)
#define WXR_W       (290*2)

#define WXR_POS_X   (WXR_CTR_X - (WXR_W/2))
#define WXR_POS_Y   (WXR_CTR_Y)

static void draw_fbo(rds81_t *wxr, NVGcontext *vg, mat4 pvm) {
    float full_range = XPLMGetDataf(wxr->dr_range);
    int stab = XPLMGetDatai(wxr->dr_stab);
    
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    if(wxr->mode > RDS81_MODE_STBY) {
        glUseProgram(wxr->shader_wxr);
        // glUniform1f(glGetUniformLocation(wxr->shader_wxr, "blink"), blink);
        quad_set_shader(wxr->wxr_quad, wxr->shader_wxr);
        quad_render(pvm, wxr->wxr_quad, VEC2(WXR_POS_X, WXR_POS_Y), VEC2(WXR_W, WXR_H), 0.f, 1.f);
        quad_render(pvm, wxr->dots_quad, VEC2(0, 0), VEC2(RDS_SCREEN_W, RDS_SCREEN_H), 0.f, 1.f);
    }
    
    nvgFontSize(vg, 30.f);
    nvgFontFace(vg, "default");
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE);
    
    // Draw Range Info
    if(wxr->mode > RDS81_MODE_STBY) {
        nvgFillColor(vg, nvgRGB(0, 255, 255));
    
        static const vec2 rng_pos[4] = {
            {RDS_SCREEN_W/2 + 40, WXR_H-20},
            {RDS_SCREEN_W/2 + 130, WXR_H-100},
            {RDS_SCREEN_W/2 + 180, WXR_H-170},
            {RDS_SCREEN_W/2 + 170, WXR_H-310},
        };
    
        for(int i = 0; i < 4; ++i) {
            char buf[32];
            snprintf(buf, sizeof(buf), "%02.0f", full_range * (float)(i+1) / 4.f);
            nvgText(vg, rng_pos[i][0] + 60, rng_pos[i][1], buf, NULL);
        }
    
        // Draw Tilt info
        nvgFontSize(vg, 30.f);
        nvgFillColor(vg, nvgRGB(255, 255, 0));
        float tilt = XPLMGetDataf(wxr->dr_tilt);
        char buf[32];
        if(round(tilt * 10) == 0) {
            nvgText(vg, RDS_SCREEN_W/2 + 240, WXR_H-350, "0°", NULL);
        } else {
            snprintf(buf, sizeof(buf), "%c %4.1f°", tilt > 0 ? 'U' : 'D', fabs(tilt));
            nvgText(vg, RDS_SCREEN_W/2 + 195, WXR_H-350, buf, NULL);
        }
    }
    
    // Draw Weather Mode
    if(wxr->mode != RDS81_MODE_OFF) {
        const char *mode_str = "STBY";
        switch(wxr->mode) {
        case RDS81_MODE_OFF:
        case RDS81_MODE_STBY: mode_str = "STBY"; break;
        case RDS81_MODE_TEST: mode_str = "TEST"; break;
        case RDS81_MODE_ON:
            if(wxr->submode == RDS81_SUBMODE_WX)
                mode_str = "WX";
            else if(wxr->submode == RDS81_SUBMODE_WXA)
                mode_str = "WXA";
            else if(wxr->submode == RDS81_SUBMODE_MAP)
                mode_str = "MAP";
            break;
        }
        nvgFillColor(vg, nvgRGB(0, 255, 255));
        nvgText(vg, WXR_POS_X+20, WXR_H-40, mode_str, NULL);
    }
    
    // Draw Stab Info
    if(wxr->mode > RDS81_MODE_STBY && stab != 1) {
        nvgFillColor(vg, nvgRGB(0, 255, 255));
        nvgText(vg, WXR_POS_X+20, WXR_H-340, "STAB OFF", NULL);
    }
}

static void rds_update_wxr_tex(int src_tex, int shader) {
    quad_set_tex(wxr->src_quad, src_tex);
    glBindFramebuffer(GL_FRAMEBUFFER, wxr->wxr_fbo);
    glViewport(0, 0, RDS_WXR_BUF_W, RDS_WXR_BUF_H);
    
    if(wxr->ant_clear || !wxr->is_warm) {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        wxr->ant_clear = false;
        return;
    }
    

    mat4 ortho;
    glm_ortho(0, RDS_WXR_BUF_W, 0, RDS_WXR_BUF_H, -1, 1, ortho);
    
    // Get the data we need
    float full_range = XPLMGetDataf(wxr->dr_range);
    
    glUseProgram(shader);
    glBindTexture(GL_TEXTURE_2D, wxr->crt_mask_tex);
    glUniform2f(glGetUniformLocation(shader, "aspect"), (float)RDS_WXR_BUF_W/(float)RDS_WXR_BUF_H, 1.f);
    glUniform1f(glGetUniformLocation(shader, "ant_lim"), DEG2RAD(RDS_ANT_LIM));
    glUniform1f(glGetUniformLocation(shader, "range"), full_range);
    glUniform1f(glGetUniformLocation(shader, "ant_offset"), -(float)wxr->ant_dir);
    if(wxr->ant_angle > wxr->ant_angle_last) {
        glUniform1f(glGetUniformLocation(shader, "angle_start"), DEG2RAD(wxr->ant_angle_last));
        glUniform1f(glGetUniformLocation(shader, "angle_end"), DEG2RAD(wxr->ant_angle));
    } else {
        glUniform1f(glGetUniformLocation(shader, "angle_start"), DEG2RAD(wxr->ant_angle));
        glUniform1f(glGetUniformLocation(shader, "angle_end"), DEG2RAD(wxr->ant_angle_last));
    }

    quad_set_shader(wxr->src_quad, shader);
    quad_render(ortho, wxr->src_quad, VEC2(0, 0), VEC2(RDS_WXR_BUF_W, RDS_WXR_BUF_H), 0.f, 1.f);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static float ease_out_cubic(float t)
{
	float f = (t - 1);
	return f * f * f + 1;
}

static void rds_draw_screen(void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    
    rds81_update(wxr);
    
    // Save XP data
    int old_vp[4];
    int old_fbo = XPLMGetDatai(wxr->dr_fbo);
    XPLMGetDatavi(wxr->dr_viewport, old_vp, 0, 4);

    XPLMSetGraphicsState(0, 2, 0, 1, 1, 0, 0);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    int src_wxr = XPLMGetTexture(wxr->wxr_tex_id);
    rds_update_wxr_tex(src_wxr, wxr->mode == RDS81_MODE_TEST ? wxr->shader_test : wxr->shader_ant);
    
    if(wxr->mode > RDS81_MODE_OFF && rds81_has_power(wxr)) {
        mat4 ortho;
        glm_ortho(0, RDS_SCREEN_W, 0, RDS_SCREEN_H, -1, 1, ortho);
        glBindFramebuffer(GL_FRAMEBUFFER, wxr->screen_fbo);
        glViewport(0, 0, RDS_SCREEN_W/2, RDS_SCREEN_H/2);
    
        nvgBeginFrame(wxr->vg, RDS_SCREEN_W, RDS_SCREEN_H, 2);
        draw_fbo(wxr, wxr->vg, ortho);
        nvgEndFrame(wxr->vg);
    
        // Revert to how things were before we mucked with OpenGL state
        glBindFramebuffer(GL_FRAMEBUFFER, old_fbo);
        glViewport(old_vp[0], old_vp[1], old_vp[2], old_vp[3]);
    
    
        // For the "turning on" animation, we compute the time since we turned on, then use that
        // to simulate "warmup" (AKA the alpha slowly ramps up, and the dispaly "zooms in".)
        double time_since_on = time_get_clock() - wxr->on_time;
        float t = CLAMP(time_since_on / RDS_WARMUP_SCALE, 0.f, 1.f);
        float scale = 0.1f + 0.9f * ease_out_cubic(t);
        

        float blink = 1.f;
        if(wxr->submode == RDS81_SUBMODE_WXA) {
            blink = (int)(time_since_on * 2.f) % 2;
        }
        
        mat4 pvm;
        rds_get_xp_pvm(wxr, pvm);
        glUseProgram(wxr->shader_screen);
        
        XPLMBindTexture2d(wxr->screen_tex, 0);
        XPLMBindTexture2d(wxr->crt_mask_tex, 1);
        glUniform1f(glGetUniformLocation(wxr->shader_screen, "blink"), blink);
        glUniform1i(glGetUniformLocation(wxr->shader_screen, "mask"), 1);
        glUniform1f(glGetUniformLocation(wxr->shader_screen, "scale"), scale);
        
        quad_set_shader(wxr->screen_quad, wxr->shader_screen);
        quad_render(pvm, wxr->screen_quad, VEC2(0, 0), VEC2(RDS_SCREEN_W * RDS_SCALE, RDS_SCREEN_H * RDS_SCALE), 0.f, 1.f);
        
        XPLMBindTexture2d(0, 0);
        XPLMBindTexture2d(0, 1);
    }
}

static int rds_click_bezel(int x, int y, XPLMMouseStatus mouse, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    
    switch(mouse) {
    case xplm_MouseDown:
        rds81_click_down(wxr, (vec2){x, y});
        break;
    case xplm_MouseUp:
        rds81_click_release(wxr);
        break;
    }
    return wxr->act_cmd != NULL;
}

static int rds_scroll_bezel(int x, int y, int wheel, int clicks, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    
    if(wheel != 0)
        return 0;
    
    return rds81_scroll(wxr, (vec2){x, y}, clicks);
}

static XPLMCursorStatus rds_cursor_bezel(int x, int y, void *refcon) {
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    return rds81_cursor(wxr, (vec2){x, y}) ? xplm_CursorCustom : xplm_CursorDefault;
}

static float rds_brightness(float rheo, float ambiant, float bus, void *refcon) {
    UNUSED(refcon);
    rds81_t *wxr = refcon;
    ASSERT(wxr != NULL);
    
    double time_since_on = time_get_clock() - wxr->on_time;
    float alpha = 0.2f + 0.8f * CLAMP(powf(time_since_on / RDS_WARMUP_ALPHA, 2), 0.f, 1.f);
    
    return rds81_has_power(wxr) ? 
        alpha * (0.01f + rheo * 1.5f * ambiant) : 0.f;
}

// MARK: - "public" API

GLuint rds81_load_shader(const char *name) {
    
    char fname_vert[64];
    char fname_frag[64];
    
    if(GLEW_VERSION_4_2) {
        snprintf(fname_vert, sizeof(fname_vert), "%s.vert.420", name);
        snprintf(fname_frag, sizeof(fname_frag), "%s.frag.420", name);
    } else {
        snprintf(fname_vert, sizeof(fname_vert), "%s.vert.120", name);
        snprintf(fname_frag, sizeof(fname_frag), "%s.frag.120", name);
    }
    
    char *vert_path = fs_make_path(get_plugin_dir(), "resources", "shaders", fname_vert, NULL);
    char *frag_path = fs_make_path(get_plugin_dir(), "resources", "shaders", fname_frag, NULL);
    
    GLuint shader = gl_program_new_file(vert_path, frag_path);
    
    free(vert_path);
    free(frag_path);
    
    return shader;
}


static void rds81_reload_shaders() {
    if(wxr == NULL)
        return;
    
    if(wxr->shader_wxr)
        glDeleteProgram(wxr->shader_wxr);
    if(wxr->shader_screen)
        glDeleteProgram(wxr->shader_screen);
    if(wxr->shader_ant)
        glDeleteProgram(wxr->shader_ant);
    if(wxr->shader_test)
        glDeleteProgram(wxr->shader_test);
    
    wxr->shader_wxr = rds81_load_shader("wxr_copy");
    wxr->shader_screen = rds81_load_shader("rdr_screen");
    wxr->shader_ant = rds81_load_shader("wxr_antenna");
    wxr->shader_test = rds81_load_shader("wxr_test");
}

GLuint rds81_load_tex(const char *name) {
    char *path = fs_make_path(get_plugin_dir(), "resources", name, NULL);
    int w = 0, h = 0;
    GLuint tex = gl_load_tex(path, &w, &h);
    if(tex == 0)
        log_msg("could not load texture `%s'", path);
    free(path);
    return tex;
}

cursor_t* rds81_load_cursor(const char *name) {
    char *path = fs_make_path(get_plugin_dir(), "resources", name, NULL);
    cursor_t *cur = cursor_read_from_file(path);
    if(cur == NULL)
        log_msg("could not load cursor `%s'", path);
    free(path);
    return cur;
}

#ifdef RDS_DEBUG_SHADERS
static int reload_shaders_cmd(XPLMCommandRef ref, XPLMCommandPhase phase, void *ptr) {
    UNUSED(ref);
    UNUSED(phase);
    UNUSED(ptr);
    
    if(wxr != NULL) {
        rds81_reload_shaders();
    }
    return 1;
}
#endif

// This *must* run in XPPluginStart, not enable, so OBJ can bind to our commands and datarefs.

void rds81_init(rds81_side_t side) {
    if(wxr != NULL)
        return;
    wxr = safe_calloc(1, sizeof(*wxr));
    wxr->vg = nvgCreateGL2(NVG_ANTIALIAS);
    
    // char *font_path = fs_make_path(get_plugin_dir(), "resources", "MonomaniacOne-Regular.ttf", NULL);
    char *font_path = fs_make_path(get_plugin_dir(), "resources", "Roboto-Bold.ttf", NULL);
    int res = nvgCreateFont(wxr->vg, "default", font_path);
    free(font_path);
    log_msg("font load: %d", res);
    
    wxr->wxr_tex_id = side == RDS81_SIDE_COPILOT ? xplm_Tex_Radar_Copilot : xplm_Tex_Radar_Pilot;
    
    // Gather all the datarefs we need to make things work
    const char *side_str = side == RDS81_SIDE_COPILOT ? "_copilot" : "";
    wxr->dr_proj_mat = find_dr_safe("sim/graphics/view/projection_matrix");
    wxr->dr_mv_mat = find_dr_safe("sim/graphics/view/modelview_matrix");
    wxr->dr_fbo = find_dr_safe("sim/graphics/view/current_gl_fbo");
    wxr->dr_viewport = find_dr_safe("sim/graphics/view/viewport");
    
    wxr->dr_avionics_power = find_dr_safe("sim/cockpit2/switches/avionics_power_on");
    
    wxr->dr_mode = find_dr_safe("sim/cockpit2/EFIS/EFIS_weather_mode%s", side_str);
    wxr->dr_tilt = find_dr_safe("sim/cockpit2/EFIS/EFIS_weather_tilt", side_str);
    wxr->dr_tilt_antenna = find_dr_safe("sim/cockpit2/EFIS/EFIS_weather_tilt_antenna%s", side_str);
    wxr->dr_auto_tilt = find_dr_safe("sim/cockpit2/EFIS/EFIS_weather_auto_tilt%s", side_str);
    wxr->dr_gain = find_dr_safe("sim/cockpit2/EFIS/EFIS_weather_gain%s", side_str);
    
    wxr->dr_stab = find_dr_safe("sim/cockpit2/EFIS/EFIS_weather_stab%s", side_str);
    wxr->dr_gcs = find_dr_safe("sim/cockpit2/EFIS/EFIS_weather_gcs%s", side_str);
    wxr->dr_pws = find_dr_safe("sim/cockpit2/EFIS/EFIS_weather_pws");
    wxr->dr_multiscan = find_dr_safe("sim/cockpit2/EFIS/EFIS_weather_multiscan%s", side_str);
    wxr->dr_vertical = find_dr_safe("sim/cockpit2/EFIS/EFIS_weather_vertical%s", side_str);
    
    wxr->dr_sector_brg = find_dr_safe("sim/cockpit2/EFIS/EFIS_weather_sector_brg");
    wxr->dr_sector_width = find_dr_safe("sim/cockpit2/EFIS/EFIS_weather_sector_width");
    wxr->dr_antenna_limit = find_dr_safe("sim/cockpit2/EFIS/EFIS_weather_antenna_limit");
    
    wxr->dr_range_idx = find_dr_safe("sim/cockpit2/EFIS/map_range%s", side_str);
    wxr->dr_range = find_dr_safe("sim/cockpit2/EFIS/map_range_nm%s", side_str);
    
    // Bind our own commands
    XPLMRegisterCommandHandler(wxr_out.cmd_popup, handle_popup, 0, wxr);
    XPLMRegisterCommandHandler(wxr_out.cmd_popout, handle_popout, 0, wxr);
    
    rds81_bind_commands(wxr);
    
    // Allocate the OpenGL resources we need
    rds81_reload_shaders();
    
    wxr->wxr_fbo = gl_fbo_new(RDS_WXR_BUF_W, RDS_WXR_BUF_H, &wxr->wxr_tex);
    wxr->screen_fbo = gl_fbo_new(RDS_SCREEN_W/2, RDS_SCREEN_H/2, &wxr->screen_tex);
    wxr->bezel_tex = rds81_load_tex("bezel.png");
    wxr->dots_tex = rds81_load_tex("dots.png");
    wxr->crt_mask_tex = rds81_load_tex("crt_mask.png");
    
    wxr->src_quad = quad_new(0, wxr->shader_ant);
    wxr->bezel_quad = quad_new(wxr->bezel_tex, 0);
    wxr->screen_quad = quad_new(wxr->screen_tex, wxr->shader_screen);
    wxr->dots_quad = quad_new(wxr->dots_tex, 0);
    wxr->wxr_quad = quad_new(wxr->wxr_tex, wxr->shader_wxr);
    
    wxr->cur_click = rds81_load_cursor("cursor_click.png");
    wxr->cur_rotate_left = rds81_load_cursor("cursor_rot_left.png");
    wxr->cur_rotate_right = rds81_load_cursor("cursor_rot_right.png");
    
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
        
        .bezelClickCallback = rds_click_bezel,
        .bezelScrollCallback = rds_scroll_bezel,
        .bezelCursorCallback = rds_cursor_bezel,
        
        .brightnessCallback = rds_brightness,
        
        .deviceID = DEVICE_ID,
        .deviceName = DEVICE_NAME,
        .refcon = wxr,
    };
    wxr->device = XPLMCreateAvionicsEx(&desc);
    ASSERT(wxr->device != NULL);
    
    rds81_init_kn_butt(wxr);
    
    wxr->mode = RDS81_MODE_OFF;
    wxr->submode = RDS81_SUBMODE_WX;
    wxr->stab = true;
    wxr->map_gain = 0.5f;
    
    wxr->ant_angle = -RDS_ANT_LIM;
    wxr->ant_angle_last = -RDS_ANT_LIM;
    wxr->ant_dir = 1;
    wxr->ant_clear = true;
    
    rds81_reset_datarefs(wxr);
    
#ifdef RDS_DEBUG_SHADERS
    wxr->reload_shaders_cmd = XPLMCreateCommand("rdr200/reload_wxr_shaders", "Reload WXR shaders");
    XPLMRegisterCommandHandler(wxr->reload_shaders_cmd, reload_shaders_cmd, 0, wxr);
    
    XPLMMenuID acf = XPLMFindAircraftMenu();
    wxr->reload_shaders_menu = XPLMAppendMenuItemWithCommand(acf, "Reload WXR Shaders", wxr->reload_shaders_cmd);
#endif
}

void rds81_fini() {
    if(wxr == NULL)
        return;
    
    
#ifdef RDS_DEBUG_SHADERS
    XPLMMenuID acf = XPLMFindAircraftMenu();
    XPLMRemoveMenuItem(acf, wxr->reload_shaders_menu);
    XPLMUnregisterCommandHandler(wxr->reload_shaders_cmd, reload_shaders_cmd, 0, wxr);
#endif
    
    rds81_fini_kn_butt(wxr);
    rds81_unbind_commands(wxr);
    XPLMUnregisterCommandHandler(wxr_out.cmd_popup, handle_popup, 0, wxr);
    XPLMUnregisterCommandHandler(wxr_out.cmd_popout, handle_popout, 0, wxr);
    
    quad_destroy(wxr->bezel_quad);
    quad_destroy(wxr->screen_quad);
    quad_destroy(wxr->dots_quad);
    quad_destroy(wxr->wxr_quad);
    quad_destroy(wxr->src_quad);
    
    glDeleteProgram(wxr->shader_screen);
    glDeleteProgram(wxr->shader_wxr);
    glDeleteProgram(wxr->shader_ant);
    glDeleteProgram(wxr->shader_test);
    
    glDeleteTextures(1, &wxr->wxr_tex);
    glDeleteTextures(1, &wxr->dots_tex);
    glDeleteTextures(1, &wxr->screen_tex);
    glDeleteTextures(1, &wxr->bezel_tex);
    glDeleteTextures(1, &wxr->crt_mask_tex);
    glDeleteFramebuffers(1, &wxr->wxr_fbo);
    glDeleteFramebuffers(1, &wxr->screen_fbo);
    
    if(wxr->cur_click)
        cursor_free(wxr->cur_click);
    if(wxr->cur_rotate_left)
        cursor_free(wxr->cur_rotate_left);
    if(wxr->cur_rotate_right)
        cursor_free(wxr->cur_rotate_right);
    
    XPLMDestroyAvionics(wxr->device);
    nvgDeleteGL2(wxr->vg);
    wxr = NULL;
    free(wxr);
}
