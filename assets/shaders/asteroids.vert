#version 460 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTcoord;
layout (location = 2) in mat4 aWorld;
//layout (location = 2) in vec3 aTranslation;

out vec2 uv;

uniform mat4 u_mvp;

void main()
{
   uv = aTcoord;
   gl_Position = u_mvp * aWorld * vec4(aPosition, 1.0);
}
