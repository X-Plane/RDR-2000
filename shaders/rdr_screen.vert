#version 460

layout(location=0)  uniform mat4    pv;
layout(location=1)  uniform mat4    model;
layout(location=0)  in vec3         vtx_pos;
layout(location=1)  in vec2         vtx_tex0;
layout(location=0)  out vec2        tex_coord;

void main()
{
    tex_coord = vtx_tex0;
    gl_Position = pv * model * vec4(vtx_pos, 1.0);
}
