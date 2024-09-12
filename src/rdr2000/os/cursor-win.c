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

#include <windows.h>
#include "../cursor.h"
#include <helpers/helpers.h>
#include <stdarg.h>

struct cursor_s {
	HCURSOR		crs;
};

void
strlcpy(char *restrict dest, const char *restrict src, size_t cap)
{
	size_t l;

	ASSERT(cap != 0);
	/*
	 * We MUSTN'T use strlen here, because src may be SIGNIFICANTLY
	 * larger than dest and we don't want to measure the ENTIRE body
	 * of src. We only care for length UP TO the destination capacity.
	 */
	for (l = 0; l + 1 < cap && src[l] != '\0'; l++)
		;
	/*
	 * Due to a bug in GCC, we can't use strncpy, as it sometimes throws
	 * "call to __builtin___strncpy_chk will always overflow destination
	 * buffer", even when it's absolutely NOT the case.
	 */
	memcpy(dest, src, MIN(cap - 1, l + 1));
	/* Insure the string is ALWAYS terminated */
	dest[cap - 1] = '\0';
}

void
win_perror(DWORD err, const char *fmt, ...)
{
	va_list ap;
	LPSTR win_msg = NULL;
	char *caller_msg;
	int len;

	va_start(ap, fmt);
	len = vsnprintf(NULL, 0, fmt, ap);
	va_end(ap);
	caller_msg = safe_malloc(len + 1);
	va_start(ap, fmt);
	vsnprintf(caller_msg, len + 1, fmt, ap);
	va_end(ap);

	(void) FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
	    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
	    NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	    (LPSTR)&win_msg, 0, NULL);
	str_trim_space(win_msg);
	log_msg("%s: %s", caller_msg, win_msg);

	free(caller_msg);
	LocalFree(win_msg);
}

cursor_t *
cursor_read_from_file(const char *filename_png)
{
	cursor_t *cursor = safe_calloc(1, sizeof (*cursor));
	char *filename_cur, *extension;

	ASSERT(filename_png != NULL);

	/*
	 * On Windows, we need to grab a cursor file (.cur), so substitute
	 * the path extension in the filename.
	 */
	filename_cur = safe_calloc(1, strlen(filename_png) + 8);
	strlcpy(filename_cur, filename_png, strlen(filename_png) + 8);
	extension = strrchr(filename_cur, '.');
	if (extension != NULL)
		strlcpy(extension, ".cur", 8);
	else
		strlcpy(&filename_cur[strlen(filename_cur)], ".cur", 8);
	cursor->crs = LoadCursorFromFileA(filename_cur);
	if (cursor->crs == NULL) {
		win_perror(GetLastError(), "Error loading cursor file %s",
		    filename_cur);
		free(cursor);
		free(filename_cur);
		return (NULL);
	}
	free(filename_cur);

	return (cursor);
}

void
cursor_free(cursor_t *cursor)
{
	if (cursor == NULL)
		return;
	ASSERT(cursor->crs != NULL);
	DestroyCursor(cursor->crs);
	free(cursor);
}

void
cursor_make_current(cursor_t *cursor)
{
	ASSERT(cursor != NULL);
	ASSERT(cursor->crs != NULL);
	SetCursor(cursor->crs);
}
