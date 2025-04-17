#version 460 core

layout (location = 3) out vec3 gLighting;

uniform sampler2D u_normals;
uniform sampler2D u_albedo;

uniform vec3 u_lightDirection;
uniform vec3 u_lightColor;

uniform float u_ambient;
uniform float u_diffuse;

in vec2 uv;
out vec4 FragColor;

void main()
{
    vec3 normal = texture(u_normals, uv).xyz;
    vec3 albedo = texture(u_albedo, uv).rgb;
    
    float dotNL = max(dot(normalize(normal), -u_lightDirection), 0.0);

    vec3 lighting = vec3(0.0);
    lighting += albedo * u_lightColor * u_ambient;
    lighting += albedo * u_lightColor * u_diffuse * dotNL;
    gLighting = lighting;
}
