#version 460 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gColor;

in vec3 position;   // world-space
in vec3 normal;     // world-space
in vec2 uv;

uniform sampler2D u_tex;

void main()
{
    gPosition = position;
    gNormal = normalize(normal);
    gColor = texture(u_tex, uv).rgb;
}
