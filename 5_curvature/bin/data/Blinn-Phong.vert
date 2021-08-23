#version 410

in vec4 v_pos;
in vec4 v_color;
in vec4 v_normal;

out vec3 f_pos;
out vec4 f_color;
out vec4 f_normal;

uniform mat4 modelMatrix;
uniform mat4 modelViewProjectionMatrix;

void main()
{
    gl_Position = modelViewProjectionMatrix * v_pos;

    vec4 pos = modelMatrix * v_pos;
    f_pos = pos.xyz / pos.w;
    
    f_color = v_color;
    f_normal = v_normal;
}