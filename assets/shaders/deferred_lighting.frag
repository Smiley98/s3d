#version 460 core

uniform sampler2D u_positions;
uniform sampler2D u_normals;
uniform sampler2D u_albedo;

uniform vec2 u_resolution;
uniform vec3 u_lightPosition;
uniform vec3 u_lightColor;

//uniform float u_lightRadius;
// Attenuation serves two purposes:
// 1. Contribution.
// 2. Falloff.
// Deferred lighting ensures lights don't contribute to fragments outside their radius.
// However, falloff still needs a formula... Simply don't have falloff??
// P0 will most likely be made with a forward pipeline so transparency and multi-sampling are easier!

out vec4 FragColor;

void main()
{
    vec2 uv = gl_FragCoord.xy / u_resolution;

    vec3 position = texture(u_positions, uv).xyz;
    vec3 albedo = texture(u_albedo, uv).rgb;

    vec3 N = texture(u_normals, uv).xyz;
    vec3 L = normalize(u_lightPosition - position);

    float dotNL = max(dot(N, L), 0.0);
    float ambient = 0.1;
    float diffuse = 1.0;

    vec3 color = vec3(0.0);
    color += u_lightColor * albedo * ambient;
    color += u_lightColor * albedo * diffuse * dotNL;
    FragColor = vec4(color, 1.0);
}
