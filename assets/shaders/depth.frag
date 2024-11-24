#version 460 core

out vec4 FragColor;

uniform float u_near;
uniform float u_far;

// Unproject non-linear depth to camera-space
float linearize(float depth)
{
    // [0, 1] --> [-1, 1]
    float z = depth * 2.0 - 1.0;
    
    // [-1, 1] --> [camera, fragment]
    float linearDepth = (2.0 * u_near * u_far) / (u_far + u_near - z * (u_far - u_near));

    return linearDepth;
}

// Normalize linear camera-space depth
float normalizeLinearDepth(float linearDepth)
{
    // [camera, fragment] --> [0, 1]
    return (linearDepth - u_near) / (u_far - u_near);
}

void main()
{
    float linearDepth = linearize(gl_FragCoord.z);
    float depth = normalizeLinearDepth(linearDepth);
    FragColor = vec4(vec3(depth), 1.0);
}
