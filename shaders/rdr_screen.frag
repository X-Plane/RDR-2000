#version 460

layout(location=0)      uniform sampler2D   tex;
layout(location=1)      uniform sampler2D   mask;
layout(location=2)      uniform float       alpha;
layout(location=3)      uniform float       scale;
layout(location=4)      uniform float       blink;

layout(location = 0)    in vec2             tex_coord;
layout(location = 0)    out vec4            out_color;


#define MAGENTA     vec3(1, 0.8, 1)
#define GLOW        vec4(0.12, 0.15, 0.2, 1.0)

void main() {
    vec2 uv = vec2((tex_coord.x / scale) - 0.5 * (1.0/scale - 1.0), tex_coord.y/scale);
    vec4 wxr_col = texture(tex, uv);

    float dist_from_magenta = distance(MAGENTA, wxr_col.rgb);
    float t = clamp(step(0.5, dist_from_magenta) + blink, 0, 1);
    
    vec4 col = GLOW + t * wxr_col;
    vec4 mask_brt = texture(mask, tex_coord);
    out_color = col * pow(mask_brt.r, 1.5) * mask_brt.a * alpha;
}