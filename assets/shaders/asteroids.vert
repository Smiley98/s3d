#version 460 core

layout (location = 0) in vec3 aPosition;  // per-draw
layout (location = 1) in vec2 aTcoord;    // per-draw
layout (location = 2) in vec3 aNormal;    // per-draw
layout (location = 3) in mat4 aWorlds;    // per-instance
layout (location = 7) in mat3 aNormals;   // per-instance

out vec2 uv;
out vec3 normal;
out float lighting;

uniform mat4 u_mvp;
uniform mat4 u_orbit;
uniform vec3 u_sunPos;

void main()
{
   // Normals now optimized as instance data rather than mat3(transpose(inverse(worldMat)));
   // Might be faster computing normal matrix per-vertex since that saves on data
   // Might also be faster computing lighting in fragment shader due to early depth-test 
   mat4 worldMat = u_orbit * aWorlds;
   mat3 normalMat = mat3(u_orbit) * aNormals;
   uv = aTcoord;
   normal = normalMat * aNormal;

   vec3 P = (worldMat * vec4(aPosition, 1.0)).xyz;
   vec3 N = normalize(normal);
   vec3 L = normalize(u_sunPos - P);
   float diffuse = max(dot(N, L), 0.0) * 0.6;
   lighting = 0.1 + diffuse;

   gl_Position = u_mvp * worldMat * vec4(aPosition, 1.0);
}
