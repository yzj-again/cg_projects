#version 410

in vec4 f_normal;  // normal vector
in vec3 f_position;  // position of vertex in the world coordinate

out vec4 FRAG_COLOR;

// these are passed in from OF programmable renderer
uniform mat4 modelViewProjectionMatrix;
uniform mat4 normalMatrix;  // normalMatrix in the world coordinate
uniform vec3 eyePos;  // normalMatrix in the world coordinate

uniform samplerCube envMap;

void main ()
{
    
    vec3 I = normalize(f_position - eyePos);
    vec3 N = normalize((normalMatrix * f_normal).xyz);

    float eta = 1.2f;
    float R0 = pow((1 - eta) / (1 + eta), 2);
    float cos_theta = dot(-I, N) / (length(I) * length(N));
    float R_theta = R0 + (1-R0) * pow(1 - cos_theta, 2);


    vec3 reflect_vec3 = normalize(reflect(I, N));
    vec4 reflect_rgba = texture(envMap, reflect_vec3);
    vec3 refract_vec3 = normalize(refract(I, N, eta));
    vec4 refract_rgba = texture(envMap, refract_vec3);

    // vec4 local_color = reflect_rgba;
    // vec4 local_color = refract_rgba;
    // vec4 local_color = 0.5*reflect_rgba + 0.5*refract_rgba;
    vec4 local_color = R_theta*reflect_rgba + (1-R_theta)*refract_rgba;
    
    // vec4 local_color = vec4(N, 1.0);
    // vec4 local_color = vec4(normalize(eyePos), 1.0);
    // vec4 local_color = vec4(R_theta, R_theta, R_theta, 1.0);

    FRAG_COLOR = clamp(local_color, 0.0, 1.0);
}