#version 460 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTcoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in mat4 aWorlds;

out vec2 uv;
out vec3 normal;

uniform mat4 u_orbit;
uniform mat4 u_mvp;

void main()
{
   uv = aTcoord;
   normal = aNormal;
   gl_Position = u_mvp * u_orbit * aWorlds * vec4(aPosition, 1.0);
}
