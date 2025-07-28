#version 460

layout(location=0)      uniform sampler2D   tex;
layout(location=1)      uniform float       alpha;

layout(location = 0)    in vec2             tex_coord;
layout(location = 0)    out vec4            out_color;

void main() {
    vec4 col = texture(tex, tex_coord);
    float brt = (col.r + col.g + col.b) * col.a / 3.f;
    if(brt < 0.05) discard;
    out_color = col;
}
