#version 460 core

out vec4 FragColor;

in vec2 uv;
in vec3 normal;

uniform sampler2D u_tex;

void main()
{
    //vec3 col = texture(u_tex, uv).rgb;
    vec3 col = normalize(normal);
    FragColor = vec4(col, 1.0);
}
