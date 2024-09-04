/*===--------------------------------------------------------------------------------------------===
 * gl.c
 *
 * Created by Amy Parent <amy@amyparent.com>
 * Copyright (c) 2024 Laminar Research. All rights reserved
 *
 * Licensed under the MIT License
 *===--------------------------------------------------------------------------------------------===
*/
#include <glutils/gl.h>
#define STB_IMAGE_IMPLEMENTATION
#include <glutils/stb_image.h>
#include <helpers/helpers.h>
// #include <acfutils/log.h>
// #include <acfutils/assert.h>
// #include <acfutils/helpers.h>
#include <stdlib.h>

static bool check_shader(GLuint sh) {
    GLint is_compiled = 0;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &is_compiled);
    if(is_compiled == GL_TRUE)
        return true;

    GLint length = 0;
    glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &length);

    char log[length + 1];
    glGetShaderInfoLog(sh, length, &length, log);
    log[length] = '\0';
    log_msg("shader compile error: %s", log);
    return false;
}

static bool check_program(GLuint sh) {
    GLint is_compiled = 0;
    glGetProgramiv(sh, GL_LINK_STATUS, &is_compiled);
    if(is_compiled == GL_TRUE)
        return true;

    GLint length = 0;
    glGetProgramiv(sh, GL_INFO_LOG_LENGTH, &length);

    char log[length + 1];
    glGetProgramInfoLog(sh, length, &length, log);
    log[length] = '\0';
    log_msg("shader link error: %s", log);
    return false;
}

GLuint gl_create_program(const char *vertex, const char *fragment) {
    ASSERT(vertex);
    ASSERT(fragment);

    GLuint vert = gl_load_shader(vertex, GL_VERTEX_SHADER);
    if(!vert)
        return 0;

    GLuint frag = gl_load_shader(fragment, GL_FRAGMENT_SHADER);
    if(!frag)
        return 0;

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);

    glDeleteShader(vert);
    glDeleteShader(frag);

    if(!check_program(prog)) {
        glDeleteProgram(prog);
        return 0;
    }
    return prog;
}

GLuint gl_load_shader(const char *source, int type) {
    ASSERT(source);
    ASSERT(type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER);
    GLint size = strlen(source);
    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &source, &size);
    glCompileShader(sh);
    if(!check_shader(sh)) {
        glDeleteShader(sh);
        return 0;
    }
    return sh;
}

GLuint gl_create_tex(unsigned width, unsigned height) {
    ASSERT(width > 0);
    ASSERT(height > 0);
    
    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    return tex;
}

GLuint gl_load_tex(const char *path, int *w, int *h) {
    // stbi_set_flip_vertically_on_load(true);
    int components = 0;
    uint8_t *data = stbi_load(path, w, h, &components, 4);
    if(!data) {
        log_msg("unable to load image `%s`", path);
        return 0;
    }
    if(components != 4) {
        log_msg("image `%s` does not have the right format", path);
        free(data);
        return 0;
    }

    GLuint tex = gl_create_tex(*w, *h);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *w, *h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    free(data);

    log_msg("loaded texture `%s` (%dx%d px)", path, *w, *h);
    return tex;
}

void gl_ortho(float proj[16], float x, float y, float width, float height) {
    ASSERT(proj);
    // float x_max = (x+width) -1;
    // float y_max = (y+height) -1;
    float z_near = 1.0;
    float z_far = -1.0;
    
    float left = x;
    float right = x + width;
    float top = y;
    float bottom = y + height;

    proj[0] = 2.f / (right-left);
    proj[1] = 0.f;
    proj[2] = 0.f;
    proj[3] = - (right + left) / (right - left);

    proj[4] = 0.f;
    proj[5] = 2.f / (top-bottom);
    proj[6] = 0.f;
    proj[7] = - (top + bottom) / (top - bottom);

    proj[8] = 0.f;
    proj[9] = 0.f;
    proj[10] = -2.f / (z_far-z_near);
    proj[11] = (z_near+z_far)/(z_near-z_far);

    proj[12] = 0.f;
    proj[13] = 0.f;
    proj[14] = 0.f;
    proj[15] = 1.f;
}

void check_gl(const char *where, int line) {
    GLenum error = glGetError();
    if(error == GL_NO_ERROR) return;
    log_msg("%s() OpenGL error code 0x%04x line %d", where, error, line);
}
