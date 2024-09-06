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
    
    vec2    last_pos;
    vec2    last_size;
};


typedef struct {
    vec2    pos;
    vec2    tex;
} vertex_t;

#endif /* ifndef _RENDERER_IMPL_H_ */

