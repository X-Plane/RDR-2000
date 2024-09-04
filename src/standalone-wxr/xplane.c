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
#include <helpers/helpers.h>
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
    return 1;
}

PLUGIN_API void XPluginStop(void) {
}

PLUGIN_API int XPluginEnable(void) {
    return 1;
}

PLUGIN_API void XPluginDisable(void) {
    
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID from, int msg, void *param) {
    if(msg != XPLM_MSG_PLANE_LOADED)
        return;
    
    intptr_t acf_id = (intptr_t)param;
    if(acf_id == 0)
        get_paths();
}


