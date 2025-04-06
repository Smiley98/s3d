#version 460 core

out vec4 FragColor;

uniform samplerCube u_chameleon_map;
uniform samplerCube u_environment_map;
uniform vec3 u_camPos;

uniform sampler2D u_diffuse;
uniform sampler2D u_specular;

in vec3 position;   // world-space
in vec3 normal;     // world-space
in vec2 uv;

uniform vec3 u_lightPosition;
uniform float u_lightRadius;

void main()
{
    vec3 lightPosition = u_lightPosition;
    float lightRadius = u_lightRadius;

    vec3 N = normalize(normal);
    vec3 L = normalize(lightPosition - position);
    vec3 V = normalize(u_camPos - position);
    vec3 R = reflect(-L, N);
    vec3 I = reflect(-V, N);

    float ambient = 0.25;
    float diffuse = max(0.0, dot(N, L));
    float specular = pow(max(dot(V, R), 0.0), 64.0);
    float attenuation = smoothstep(lightRadius, 0.0, length(lightPosition - position));
    
    // Neither the diffuse nor specular maps apply a correct map for chameleon paint. Doing so would require an artist.
    vec3 objDiff = texture(u_diffuse, uv).rgb;
    vec3 objSpec = texture(u_specular, uv).rgb;

    vec3 objColor = vec3(0.0);
    objColor += texture(u_chameleon_map, I).rgb;
    objColor += texture(u_environment_map, I).rgb;

    vec3 lighting = vec3(0.0);
    lighting += objColor * objDiff * ambient;
    lighting += objColor * objDiff * diffuse;
    lighting += objColor * objSpec * specular;
    lighting *= attenuation;
    FragColor = vec4(lighting, 1.0);
}
