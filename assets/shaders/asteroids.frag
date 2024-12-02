#version 460 core

out vec4 FragColor;

in vec2 uv;
in vec3 normal;
in float lighting;

uniform sampler2D u_tex;

void main()
{
    vec3 col = texture(u_tex, uv).rgb * lighting;
    FragColor = vec4(col, 1.0);
}
