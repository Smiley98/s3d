#version 460 core

layout (location = 3) out vec3 gLighting;

uniform sampler2D u_positions;
uniform sampler2D u_normals;
uniform sampler2D u_albedo;

uniform vec3 u_lightPosition;
uniform vec3 u_lightColor;
uniform float u_lightRadius;

uniform float u_ambient;
uniform float u_diffuse;
uniform float u_specular;

uniform vec3 u_cameraPosition;
uniform float u_isViewSpace;
uniform vec2 u_screen;

out vec4 FragColor;

void main()
{
    vec2 uv = (gl_FragCoord.xy) / u_screen;

    vec3 position = texture(u_positions, uv).xyz;
    vec3 albedo = texture(u_albedo, uv).rgb;
    
    // Camera position is (0.0, 0.0, 0.0) in view-space
    vec3 cameraPosition = u_cameraPosition * (1.0 - u_isViewSpace);
    vec3 N = texture(u_normals, uv).xyz;
    vec3 L = normalize(u_lightPosition - position);
    vec3 V = normalize(cameraPosition - position);
    vec3 R = reflect(-L, N);

    float dotNL = max(dot(N, L), 0.0);
    float dotVR = max(dot(V, R), 0.0);
    float attenuation = smoothstep(u_lightRadius, 0.0, length(u_lightPosition - position));
    
    vec3 lighting = vec3(0.0);
    lighting += u_lightColor * albedo * u_ambient;
    lighting += u_lightColor * albedo * u_diffuse * dotNL;
    lighting += u_lightColor * albedo * u_specular * pow(dotVR, 32);
    lighting *= attenuation;
    gLighting = lighting;
}
