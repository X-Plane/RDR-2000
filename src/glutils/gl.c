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
// #define STB_IMAGE_IMPLEMENTATION
// We don't define IMPLEMENTATION because otherwise we get link-time errors, since NanoVG already
// does that.
#include <glutils/stb_image.h>
#include <helpers/helpers.h>
#include <stdlib.h>


GLuint gl_fbo_new(unsigned width, unsigned height, GLuint *out_tex) {
    ASSERT(width > 0);
    ASSERT(height > 0);
    ASSERT(out_tex != NULL);
    
    GLuint fbo = 0;
    GLuint tex = 0;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        log_msg("framebuffer %u incomplete", fbo);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    *out_tex = tex;
    return fbo;
}

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

GLuint gl_program_new(const char *vertex, const char *fragment) {
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

GLuint gl_tex_new(unsigned width, unsigned height) {
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

    GLuint tex = gl_tex_new(*w, *h);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *w, *h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    free(data);
    return tex;
}

void check_gl(const char *where, int line) {
    GLenum error = glGetError();
    if(error == GL_NO_ERROR) return;
    log_msg("%s() OpenGL error code 0x%04x line %d", where, error, line);
}
