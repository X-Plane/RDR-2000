/*
 * CDDL HEADER START
 *
 * This file and its contents are supplied under the terms of the
 * Common Development and Distribution License ("CDDL"), version 1.0.
 * You may only use this file in accordance with the terms of version
 * 1.0 of the CDDL.
 *
 * A full copy of the text of the CDDL should have accompanied this
 * source.  A copy of the CDDL is also available via the Internet at
 * http://www.illumos.org/license/CDDL.
 *
 * CDDL HEADER END
*/
/*
 * Copyright 2020 Saso Kiselkov. All rights reserved.
 */

#include <X11/Xcursor/Xcursor.h>

#include "../cursor.h"
#include <helpers/helpers.h>
#include <glutils/stb_image.h>
#include <XPLMDataAccess.h>

struct cursor_s {
	Cursor		crs;
};

/*
 * Because cursors are only ever created & used from the main rendering
 * thread, it is safe to use a simple unprotected global var with a refcount.
 */
static int	dpy_refcount = 0;
static Display	*dpy = NULL;

cursor_t *
cursor_read_from_file(const char *filename_png)
{
	cursor_t *cursor;
	uint8_t *buf;
	int w, h;
    int components = 0;
	XcursorImage img = { .pixels = NULL };

	ASSERT(filename_png != NULL);

    buf = stbi_load(filename_png, &w, &h, &components, 4);
	if (buf == NULL)
		return (NULL);

	if (dpy_refcount == 0)
		dpy = XOpenDisplay(NULL);
	if (dpy == NULL) {
		log_msg("Can't open display");
		free(buf);
		return (NULL);
	}
	dpy_refcount++;

	img.size = w;
	img.width = w;
	img.height = h;
	img.xhot = w / 2;
	img.yhot = h / 2;
	img.pixels = (XcursorPixel *)buf;
	cursor = safe_calloc(1, sizeof (*cursor));
	cursor->crs = XcursorImageLoadCursor(dpy, &img);

	free(buf);

	return (cursor);
}

void
cursor_free(cursor_t *cursor)
{
	if (cursor == NULL)
		return;

	ASSERT(dpy != NULL);
	XFreeCursor(dpy, cursor->crs);
	free(cursor);

	dpy_refcount--;
	ASSERT(dpy_refcount >= 0);
	if (dpy_refcount == 0) {
		XCloseDisplay(dpy);
		dpy = NULL;
	}
}

void
cursor_make_current(cursor_t *cursor)
{
	XPLMDataRef system_window_dr = NULL;
	int win_ptr[2];
	Window win;

	ASSERT(cursor != NULL);
	ASSERT(dpy != NULL);
	system_window_dr = XPLMFindDataRef("sim/operation/windows/system_window_64");
    ASSERT(system_window_dr != NULL);
	ASSERT(XPLMGetDatavi(system_window_dr, win_ptr, 0, 2) == 2);
	memcpy(&win, win_ptr, sizeof (void *));

	XDefineCursor(dpy, win, cursor->crs);
	XFlush(dpy);
}
