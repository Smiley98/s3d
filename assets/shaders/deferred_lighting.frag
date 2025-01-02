#version 460 core

uniform sampler2D u_positions;
uniform sampler2D u_normals;
uniform sampler2D u_albedo;

uniform vec3 u_lightPosition;
uniform vec3 u_lightColor;
uniform float u_ambient;
uniform float u_diffuse;

uniform vec2 u_viewportSize;
uniform vec2 u_viewportOffset;

out vec4 FragColor;

void main()
{
    vec2 uv = (gl_FragCoord.xy - u_viewportOffset) / u_viewportSize;

    vec3 position = texture(u_positions, uv).xyz;
    vec3 albedo = texture(u_albedo, uv).rgb;
    
    vec3 N = texture(u_normals, uv).xyz;
    vec3 L = normalize(u_lightPosition - position);
    float dotNL = max(dot(N, L), 0.0);
    
    vec3 color = vec3(0.0);
    color += u_lightColor * albedo * u_ambient;
    color += u_lightColor * albedo * u_diffuse * dotNL;
    FragColor = vec4(color, 1.0);
}
