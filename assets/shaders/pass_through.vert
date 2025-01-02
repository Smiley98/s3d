#version 460 core

layout (location = 0) in vec3 aPosition;
//layout (location = 1) in vec3 aNormal; // Bound in vao but not necessary.
//layout (location = 2) in vec2 aTcoord; // Bound in vao but not necessary.

uniform mat4 u_mvp;

void main()
{
   gl_Position = u_mvp * vec4(aPosition, 1.0);
}
