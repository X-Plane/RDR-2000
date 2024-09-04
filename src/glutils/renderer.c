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
    "#version 400\n"
    "uniform mat4   pvm;\n"
    "in vec2        vtx_pos;\n"
    "in vec2        vtx_tex0;\n"
    "out vec2       tex_coord;\n"
    "void main() {\n"
    "    tex_coord = vtx_tex0;\n"
    "    gl_Position = pvm * vec4(vtx_pos, 0.0, 1.0);\n"
    // "    gl_Position = vec4(vtx_pos, 0.0, 1.0);\n"
    "}\n";

static const char *frag_shader =
    "#version 400\n"
    "uniform sampler2D	tex;\n"
    "uniform float	    alpha;\n"
    "in vec2	        tex_coord;\n"
    "out vec4	        color_out;\n"
    "void main() {\n"
    "    vec4 color = texture(tex, tex_coord);\n"
    "    color.a *= alpha;\n"
    "    color_out = color;\n"
    "}\n";

static bool is_init = false;
// static GLuint default_quad_shader = 0;

void render_init() {
    if(is_init) return;
    is_init = true;
}

void render_fini() {
    ASSERT(is_init);
    is_init = false;
}

target_t *target_new(double x, double y, double width, double height) {
    ASSERT(width > 0);
    ASSERT(height > 0);
    target_t *target = safe_calloc(1, sizeof(*target));
    target_set_size(target, width, height);
    target->size = VECT2(width, height);
    target->offset = VECT2(x, y);
    gl_ortho(target->proj, target->offset.x, target->offset.y, target->size.x, target->size.y);
    return target;
}

void target_destroy(target_t *target) {
    ASSERT(target);
    free(target);
}

void target_set_offset(target_t *target, double x, double y) {
    ASSERT(target);
    target->offset = VECT2(x, y);
    gl_ortho(target->proj, x, y, target->size.x, target->size.y);
    
}

void target_set_size(target_t *target, double width, double height) {
    ASSERT(target);
    ASSERT(width > 0);
    ASSERT(height > 0);
    target->size = VECT2(width, height);
    gl_ortho(target->proj, target->offset.x, target->offset.y, width, height);
}

static inline void enable_attrib(GLint index, GLint size, GLenum type,
    GLboolean normalized, size_t stride, size_t offset)
{
	if (index != -1) {
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, size, type, normalized,
		    stride, (void *)offset);
	}
}

void quad_init(gl_quad_t *quad, unsigned tex, unsigned shader) {
    quad->last_pos = NULL_VECT2;
    quad->last_size = NULL_VECT2;
    
    quad->tex = tex;
    if(shader) {
        quad->shader = shader;
    } else {
        quad->shader = gl_create_program(vert_shader, frag_shader);
    }
    
    glGenVertexArrays(1, &quad->vao);
    glBindVertexArray(quad->vao);
    
    glGenBuffers(1, &quad->vbo);
    glGenBuffers(1, &quad->ibo);
    
    static const GLuint indices[] = {0, 1, 2, 0, 2, 3};
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glUseProgram(quad->shader);
    quad->loc.pvm = glGetUniformLocation(quad->shader, "pvm");
    quad->loc.tex = glGetUniformLocation(quad->shader, "tex");
    quad->loc.alpha = glGetUniformLocation(quad->shader, "alpha");
    
    quad->loc.vtx_pos = glGetAttribLocation(quad->shader, "vtx_pos");
    quad->loc.vtx_tex0 = glGetAttribLocation(quad->shader, "vtx_tex0");
    
    glBindBuffer(GL_ARRAY_BUFFER, quad->vbo);
    enable_attrib(quad->loc.vtx_pos, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), offsetof(vertex_t, pos));
    enable_attrib(quad->loc.vtx_tex0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), offsetof(vertex_t, tex));
    
    glBindVertexArray(0);
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
    glDeleteVertexArrays(1, &quad->vao);
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
    vert[0].tex = (vec2f_t){0, 0};

    vert[1].pos.x = pos.x + size.x;
    vert[1].pos.y = pos.y;
    vert[1].tex = (vec2f_t){1, 0};

    vert[2].pos.x = pos.x + size.x;
    vert[2].pos.y = pos.y + size.y;
    vert[2].tex = (vec2f_t){1, 1};

    vert[3].pos.x = pos.x;
    vert[3].pos.y = pos.y + size.y;
    vert[3].tex = (vec2f_t){0, 1};
    
    glBindBuffer(GL_ARRAY_BUFFER, quad->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);
    CHECK_GL();
    
    quad->last_pos = pos;
    quad->last_size = size;
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void render_quad(target_t *target, gl_quad_t *quad, vect2_t pos, vect2_t size, double alpha) {
    ASSERT(is_init);
    ASSERT(quad);
    ASSERT(target);
    
#if APL
    glDisableClientState(GL_VERTEX_ARRAY);
#endif
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, quad->tex);
    // glBindBuffer(GL_ARRAY_BUFFER, quad->vbo);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad->ibo);
    
    prepare_vertices(quad, pos, size);
    
    glUseProgram(quad->shader);
    glBindVertexArray(quad->vao);
    
    glUniformMatrix4fv(quad->loc.pvm, 1, GL_TRUE, target->proj);
    glUniform1f(quad->loc.alpha, alpha);
    glUniform1i(quad->loc.tex, 0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    CHECK_GL();

    // glDisableVertexAttribArray(quad->loc.vtx_pos);
    // glDisableVertexAttribArray(quad->loc.vtx_tex0);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    CHECK_GL();
}

