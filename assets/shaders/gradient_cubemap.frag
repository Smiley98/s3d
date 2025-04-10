#version 460 core

out vec4 FragColor;

uniform int u_face;
uniform vec2 u_res;

uniform vec3 u_TL;
uniform vec3 u_TR;
uniform vec3 u_BL;
uniform vec3 u_BR;

vec3 faceToDir(int face, vec2 uv)
{
    vec2 coords = 2.0 * uv - 1.0;
    if (face == 0) return vec3(1.0, -coords.y, -coords.x);  // +X
    if (face == 1) return vec3(-1.0, -coords.y, coords.x);  // -X
    if (face == 2) return vec3(coords.x, 1.0, coords.y);    // +Y
    if (face == 3) return vec3(coords.x, -1.0, -coords.y);  // -Y
    if (face == 4) return vec3(coords.x, -coords.y, 1.0);   // +Z
    return vec3(-coords.x, -coords.y, -1.0);                // -Z (face == 5)
}

vec3 getGradientColor(vec3 dir)
{
    dir = normalize(dir);
    vec3 dirNorm = dir * 0.5 + 0.5;
    
    // Stronger influence towards corners
    float dy = smoothstep(0.0, 1.0, dirNorm.y);
    float dx = smoothstep(0.0, 1.0, dirNorm.x);

    vec3 top = mix(u_TL, u_TR, dx);
    vec3 bot = mix(u_BL, u_BR, dx);
    return mix(top, bot, dy);
}

void main()
{
    vec2 uv = gl_FragCoord.xy / u_res;
    vec3 dir = faceToDir(u_face, uv);
    vec3 rgb = getGradientColor(dir);
    FragColor = vec4(rgb, 1.0);
}
