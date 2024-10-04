#version 460 core

out vec4 FragColor;

uniform vec3 u_camPos;
uniform vec3 u_sunPos;
uniform vec3 u_planetColor;

in vec3 position;
in vec3 normal;

void main()
{
    vec3 N = normalize(normal);
    vec3 L = normalize(u_sunPos - position);
    vec3 V = normalize(u_camPos - position);
    vec3 R = reflect(-L, N);

    float ambient = 0.25;
    float diffuse = max(0.0, dot(N, L));
    float specular = pow(max(dot(V, R), 0.0), 32.0);
    //float attenuation = smoothstep(u_light.radius, 0.0, length(u_light.position - position));
    
    vec3 lighting = vec3(0.0);
    lighting += ambient * u_planetColor;
    lighting += diffuse * u_planetColor;
    //lighting += specular * u_planetColor;
    //lighting *= attenuation;

    //FragColor = vec4(normal, 1.0);

    FragColor = vec4(lighting, 1.0);
}
