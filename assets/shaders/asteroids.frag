#version 460 core

out vec4 FragColor;

in vec2 uv;

uniform sampler2D u_tex;

void main()
{
    FragColor = vec4(texture(u_tex, uv).rgb, 1.0);
}
