#version 460 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTcoord;
layout (location = 2) in mat4 aWorld;

out vec2 uv;

uniform mat4 u_orbit;
uniform mat4 u_mvp;

void main()
{
   uv = aTcoord;
   gl_Position = u_mvp * u_orbit * aWorld * vec4(aPosition, 1.0);
}
