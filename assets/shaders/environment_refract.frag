#version 460

out vec4 FragColor;

in vec3 position;   // world-space
in vec3 normal;     // world-space
in vec2 uv;

uniform samplerCube u_cubemap;
uniform vec3 u_camPos;
uniform float u_ratio;

void main()
{
    vec3 N = normalize(normal);
    vec3 I = normalize(position - u_camPos);    // camera to fragment
    vec3 R = refract(I, N, u_ratio);
    FragColor = texture(u_cubemap, R);
}
