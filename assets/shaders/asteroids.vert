#version 460 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTcoord;

const int PLANET_COUNT = 9;

uniform mat4 u_mvp[PLANET_COUNT];
uniform mat4 u_world[PLANET_COUNT];
uniform mat3 u_normal[PLANET_COUNT];
uniform vec3 u_color[PLANET_COUNT];
uniform vec3 u_sunPos;

out vec3 color;

void main()
{
   int i = gl_InstanceID;
   mat4 mvp = u_mvp[i];
   mat4 world = u_world[i];
   mat3 normal = u_normal[i];
   vec3 col = u_color[i];

   vec3 P = (world * vec4(aPosition, 1.0)).xyz;
   vec3 N = normalize(normal * aNormal);
   vec3 L = normalize(u_sunPos - P);

   float diffuse = max(dot(N, L), 0.0);
   color = i > 0 ? diffuse * col : col;

   gl_Position = mvp * vec4(aPosition, 1.0);
}
