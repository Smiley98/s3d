#version 460

out vec4 FragColor;
in vec2 uv;

uniform float u_time;
uniform vec2 u_resolution;

uniform mat3 u_camRot;
uniform vec3 u_camPos;
uniform float u_fov;

float sdSphere(vec3 p, vec3 t, float r)
{
  return length(p - t) - r;
}

float map(vec3 p)
{
  float sphere = sdSphere(p, vec3(3.0, 0.0, -1.0), 1.0);
  return sphere;
}

void main()
{
  // [0, 1] --> [-1, 1]
  vec2 uv = (gl_FragCoord.xy * 2.0 - u_resolution) / u_resolution.y;

  // Ray origin
  vec3 ro = u_camPos;
  
  // Ray direction
  vec3 rd = u_camRot * normalize(vec3(uv * u_fov, -1.0));
  
  // Distance along ray
  float t = 0.0;
  
  int i;
  for (i = 0; i < 80; i++)
  {
    // Position in scene
    vec3 p = ro + rd * t;
    
    float d = map(p);
    t += d;
    
    if (t < 0.001) break; // Near plane
    if (t > 100.0) break; // Far plane
  }
  
  // Colour based on ray distance
  vec3 color = vec3(1.0, 1.0, 1.0) * t * 0.1; // range from camera (-5) to sphere (5) = 10 so / by 10 to normalize
  FragColor = vec4(color, 1.0);
}
