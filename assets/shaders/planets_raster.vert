#version 460 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTcoord;

uniform mat4 u_mvp;
uniform mat4 u_world;
uniform mat3 u_normal;

uniform vec3 u_camPos;
uniform vec3 u_sunPos;
uniform vec3 u_planetColor;
uniform int u_planetIndex;

out vec3 color;

void main()
{
   vec3 P = (u_world * vec4(aPosition, 1.0)).xyz;
   vec3 N = normalize(u_normal * aNormal);
   vec3 L = normalize(u_sunPos - P);

   float diffuse = max(dot(N, L), 0.0);
   color = u_planetIndex > 0 ? diffuse * u_planetColor : u_planetColor;

   gl_Position = u_mvp * vec4(aPosition, 1.0);
}
