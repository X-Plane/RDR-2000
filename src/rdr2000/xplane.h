/*===--------------------------------------------------------------------------------------------===
 * xplane.h
 *
 * Created by Amy Parent <amy@amyparent.com>
 * Copyright (c) 2024 Laminar Research. All rights reserved
 *
 * Licensed under the MIT License
 *===--------------------------------------------------------------------------------------------===
*/
#ifndef _XPLANE_H_
#define _XPLANE_H_

#include <XPLMDataAccess.h>
#include <XPLMDefs.h>
#include <XPLMUtilities.h>
#include <stdbool.h>

PLUGIN_API int XPluginStart(char *name, char *sig, char *desc);
PLUGIN_API void XPluginStop(void);
PLUGIN_API int XPluginEnable(void);
PLUGIN_API void XPluginDisable(void);
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID from, int msg, void *param);

const char *get_plugin_dir(void);
const char *get_xplane_dir(void);
const char *get_plane_dir(void);

XPLMDataRef find_dr_safe(const char *fmt, ...);
XPLMDataRef find_dr(const char *fmt, ...);
XPLMCommandRef find_cmd_safe(const char *fmt, ...);
XPLMCommandRef find_cmd(const char *fmt, ...);

XPLMDataRef create_dr_i(int *val, bool writeable, const char *fmt, ...);
XPLMDataRef create_dr_f(float *val, bool writeable, const char *fmt, ...);

#endif /* ifndef _XPLANE_H_ */
