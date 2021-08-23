#version 410

in vec4 position;
in vec4 color;
in vec4 normal;

out vec4 f_normal;
out vec3 f_position;

uniform mat4 modelMatrix;
uniform mat4 modelViewProjectionMatrix;

void main()
{
    f_position = normalize(modelMatrix * position).xyz;
    f_normal = normal;
    
    gl_Position = modelViewProjectionMatrix * position;
}