#version 460

out vec4 FragColor;
in vec2 uv;

uniform float u_time;
uniform vec2 u_resolution;

uniform mat3 u_camRot;
uniform vec3 u_camPos;
uniform float u_fov;

uniform mat4 u_test;

uniform mat4 u_planetMatrices[9];
uniform vec3 u_planetColors[9];
uniform float u_planetRadii[9];

struct object
{
  float dist;   // distance
  int idx;      // material index
};

mat2 rotate(float angle)
{
  float s = sin(angle);
  float c = cos(angle);
  return mat2(c, -s, s, c);
}

float opU(float a, float b)
{
    return min(a, b);
}

float sdBox(vec3 p, vec3 b)
{
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float sdSphere(vec3 p, float r)
{
  return length(p) - r;
}

object map(vec3 p)
{ 
  object nearest;
  nearest.dist = 100.0;
  nearest.idx = -1;

  for (int i = 0; i < 9; i++)
  {
    mat4 mat = u_planetMatrices[i];
    vec3 col = u_planetColors[i];
    float rad = u_planetRadii[i];

    object planet;
    planet.dist = sdSphere((mat * vec4(p, 1.0)).xyz, rad);
    planet.idx = i;
    nearest = planet.dist < nearest.dist ? planet : nearest;
  }

  return nearest;
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
  
  int hit = -1;
  for (int i = 0; i < 64; i++)
  {
    // Position along ray
    vec3 p = ro + rd * t;
    
    // Length of ray
    object obj = map(p);
    float d = obj.dist;
    t += d;
    
    // Ray is near an object, register a hit!
    if (d < 0.01)
    {
      hit = obj.idx;
      break;
    }
    
    // Ray is too far from anything, abort mission!
    if (t > 100.0)
      break;
  }

  if (hit < 0)
    discard;

  vec3 color = u_planetColors[hit];
  FragColor = vec4(color, 1.0);
}
