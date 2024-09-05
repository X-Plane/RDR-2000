//===--------------------------------------------------------------------------------------------===
// display_render.c - OpenGL 2D rendering functions for display.c
//
// Created by Amy Parent <amy@amyparent.com>
// Copyright (c) 2021 Amy Parent
// Licensed under the MIT License
// =^•.•^=
//===--------------------------------------------------------------------------------------------===
#include "glutils_impl.h"
// #include <acfutils/assert.h>
// #include <acfutils/safe_alloc.h>
#include <helpers/helpers.h>
#include <stddef.h>

static const char *vert_shader =
    "#version 120\n"
    "uniform mat4       pvm;\n"
    "attribute vec3     vtx_pos;\n"
    "attribute vec2     vtx_tex0;\n"
    "varying vec2       tex_coord;\n"
    "void main() {\n"
    "   tex_coord = vtx_tex0;\n"
    "   gl_Position = pvm * vec4(vtx_pos, 1.0);\n"
    "}\n";

static const char *frag_shader =
    "#version 120\n"
    "uniform sampler2D  tex;\n"
    "uniform float      alpha;\n"
    "varying vec2       tex_coord;\n"
    "void main() {\n"
    "   gl_FragColor = texture2D(tex, tex_coord);\n"
    "   gl_FragColor.a *= alpha;\n"
    "}\n";

void quad_init(gl_quad_t *quad, unsigned tex, unsigned shader) {
    quad->last_pos = NULL_VECT2;
    quad->last_size = NULL_VECT2;
    
    quad->tex = tex;
    if(shader) {
        quad->shader = shader;
    } else {
        quad->shader = gl_program_new(vert_shader, frag_shader);
    }
    
    // glGenVertexArrays(1, &quad->vao);
    // glBindVertexArray(quad->vao);
    
    glGenBuffers(1, &quad->vbo);
    glGenBuffers(1, &quad->ibo);
    
    static const GLuint indices[] = {0, 2, 1, 0, 3, 2};
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glUseProgram(quad->shader);
    quad->loc.pvm = glGetUniformLocation(quad->shader, "pvm");
    quad->loc.tex = glGetUniformLocation(quad->shader, "tex");
    quad->loc.alpha = glGetUniformLocation(quad->shader, "alpha");
    
    quad->loc.vtx_pos = glGetAttribLocation(quad->shader, "vtx_pos");
    quad->loc.vtx_tex0 = glGetAttribLocation(quad->shader, "vtx_tex0");
    
    // glBindVertexArray(0);
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    // XPLMBindTexture2d(0, 0);
}

void quad_set_tex(gl_quad_t *quad, unsigned tex) {
    quad->tex = tex;
}

void quad_fini(gl_quad_t *quad) {
    glDeleteBuffers(1, &quad->vbo);
    glDeleteBuffers(1, &quad->ibo);
}

gl_quad_t *quad_new(unsigned tex, unsigned shader) {
    gl_quad_t *quad = safe_calloc(1, sizeof(*quad));
    quad_init(quad, tex, shader);
    return quad;
}

void quad_destroy(gl_quad_t *quad) {
    quad_fini(quad);
    free(quad);
}

static inline bool vec2_eq(vect2_t a, vect2_t b) {
    return a.x == b.x && a.y == b.y;
}

static void prepare_vertices(gl_quad_t *quad, vect2_t pos, vect2_t size) {
    if(vec2_eq(quad->last_pos, pos) && vec2_eq(quad->last_size, size))
        return;
    vertex_t vert[4];
    
    vert[0].pos.x = pos.x;
    vert[0].pos.y = pos.y;
    vert[0].tex = (vec2f_t){0, 1};

    vert[1].pos.x = pos.x + size.x;
    vert[1].pos.y = pos.y;
    vert[1].tex = (vec2f_t){1, 1};

    vert[2].pos.x = pos.x + size.x;
    vert[2].pos.y = pos.y + size.y;
    vert[2].tex = (vec2f_t){1, 0};

    vert[3].pos.x = pos.x;
    vert[3].pos.y = pos.y + size.y;
    vert[3].tex = (vec2f_t){0, 0};
    
    glBindBuffer(GL_ARRAY_BUFFER, quad->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);
    CHECK_GL();
    
    quad->last_pos = pos;
    quad->last_size = size;
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void quad_render(float pvm[16], gl_quad_t *quad, vect2_t pos, vect2_t size, double alpha) {
    ASSERT(quad);
    ASSERT(pvm);
    
// #if APL
//     glDisableClientState(GL_VERTEX_ARRAY);
// #endif
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, quad->tex);
    
    prepare_vertices(quad, pos, size);
    
    glUseProgram(quad->shader);
    glBindBuffer(GL_ARRAY_BUFFER, quad->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad->ibo);
    
    glEnableVertexAttribArray(quad->loc.vtx_pos);
    glEnableVertexAttribArray(quad->loc.vtx_tex0);
    
    glVertexAttribPointer(quad->loc.vtx_pos, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *)offsetof(vertex_t, pos));
    glVertexAttribPointer(quad->loc.vtx_tex0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *)offsetof(vertex_t, tex));
    
    glUniformMatrix4fv(quad->loc.pvm, 1, GL_FALSE, pvm);
    glUniform1f(quad->loc.alpha, alpha);
    glUniform1i(quad->loc.tex, 0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    CHECK_GL();
    
    glDisableVertexAttribArray(quad->loc.vtx_pos);
    glDisableVertexAttribArray(quad->loc.vtx_tex0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    CHECK_GL();
}

