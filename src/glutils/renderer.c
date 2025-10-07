//===--------------------------------------------------------------------------------------------===
// display_render.c - OpenGL 2D rendering functions for display.c
//
// Created by Amy Parent <amy@amyparent.com>
// Copyright (c) 2021 Amy Parent
// Licensed under the MIT License
// =^•.•^=
//===--------------------------------------------------------------------------------------------===
#include "glutils_impl.h"
#include <XPLMGraphics.h>
#include <helpers/helpers.h>
#include <stddef.h>

static const char *vert_shader =
    "#version 120\n"
    "uniform mat4       pv;\n"
    "uniform mat4       model;\n"
    "attribute vec3     vtx_pos;\n"
    "attribute vec2     vtx_tex0;\n"
    "varying vec2       tex_coord;\n"
    "void main() {\n"
    "   tex_coord = vtx_tex0;\n"
    "   gl_Position = pv * model * vec4(vtx_pos, 1.0);\n"
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

static void quad_find_shader_uniforms(gl_quad_t *quad) {
    glUseProgram(quad->shader);
    quad->loc.pv = glGetUniformLocation(quad->shader, "pv");
    quad->loc.model = glGetUniformLocation(quad->shader, "model");
    quad->loc.tex = glGetUniformLocation(quad->shader, "tex");
    quad->loc.alpha = glGetUniformLocation(quad->shader, "alpha");
    
    quad->loc.vtx_pos = glGetAttribLocation(quad->shader, "vtx_pos");
    quad->loc.vtx_tex0 = glGetAttribLocation(quad->shader, "vtx_tex0");
    glUseProgram(0);
}

void quad_init(gl_quad_t *quad, unsigned tex, unsigned shader) {
    quad->last_size[0] = NAN;
    
    quad->tex = tex;
    if(shader) {
        quad->shader = shader;
        quad->own_shader = false;
    } else {
        quad->shader = gl_program_new(vert_shader, frag_shader);
        quad->own_shader = true;
    }
    quad_find_shader_uniforms(quad);
    
    glGenBuffers(1, &quad->vbo);
    glGenBuffers(1, &quad->ibo);
    
    static const GLuint indices[] = {0, 2, 1, 0, 3, 2};
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    XPLMBindTexture2d(0, 0);
}

void quad_set_tex(gl_quad_t *quad, unsigned tex) {
    quad->tex = tex;
}


void quad_set_shader(gl_quad_t *quad, unsigned shader) {
    GLuint old_shader = quad->shader;
    if(quad->own_shader)
        glDeleteProgram(quad->shader);
    quad->shader = shader;
    quad->own_shader = false;
    
    if(old_shader != quad->shader) {
        quad_find_shader_uniforms(quad);
    }
}

void quad_fini(gl_quad_t *quad) {
    if(quad->own_shader)
        glDeleteProgram(quad->shader);
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

static inline bool vec2_eq(vec2 a, vec2 b) {
    return a[0] == b[0] && a[1] == b[1];
}

static void prepare_vertices(gl_quad_t *quad, vec2 size) {
    if(vec2_eq(quad->last_size, size))
        return;
    vertex_t vert[4];
    
    vert[0].pos[0] = 0;
    vert[0].pos[1] = 0;
    vert[0].tex[0] = 0;
    vert[0].tex[1] = 0;

    vert[1].pos[0] = size[0];
    vert[1].pos[1] = 0;
    vert[1].tex[0] = 1;
    vert[1].tex[1] = 0;

    vert[2].pos[0] = size[0];
    vert[2].pos[1] = size[1];
    vert[2].tex[0] = 1;
    vert[2].tex[1] = 1;

    vert[3].pos[0] = 0;
    vert[3].pos[1] = size[1];
    vert[3].tex[0] = 0;
    vert[3].tex[1] = 1;
    
    glBindBuffer(GL_ARRAY_BUFFER, quad->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);
    CHECK_GL();
    
    glm_vec2_copy(size, quad->last_size);
}
void quad_render(mat4 pvm, gl_quad_t *quad, vec2 pos, vec2 size, float rot, float alpha) {
    ASSERT(quad);
    ASSERT(pvm);
    
    XPLMBindTexture2d(quad->tex, 0);
    
    prepare_vertices(quad, size);
    
    glUseProgram(quad->shader);
    glBindBuffer(GL_ARRAY_BUFFER, quad->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad->ibo);
    
    glEnableVertexAttribArray(quad->loc.vtx_pos);
    glEnableVertexAttribArray(quad->loc.vtx_tex0);
    
    glVertexAttribPointer(quad->loc.vtx_pos, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *)offsetof(vertex_t, pos));
    glVertexAttribPointer(quad->loc.vtx_tex0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *)offsetof(vertex_t, tex));
    
    mat4 model;
	glm_mat4_identity(model);
    glm_translate(model, (vec3){pos[0], pos[1], 0});
    glm_rotate_at(model, (vec3){size[0]/2.f, size[1]/2.f, 0}, glm_rad(rot), (vec3){0, 0, 1});
    
    glUniformMatrix4fv(quad->loc.pv, 1, GL_FALSE, (float *)pvm);
    glUniformMatrix4fv(quad->loc.model, 1, GL_FALSE, (float *)model);
    glUniform1f(quad->loc.alpha, alpha);
    glUniform1i(quad->loc.tex, 0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    CHECK_GL();
    
    glDisableVertexAttribArray(quad->loc.vtx_pos);
    glDisableVertexAttribArray(quad->loc.vtx_tex0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    XPLMBindTexture2d(0, 0);
    glUseProgram(0);
    CHECK_GL();
}

