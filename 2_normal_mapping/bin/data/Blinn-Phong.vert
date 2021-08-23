#version 410

layout (location=0) in vec4 position;
layout (location=1) in vec4 tangent;
layout (location=2) in vec4 normal;
layout (location=3) in vec2 uv;


out vec3 v_position_mv;
out vec2 v_uv;
out mat3 TBN;

uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 normalMatrix;


void main()
{
    gl_Position = modelViewProjectionMatrix * position;

    vec4 position_mv = modelViewMatrix * position;
    v_position_mv = position_mv.xyz / position_mv.w;

    vec3 T = normalize((normalMatrix * tangent).xyz);
    vec3 N = normalize((normalMatrix * normal).xyz);
    vec3 B = normalize(cross(N, T));
    TBN = mat3(T, B, N);

    v_uv = uv;
    v_uv.y = 1-v_uv.y;
}