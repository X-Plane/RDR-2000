#version 460

layout(location = 0)    uniform sampler2D   tex;
layout(location = 1)    uniform vec2        aspect;
layout(location = 3)    uniform float       ant_lim;
layout(location = 4)    uniform float       angle_start;
layout(location = 5)    uniform float       angle_end;
layout(location = 6)    uniform float       range;

layout(location = 0)    in vec2             tex_coord;
layout(location = 0)    out vec4            out_color;

#define TRANS       vec4(0.00, 0.00, 0.00, 1)
#define GREEN       vec4(0, 1, 0.2, 1)
#define YELLOW      vec4(1, 1, 0, 1)
#define RED         vec4(1, 0, 0, 1)
#define MAGENTA     vec4(1, 0.5, 1, 1)


vec4 colors[5] = { TRANS, GREEN, YELLOW, RED, MAGENTA };

void main() {
    const vec2 up = vec2(0, 1);
    vec2 beam = (tex_coord - vec2(0.5, 0)) * aspect;
    float beam_dist = length(beam);
    float beam_angle = acos(dot(normalize(beam), up));
    if(beam_angle > ant_lim || beam_dist > 0.62) discard;
    beam_angle *= sign(beam.x);
    if(beam_angle < angle_start || beam_angle > angle_end) discard;
    out_color = colors[clamp(int(beam_dist * 5 / 0.62), 0, 4)];
}
