#version 460 core

uniform sampler2D u_positions;
uniform sampler2D u_normals;
uniform sampler2D u_albedo;

in vec2 uv;
out vec4 FragColor;

void main()
{
    vec3 position = texture(u_positions, uv).xyz;
    vec3 normal = texture(u_normals, uv).xyz;
    vec3 albedo = texture(u_albedo, uv).rgb;

    float dotNL = max(dot(normalize(normal), normalize(vec3(0.0, 0.0, 1.0))), 0.0);
    float ambient = 0.1;
    float diffuse = 1.0;

    vec3 color = vec3(0.0);
    color += albedo * ambient;
    color += albedo * diffuse * dotNL;
    FragColor = vec4(color, 1.0);
}
