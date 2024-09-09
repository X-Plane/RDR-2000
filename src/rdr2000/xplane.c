/*===--------------------------------------------------------------------------------------------===
 * xplane.c
 *
 * Created by Amy Parent <amy@amyparent.com>
 * Copyright (c) 2024 Laminar Research. All rights reserved
 *
 * Licensed under the MIT License
 *===--------------------------------------------------------------------------------------------===
*/
#include "xplane.h"

#include "rds-81.h"

#include <glutils/gl.h>
#include <glutils/stb_image.h>
#include <helpers/helpers.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <XPLMPlanes.h>
#include <XPLMPlugin.h>
#include <XPLMProcessing.h>

#define PLUGIN_NAME         "Standalone Weather Radar"
#define PLUGIN_SIG          "com.laminar.standalone-wxr"
#define PLUGIN_DESCRIPTION  "RDS-81 standalone weather radar unit for GA"

static char xplane_dir[512];
static char plane_dir[512];
static char plugin_dir[512];

static rds81_t *wxr = NULL;


static void get_paths() {
    char name[512]; // Useless, but we have to supply a pointer for the plane's name.
    
    XPLMGetSystemPath(xplane_dir);
	XPLMGetPluginInfo(XPLMGetMyID(), NULL, plugin_dir, NULL, NULL);
    XPLMGetNthAircraftModel(XPLM_USER_AIRCRAFT, name, plane_dir);
    
#if	IBM
	fix_path_inplace(xplane_dir);
	fix_path_inplace(plugin_dir);
	fix_path_inplace(plane_dir);
#endif	/* IBM */
    
	/* cut off the trailing path component (our filename) */
    char *p = NULL;
    if((p = strrchr(plane_dir, DIR_SEP)) != NULL)
        *p = '\0';
    
    p = NULL;
	if ((p = strrchr(plugin_dir, DIR_SEP)) != NULL)
		*p = '\0';
	/*
	 * Cut off an optional '32' or '64' trailing component. Please note
	 * that XPLM 3.0 now supports OS-specific suffixes, so clamp those
	 * away as well.
	 */
	if ((p = strrchr(plugin_dir, DIR_SEP)) != NULL) {
		if (strcmp(p + 1, "64") == 0 || strcmp(p + 1, "32") == 0 ||
		    strcmp(p + 1, "win_x64") == 0 ||
		    strcmp(p + 1, "mac_x64") == 0 ||
		    strcmp(p + 1, "lin_x64") == 0)
			*p = '\0';
	}
}

const char *get_plugin_dir(void) {
    return plugin_dir;
}

const char *get_xplane_dir(void) {
    return xplane_dir;
}

const char *get_plane_dir(void) {
    return plane_dir;
}

PLUGIN_API int XPluginStart(char *name, char *sig, char *desc) {
    log_init(PLUGIN_SIG, XPLMDebugString);
    
	XPLMEnableFeature("XPLM_USE_NATIVE_PATHS", 1);
	strcpy(name, PLUGIN_NAME);
	strcpy(sig, PLUGIN_SIG);
	strcpy(desc, PLUGIN_DESCRIPTION);
    
    get_paths();
    stbi_set_flip_vertically_on_load(true);
    glewInit();
    rds81_declare_cmd_dr();
    log_msg("%s start done", PLUGIN_SIG);
    return 1;
}

static float first_flight_loop(float elapsed1, float elapsed2, int count, void *refcon) {
    UNUSED(elapsed1);
    UNUSED(elapsed2);
    UNUSED(count);
    UNUSED(refcon);
    
    if(wxr == NULL) {
        rds81_side_t side = rds81_find_best_side();
        if(side != RDS81_SIDE_NONE)
            wxr = rds81_new(side);
    }
    return 0;
}

PLUGIN_API int XPluginEnable(void) {
    XPLMRegisterFlightLoopCallback(first_flight_loop, -1.f, NULL);
    return 1;
}

PLUGIN_API void XPluginDisable(void) {
    XPLMUnregisterFlightLoopCallback(first_flight_loop, NULL);
    if(wxr)
        rds81_destroy(wxr);
    wxr = NULL;
}

PLUGIN_API void XPluginStop(void) {
    
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID from, int msg, void *param) {
    if(msg != XPLM_MSG_PLANE_LOADED)
        return;
    
    intptr_t acf_id = (intptr_t)param;
    if(acf_id == 0)
        get_paths();
}

static XPLMDataRef find_dr_v(const char *fmt, va_list args) {
    char buf[2048];
    vsnprintf(buf, sizeof(buf), fmt, args);
    XPLMDataRef ref = XPLMFindDataRef(buf);
    if(!ref)
        log_msg("cannot find dataref `%s'", buf);
    return ref;
}

XPLMDataRef find_dr_safe(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    XPLMDataRef ref = find_dr_v(fmt, args);
    va_end(args);
    ASSERT(ref != NULL);
    return ref;
}

XPLMDataRef find_dr(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    XPLMDataRef ref = find_dr_v(fmt, args);
    va_end(args);
    return ref;
}

static XPLMCommandRef find_cmd_v(const char *fmt, va_list args) {
    char buf[2048];
    vsnprintf(buf, sizeof(buf), fmt, args);
    XPLMCommandRef ref = XPLMFindCommand(buf);
    if(!ref)
        log_msg("cannot find command `%s'", buf);
    return ref;
}

XPLMCommandRef find_cmd_safe(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    XPLMCommandRef ref = find_cmd_v(fmt, args);
    va_end(args);
    ASSERT(ref != NULL);
    return ref;
}

XPLMCommandRef find_cmd(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    XPLMCommandRef ref = find_cmd_v(fmt, args);
    va_end(args);
    return ref;
}
