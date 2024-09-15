#version 460 core

uniform vec3 u_pos0;
uniform vec3 u_pos1;

// Identity world-matrix
uniform mat4 u_mvp;

void main()
{
    vec4 position = vec4(gl_VertexID == 0 ? u_pos0 : u_pos1, 1.0);
    gl_Position = u_mvp * position;
}
