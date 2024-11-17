#version 460
const float EPSILON = 0.0001;

out vec4 FragColor;
in vec2 uv;

uniform float u_time;
uniform vec2 u_resolution;

uniform mat3 u_camRot;
uniform vec3 u_camPos;
uniform float u_fov;

uniform mat4 u_transform1;
uniform mat4 u_transform2;
uniform mat4 u_transform3;

mat2 rotate(float angle)
{
  float s = sin(angle);
  float c = cos(angle);
  return mat2(c, -s, s, c);
}

float sdSphere(vec3 p, float r)
{
  return length(p) - r;
}

float sdBox(vec3 p, vec3 b)
{
  vec3 q = abs(p) - b;
  return length(max(q, 0.0)) + min(max(q.x, max(q.y,q.z)), 0.0);
}

float sdOctahedron( vec3 p, float s)
{
  p = abs(p);
  return (p.x + p.y + p.z - s) * 0.57735027;
}

float sdTorus( vec3 p, vec2 t )
{
  vec2 q = vec2(length(p.xz) - t.x, p.y);
  return length(q) - t.y;
}

// Less intrusive than making map output a material index because we have to call map in normal()
struct object
{
  float dist;   // distance
  int idx;      // material index
};

object map(vec3 p)
{
  // Apply 3d rotations using a 2d matrix by multiplying by the axes you *don't* want to rotate about
  // ie a rotation about y changes xz values whereas y remains unchanged.
  mat2 rotation = rotate(radians(100.0) * u_time);
  vec3 translation = vec3(5.0, 0.0, 0.0);

  // Since we're in camera-space, we apply transformations in opposite order
  // So if scale then rotate then translate in view-space, we must translate then rotate then scale in camera space
  // Also, rotations in camera-space are clockwise instead of counter-clockwise
  vec3 extents = vec3(1.0, 1.0, 1.0);

  // Transforming the "right" way (rotate about origin -- "in-place"):
  vec3 a = p;
  a -= translation;
  a.xy *= rotation;
  float box = sdBox(a, extents);

  // Transforming the "wrong" way (rotate about translation -- "orbit"):
  vec3 b = p;
  b.xy *= rotation;
  b -= translation;
  float sphere = sdSphere(b, 1.0);

  vec3 c = (u_transform1 * vec4(p, 1.0)).xyz; //p;
  //c.xz *= rotation;
  float oct = sdOctahedron(c, 1.0);

  //vec3 lemniscate = vec3(sin(u_time * 4.0) * 6.0, sin(u_time * 8.0) * 2.0, 0.0);
  //float fig8 = sdSphere(p - lemniscate, 1.0);
  vec3 d = (u_transform2 * vec4(p, 1.0)).xyz;
  float fig8 = sdTorus(d, vec2(1.0, 0.25));

  vec3 e = (u_transform3 * vec4(p, 1.0)).xyz;
  float fig8Child = sdTorus(e, vec2(1.0, 0.25));

  const int count = 5;
  float sdfs[count] = { box, sphere, oct, fig8, fig8Child };
  object obj;
  obj.dist = 10000.0;
  obj.idx = -1;
  for (int i = 0; i < count; i++)
  {
    if (sdfs[i] < obj.dist)
    {
      obj.dist = sdfs[i];
      obj.idx = i;
    }
  }
  return obj;
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
  vec3 colors[] = {
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0),
    vec3(1.0, 0.0, 1.0),
    vec3(0.0, 1.0, 1.0)
  };

  vec3 color = colors[material];
  vec3 light = vec3(cos(u_time) * 5.0, 5.0, 5.0);

  vec3 N = normal(position);
  vec3 L = normalize(light - position);
  float dotNL = max(0.0, dot(N, L));

  return color * dotNL;
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
  
  int idx = -1;
  for (int i = 0; i < 64; i++)
  {
    // Position along ray
    vec3 p = ro + rd * t;
    
    // Distance of current step
    object obj = map(p);

    // Distance along ray
    t += obj.dist;
    
    // Hit success if step is very small
    if (obj.dist < 0.01)
    {
      idx = obj.idx;
      break;
    }

    // Hit fail if distance exceeds far plane
    if (t > 100.0)
      break;
  }

  vec3 p = ro + rd * t;
  vec3 color = idx >= 0 ? shade(p, idx) : vec3(1.0, 1.0, 1.0);
  FragColor = vec4(color, 1.0);
}
