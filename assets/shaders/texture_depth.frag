#version 460

out vec4 FragColor;
in vec2 uv;

uniform sampler2D u_tex;
uniform float u_near;
uniform float u_far;

// Unproject non-linear depth to camera-space
float decode(float depth)
{
    // [0, 1] --> [-1, 1]
    float z = depth * 2.0 - 1.0;
    float linearDepth = (2.0 * u_near * u_far) / (u_far + u_near - z * (u_far - u_near));
    return linearDepth;
}

void main()
{
    // Non-linear [0, 1]
    float depth = texture(u_tex, uv).r;

    // Linear [0, 1]
    depth = decode(depth);

    // Greyscale
    FragColor = vec4(vec3(depth), 1.0);
}
