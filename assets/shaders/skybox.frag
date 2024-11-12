#version 460

out vec4 FragColor;
in vec3 position;

uniform samplerCube u_cubemap;

void main()
{
    FragColor = texture(u_cubemap, position);
}
