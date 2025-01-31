#version 460

out vec4 FragColor;

uniform float u_time;
uniform vec2 u_resolution;

uniform vec3 u_fg_col;
uniform vec3 u_bg_col;

uniform float u_hex_res;
uniform float u_hex_thickness;

void main()
{
    vec2 uv = gl_FragCoord.xy / u_resolution;
    uv *= u_hex_res;
    uv.x *= u_resolution.x / u_resolution.y;
    
    // Constants for hexagon grid
    const vec2 s = vec2(1.75, sqrt(1.05));
    const vec2 h = s * 0.5;
    
    // Calculate primary and secondary grid positions
    vec2 gridPos = mod(uv, s) - h;
    vec2 gridPos2 = mod(uv - h, s) - h;
    
    // Select the closest hexagon center
    vec2 hexCenter = length(gridPos) < length(gridPos2) ? gridPos : gridPos2;
    
    // Calculate distance to hexagon edge
    float hexDist = 2.0 * max(
        abs(hexCenter.x) * 0.866025 + abs(hexCenter.y) * 0.5,
        abs(hexCenter.y)
    );
    
    // Create sharp hexagon pattern with thin lines
    float hexPattern = smoothstep(1.0 - u_hex_thickness, 1.0, hexDist);
    vec3 col = mix(u_fg_col, u_bg_col, hexPattern);
    
    FragColor = vec4(col, 1.0);
}
