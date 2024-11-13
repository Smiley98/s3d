#version 460 core

out vec4 FragColor;
uniform vec3 u_color;

uniform float u_near;
uniform float u_far;

float linearize(float depth)
{
    // [0, 1] --> [-1, 1]
    float z = depth * 2.0 - 1.0;

    // [-1, 1] --> [near, far]
    return (2.0 * u_near * u_far) / (u_far + u_near - z * (u_far - u_near));	
}

void main()
{
    // [near, far] --> [0, 1]
    float depth = linearize(gl_FragCoord.z) / u_far;
    FragColor = vec4(vec3(depth), 1.0);
}
