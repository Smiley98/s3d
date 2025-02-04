#version 460 core

uniform sampler2D u_positions;
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
    vec3 position = texture(u_positions, uv).xyz;
    vec3 normal = texture(u_normals, uv).xyz;
    vec3 albedo = texture(u_albedo, uv).rgb;
    
    float dotNL = max(dot(normalize(normal), -u_lightDirection), 0.0);

    vec3 lighting = vec3(0.0);
    lighting += albedo * u_lightColor * u_ambient;
    lighting += albedo * u_lightColor * u_diffuse * dotNL;
    FragColor = vec4(lighting, 1.0);
}
