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

vec4 map_color(float W) {
    W = pow(W, 1.8) * 5;
    return colors[clamp(int(W), 0, 4)];
}

vec2 beam_uv(vec2 beam) {
    return vec2(0.5, 0) + beam / aspect;
}

float random2(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233)))* 43758.5453123);
}

float random1(float n){
    return fract(sin(n) * 43758.5453123);
}

float rand_noise(float beam_dist) {
    return beam_dist * 0.05 * random2(tex_coord * 0.01);
}

vec2 rotate_beam(vec2 beam, float angle) {
    float cos_ang = cos(angle);
    float sin_ang = sin(angle);
    mat2 rot_ang = mat2(cos_ang, -sin_ang, sin_ang, cos_ang);
    return rot_ang * beam;
}

#define ATTEN_N 40

float attenuation(vec2 beam) {
    float integ = 0;
    float dist = range * length(beam);
    
    int n = int((dist/120.f) * ATTEN_N);
    
    for(int i = 0; i < n; ++i) {
        float mult = float(i)/float(n);
        integ += texture(tex, beam_uv(beam * mult)).r / float(ATTEN_N);
        if(mult * range > dist)
            break;
    }
    return 2*mix(random2(beam), integ, 1);
}

float sample_radar(vec2 beam, float dist) {
    float s1 = sin(dist/2 * 16.1803);
    float s2 = sin(dist/2 * 95.828);
    float s3 = sin(dist/2 * 181.959);
    float s4 = sin(dist/2 * 314.159);
    float s5 = sin(dist/2 * 547.363);
    
    float smear_s = (5 * s1 * s2 * s3 * s4 * s5);
    
    vec2 uv = beam_uv(rotate_beam(beam, radians(smear_s)));
    return 0.1 * abs(random2(uv)) + texture(tex, uv).r;
}

void main() {
    const vec2 up = vec2(0, 1);
    vec2 beam = (tex_coord - vec2(0.5, 0)) * aspect;
    float beam_dist = length(beam);
    float beam_angle = acos(dot(normalize(beam), up));
    if(beam_angle > ant_lim || beam_dist > 0.99)
        discard;
    beam_angle *= sign(beam.x);
    if(beam_angle < angle_start || beam_angle > angle_end) discard;
    // return;
    
    out_color = map_color(texture(tex, tex_coord).r);
    float r = attenuation(beam);
    float W = mix(sample_radar(beam, beam_dist), rand_noise(beam_dist), r);
    out_color = map_color(W);
}
