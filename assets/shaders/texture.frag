#version 460

out vec4 FragColor;
in vec2 uv;

uniform sampler2D u_tex;

void main()
{
    vec3 col = texture(u_tex, uv).rgb;
    FragColor = vec4(col, 1.0);
}
