#version 460
const float EPSILON = 0.0001;

out vec4 FragColor;
in vec2 uv;

uniform vec3 u_camPos;
uniform mat3 u_camRot;
uniform float u_fov;
uniform vec2 u_resolution;

uniform vec3 u_sunPos;
uniform mat4 u_planetMatrices[9];
uniform vec3 u_planetColors[9];
uniform float u_planetRadii[9];

struct object
{
  float dist;   // distance
  int idx;      // material index
};

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

vec3 normal(vec3 p)
{
    return normalize(vec3(
        map(vec3(p.x + EPSILON, p.y, p.z)).dist - map(vec3(p.x - EPSILON, p.y, p.z)).dist,
        map(vec3(p.x, p.y + EPSILON, p.z)).dist - map(vec3(p.x, p.y - EPSILON, p.z)).dist,
        map(vec3(p.x, p.y, p.z  + EPSILON)).dist - map(vec3(p.x, p.y, p.z - EPSILON)).dist
    ));
}

vec3 shade(vec3 position, int material)
{
  vec3 color = u_planetColors[material];
  vec3 light = u_sunPos;

  vec3 N = normal(position);
  vec3 L = normalize(light - position);
  float diffuse = max(0.0, dot(N, L));

  return material == 0 ? color : color * diffuse;
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
  
  vec3 position = ro + rd * t;
  vec3 color = shade(position, hit);
  FragColor = vec4(color, 1.0);
}
