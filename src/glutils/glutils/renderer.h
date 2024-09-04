/*===--------------------------------------------------------------------------------------------===
 * renderer.h
 *
 * Created by Amy Parent <amy@amyparent.com>
 * Copyright (c) 2024 Laminar Research. All rights reserved
 *
 * Licensed under the MIT License
 *===--------------------------------------------------------------------------------------------===
*/
#ifndef _HELPERS_H_
#define _HELPERS_H_

#include <glutils/gl.h>

typedef struct gl_quad_t gl_quad_t;
typedef struct target_t target_t;

gl_quad_t *quad_new(unsigned texture, unsigned shader);
void quad_set_tex(gl_quad_t *quad, unsigned tex);
void quad_destroy(gl_quad_t *quad);

target_t *target_new(double x, double y, double width, double height);
void target_set_size(target_t *target, double width, double height);
void target_set_offset(target_t *target, double x, double y);
void target_destroy(target_t *target);

void render_init();
void render_fini();
void render_quad(target_t *target, gl_quad_t *quad, vect2_t pos, vect2_t size, double alpha);

#endif /*_HELPERS_H_ */
