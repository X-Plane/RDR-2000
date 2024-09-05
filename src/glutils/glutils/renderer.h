/*===--------------------------------------------------------------------------------------------===
 * renderer.h
 *
 * Created by Amy Parent <amy@amyparent.com>
 * Copyright (c) 2024 Laminar Research. All rights reserved
 *
 * Licensed under the MIT License
 *===--------------------------------------------------------------------------------------------===
*/
#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <glutils/gl.h>

typedef struct gl_quad_t gl_quad_t;

gl_quad_t *quad_new(unsigned texture, unsigned shader);
void quad_set_tex(gl_quad_t *quad, unsigned tex);
void quad_destroy(gl_quad_t *quad);

void quad_render(float pvm[16], gl_quad_t *quad, vect2_t pos, vect2_t size, double alpha);

#endif /*_RENDERER_H_ */
