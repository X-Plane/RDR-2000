/*===--------------------------------------------------------------------------------------------===
 * renderer_impl.h - Renderer implementation details
 *
 * Created by Amy Parent <amy@amyparent.com>
 * Copyright (c) 2022 Amy Parent. All rights reserved
 *
 * NOTICE:  All information contained herein is, and remains the property
 * of Amy Alex Parent. The intellectual and technical concepts contained
 * herein are proprietary to Amy Alex Parent and may be covered by U.S. and
 * Foreign Patents, patents in process, and are protected by trade secret
 * or copyright law. Dissemination of this information or reproduction of
 * this material is strictly forbidden unless prior written permission is
 * obtained from Amy Alex Parent.
 *===--------------------------------------------------------------------------------------------===
*/
#ifndef _RENDERER_IMPL_H_
#define _RENDERER_IMPL_H_

#include <glutils/gl.h>
#include <glutils/renderer.h>
#include <helpers/helpers.h>

struct gl_quad_t {
    GLuint vbo;
    GLuint ibo;
    GLuint shader;
    GLuint tex;
    
    struct {
        int vtx_pos;
        int vtx_tex0;
        int pvm;
        int tex;
        int alpha;
    } loc;
    
    vect2_t last_pos;
    vect2_t last_size;
};

struct target_t {
    vect2_t size;
    vect2_t offset;
    float proj[16];
};

typedef struct {
    float x;
    float y;
} vec2f_t;

typedef struct {
    vec2f_t pos;
    vec2f_t tex;
} vertex_t;

#endif /* ifndef _RENDERER_IMPL_H_ */

