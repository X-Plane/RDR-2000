/*===--------------------------------------------------------------------------------------------===
 * gl.h
 *
 * Created by Amy Parent <amy@amyparent.com>
 * Copyright (c) 2024 Laminar Research. All rights reserved
 *
 * Licensed under the MIT License
 *===--------------------------------------------------------------------------------------------===
*/
#ifndef _GL_H_
#define _GL_H_

#include <math.h>
#include <stdbool.h>

#include <GL/glew.h>

#ifndef NDEBUG
#define GL_DEBUG
#endif

#if IBM
#include <GL/gl.h>
// #include <GL/glu.h>
#elif LIN
#define TRUE 1
#define FALSE 0
#include <GL/gl.h>
// #include <GL/glu.h>
#else
#define TRUE 1
#define FALSE 0
#if __GNUC__
#include <OpenGL/gl.h>
// #include <OpenGL/glu.h>
#else
#include <gl.h>
// #include <glu.h>
#endif
#endif

typedef struct vect2_t {
    float x, y;
} vect2_t;

#define VECT2(xx, yy)       ((vect2_t){.x=(xx), .y=(yy)})
#define NULL_VECT2          VECT2(NAN, NAN)
#define IS_NULL_VECT2(v)    (isnan((v).x) || isnan((v).y))

GLuint gl_fbo_new(unsigned width, unsigned height, GLuint *tex);

GLuint gl_program_new(const char *vertex, const char *fragment);
GLuint gl_load_shader(const char *source, int type);
GLuint gl_load_tex(const char *path, int *w, int *h);
GLuint gl_tex_new(unsigned width, unsigned height);
void gl_ortho(float proj[16], float x, float y, float width, float height);

void check_gl(const char *where, int line);

#ifdef GL_DEBUG
#define CHECK_GL() check_gl(__FUNCTION__, __LINE__)
#else
#define CHECK_GL()
#endif

#endif /* ifndef _GL_H_ */
