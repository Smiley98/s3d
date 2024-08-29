#version 460 core

out vec4 FragColor;

uniform vec2 u_resolution;

void main()
{
    // Note that z is still [0, 1] and *NON-LINEAR*
    vec2 uv = gl_FragCoord.xy / u_resolution;
    uv = uv * 2.0 - 1.0;
    FragColor = vec4(uv.x, uv.y, gl_FragCoord.z, 1.0);
}
