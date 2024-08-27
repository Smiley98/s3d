#version 460 core

out vec4 FragColor;

uniform vec2 u_resolution;

void main()
{
    vec2 uv = gl_FragCoord.xy / u_resolution;
    FragColor = vec4(uv.x, uv.y, gl_FragCoord.z, 1.0);
}
