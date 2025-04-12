#version 460 core

out vec4 FragColor;

uniform samplerCube u_paintColor;
uniform samplerCube u_environmentColor;

uniform sampler2D u_paintMask;
uniform sampler2D u_specularMask;

uniform float u_paintIntensity;
uniform float u_environmentIntensity;

uniform float u_ambient;
uniform float u_diffuse;
uniform float u_specular;

in vec3 position;   // world-space
in vec3 normal;     // world-space
in vec2 uv;

uniform vec3 u_camPos;
uniform vec3 u_lightDirection;

void main()
{
    vec3 N = normalize(normal);
    vec3 L = -u_lightDirection;
    vec3 V = normalize(u_camPos - position);
    vec3 R = reflect(-L, N);
    vec3 E = reflect(-V, N);

    float diffuse = max(0.0, dot(N, L));
    float specular = max(dot(V, R), 0.0);

    vec3 paint = texture(u_paintColor, E).rgb * texture(u_paintMask, uv).rgb * u_paintIntensity;
    vec3 env = texture(u_environmentColor, E).rgb * texture(u_specularMask, uv).r * u_environmentIntensity;
    vec3 color = env + paint;
    
    vec3 lighting = vec3(0.0);
    lighting += color * u_ambient;
    lighting += color * diffuse * u_diffuse;
    lighting += color * pow(specular, 128.0) * u_specular;
    
    FragColor = vec4(lighting, 1.0);
}

// Switch from point light to direction light
//uniform vec3 u_lightPosition;
//uniform float u_lightRadius;
//vec3 L = normalize(lightPosition - position);
//float attenuation = smoothstep(u_lightRadius, 0.0, length(u_lightPosition - position));
//lighting *= attenuation;
