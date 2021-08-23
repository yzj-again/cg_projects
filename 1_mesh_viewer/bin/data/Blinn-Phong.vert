#version 410

in vec4 position;
in vec4 color;
in vec4 normal;

out vec4 v_normal;
out vec3 v_position_mv;
out vec4 v_color;

uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 normalMatrix;

void main()
{
    vec4 position_mv = modelViewMatrix * position;
    v_position_mv = position_mv.xyz / position_mv.w;
    
    v_normal = normal;
    
    gl_Position = modelViewProjectionMatrix * position;

    v_color = color;
}