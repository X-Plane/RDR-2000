/*===--------------------------------------------------------------------------------------------===
 * helpers.c
 *
 * Created by Amy Parent <amy@amyparent.com>
 * Copyright (c) 2024 Laminar Research. All rights reserved
 *
 * Licensed under the MIT License
 *===--------------------------------------------------------------------------------------------===
*/
#include <helpers/helpers.h>
#include <stdarg.h>
#include <unistd.h>
#include <ctype.h>

static void         (*log_fn)(const char *) = NULL;
static const char   *log_prefix = "";
static size_t       log_prefix_len = 0;


void log_init(const char *prefix, void (*fn)(const char *msg)) {
    log_fn = fn;
    log_prefix = prefix;
    log_prefix_len = strlen(prefix);
}

void log_msg(const char *fmt, ...)
{
    va_list args, args_copy;
    va_start(args, fmt);
    va_copy(args_copy, args);
    
    size_t size = vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);
    
    // 1 for each bracket, one for a space, one for the newline, one for NUL
    size += log_prefix_len + 5; 
    
    char *msg = calloc(size, 1);
    size_t offset = snprintf(msg, size, "[%s] ", log_prefix);
    offset += vsnprintf(msg + offset, size - offset, fmt, args);
    va_end(args);
    msg[offset] = '\n';
    
    if(!log_fn) {
    	fprintf(stderr, "%s", msg);
    } else {
        log_fn(msg);
    }
    free(msg);
}

void assert_impl(bool val, const char *file, unsigned line, const char *expr) {
    if(val)
        return;
    log_msg("%s:%u: assertion `%s' failed\n", file, line, expr);
}

char *fs_make_path(const char *root, ...) {
    ASSERT(root != NULL);
    
    va_list args, copy;
    va_start(args, root);
    va_copy(copy, args);
    size_t len = strlen(root);
    const char *comp = NULL;
    while((comp = va_arg(args, const char *)) != NULL) {
        len += strlen(comp) + 1;
    }
    va_end(args);

    comp = NULL;
    char *path = safe_calloc(len + 1, 1);
    size_t idx = snprintf(path, 1 + len, "%s%c", root, DIR_SEP);

    while((comp = va_arg(copy, const char *)) != NULL) {
        idx += snprintf(&path[idx], 1 + len - idx, "%s%c", comp, DIR_SEP);
    }
    va_end(copy);
    
    return path;
}

char *fs_parent(const char *path) {
    ASSERT(path != NULL);
    
    const char *last_sep = strrchr(path, DIR_SEP);
    if(!last_sep)
        return NULL;
    
    size_t len = (last_sep - path);
    char *parent = safe_calloc(len, 1);
    memcpy(parent, path, len);
    parent[len] = '\0';
    return parent;
}


void fs_fix_path_inplace(char *path) {
    size_t len = 0;
    for(len = 0; path[len] != '\0'; ++len) {
#ifdef IBM
        if(path[len] == '/') {
            path[len] = '\\';
        }
#else
        if(path[len] == '\\') {
            path[len] = '/';
        }
#endif
    }
    
    if(len > 0 && path[len-1] == DIR_SEP) {
        path[len-1] = '\0';
    }
}

void str_trim_space(char *str) {
    ASSERT(str);
    
    char *start = str;
    while(*start && isspace(*start)) {
        ++start;
    }
    size_t len = strlen(start);
    memmove(str, start, len);
    
    str[len] = '\0';
    while(len && isspace(str[len-1])) {
        str[--len] = '\0';
    }
}


unsigned str_split_inplace(char *str, char delim, char **comps, unsigned max_comps) {
    ASSERT(str);
    ASSERT(comps || max_comps == 0);
    
    unsigned n = 1;
    if(max_comps) comps[0] = str;
    
    while((str = strchr(str, delim)) != NULL && (!max_comps || n < max_comps)) {
        *str = '\0';
        ++str;
        if(max_comps) comps[n++] = str;
    }
    return n;
}
