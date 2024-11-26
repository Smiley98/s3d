#version 460 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTcoord;
layout (location = 2) in vec3 aTranslation;

out vec2 uv;

// view-projection, model will be stored in attribute 2
uniform mat4 u_mvp;

void main()
{
   // Translation should change based on instance due to vertex divisor
   vec3 p = aPosition + aTranslation;
   uv = aTcoord;
   gl_Position = u_mvp * vec4(p, 1.0);
}
