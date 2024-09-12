/*===--------------------------------------------------------------------------------------------===
 * helpers.h
 *
 * Created by Amy Parent <amy@amyparent.com>
 * Copyright (c) 2024 Laminar Research. All rights reserved
 *
 * Licensed under the MIT License
 *===--------------------------------------------------------------------------------------------===
*/
#ifndef _UTILS_HELPERS_
#define _UTILS_HELPERS_

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define UNUSED(x)   ((void)(x))
    
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef CLAMP
#define CLAMP(x, a, b)  (x <= a ? a : (x >= b ? b : x))
#endif

// Memory basics

static inline void *
safe_malloc(size_t bytes) {
    void *p = malloc(bytes);
    if(!p) {
        fprintf(stderr, "out of memory\n");
        abort();
    }
    return p;
}

static inline void *
safe_calloc(size_t count, size_t size) {
    void *p = calloc(count, size);
    if(!p) {
        fprintf(stderr, "out of memory\n");
        abort();
    }
    return p;
}

static inline void *
safe_realloc(void *ptr, size_t size) {
    void *p = realloc(ptr, size);
    if(!p) {
        fprintf(stderr, "out of memory\n");
        abort();
    }
    return p;
}

static inline char *
safe_strdup(const char *str) {
    char *copy = strdup(str);
    if(!copy) {
        fprintf(stderr, "out of memory\n");
        abort();
    }
    return copy;
}

// Basic Log

void log_init(const char *prefix, void (*fn)(const char *msg));
void log_msg(const char *fmt, ...);

// Better assert than libc

#ifndef NDEBUG
#define ASSERT(x)   (assert_impl((x), __FILE__, __LINE__, #x))
#else
#define ASSERT(x)   UNUSED(x)
#endif

void assert_impl(bool val, const char *file, unsigned line, const char *expr);
    
// Filesystem utilities

#if IBM
#define UTILS_WINDOWS
#define DIR_SEP '\\'
#else
#define DIR_SEP '/'
#endif


char *fs_make_path(const char *path, ...);
void fs_fix_path_inplace(char *path);

// String handling

void str_trim_space(char *str);
unsigned str_split_inplace(char *str, char delim, char **comps, unsigned max_comp);


#endif /* ifndef _UTILS_HELPERS_ */
