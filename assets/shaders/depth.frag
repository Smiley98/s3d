#version 460 core

out vec4 FragColor;

uniform float u_near;
uniform float u_far;

// Input: depth in non-linear space between [0, 1].
// Output: depth in linear space between [near, far].
float decode(float depth, float near, float far)
{
	depth = depth * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near - depth * (far - near));
}

// Input: depth in linear space between [near, far].
// Output: depth in linear space between [0, 1].
float normalizeDepth(float depth, float near, float far)
{
	return (depth - near) / (far - near);
}

void main()
{
    // Non-linear [0, 1]
    float depth = gl_FragCoord.z;

    // Linear [near, far]
    depth = decode(depth, u_near, u_far);

    // Linear [0, 1]
    depth = normalizeDepth(depth, u_near, u_far);

    // Greyscale
    FragColor = vec4(vec3(depth), 1.0);
}
