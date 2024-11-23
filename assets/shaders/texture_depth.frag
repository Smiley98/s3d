#version 460

out vec4 FragColor;
in vec2 uv;

uniform sampler2D u_tex;
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
    // Read depth buffer from fbo's depth-texture attachment
    float depth = texture(u_tex, uv).r;

    // [near, far] --> [0, 1]
    depth = linearize(depth) / u_far;

    // Output greyscale depth to FSQ
    FragColor = vec4(vec3(depth), 1.0);
}
