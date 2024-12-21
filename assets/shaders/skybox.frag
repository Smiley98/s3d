#version 460

out vec4 FragColor;
in vec3 position;

uniform samplerCube u_cubemap;

void main()
{
    vec3 col = texture(u_cubemap, position).rgb;
    FragColor = vec4(col, 1.0);
}
