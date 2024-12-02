#version 460
const float EPSILON = 0.0001;

out vec4 FragColor;
in vec2 uv;

uniform vec3 u_camPos;
uniform mat3 u_camRot;
uniform vec2 u_resolution;
uniform float u_fov;
uniform float u_near;
uniform float u_far;

const int PLANET_COUNT = 9;

uniform mat4 u_planetMatrices[PLANET_COUNT];
uniform vec3 u_planetColors[PLANET_COUNT];
uniform float u_planetRadii[PLANET_COUNT];
uniform vec3 u_sunPos;

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
  nearest.dist = u_far;
  nearest.idx = -1;

  for (int i = 0; i < PLANET_COUNT; i++)
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

// Input: depth in linear-space between [near, far].
// Output: depth in non-linear sace between [0, 1].
float encode(float depth, float near, float far)
{
	float invDepth = 1.0 / depth;
	float invNear = 1.0 / near;
	float invFar = 1.0 / far;
	return (invDepth - invNear) / (invFar - invNear);
}

// Input: depth in non-linear space between [0, 1].
// Output: depth in linear space between [near, far].
float decode(float depth, float near, float far)
{
	depth = depth * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near - depth * (far - near));
}

// Input: depth in linear space between [near, far].
// Output: depth in linear space between [0, 1].
float normalizeDepth(float depth, float near, float far)
{
	return (depth - near) / (far - near);
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
    if (d <= EPSILON)
    {
      hit = obj.idx;
      break;
    }
    
    // Ray is too far from anything, abort mission!
    if (t >= u_far)
      break;
  }

  // Discard if no intersection found or in-front of near-plane
  if (hit < 0 || t <= u_near)
    discard;
  
  // World-space lighting
  vec3 position = ro + rd * t;
  vec3 color = shade(position, hit);

  // Scale ray-length based on screen-position to ensure consistent depth
  vec3 rd0 = normalize(vec3(uv * u_fov, -1.0));
  float z = dot(rd0, vec3(0.0, 0.0, -1.0)) * t;
  float depth = encode(z, u_near, u_far);

  // Depth visualization
  //depth = decode(depth, u_near, u_far);
  //depth = normalizeDepth(depth, u_near, u_far);
  //color = vec3(depth);

  gl_FragDepth = depth;
  FragColor = vec4(color, 1.0);
}
