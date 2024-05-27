#version 460 core
//in vec3 normal;
out vec4 FragColor;
uniform vec3 u_color;

void main()
{
    //FragColor = vec4(normal * 0.5 + 0.5, 1.0);
    FragColor = vec4(u_color, 1.0);
}
