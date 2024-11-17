#version 460
const float EPSILON = 0.0001;

out vec4 FragColor;
in vec2 uv;

uniform float u_time;
uniform vec2 u_resolution;

uniform mat3 u_camRot;
uniform vec3 u_camPos;
uniform float u_fov;

uniform mat4 u_transform;

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

float map(vec3 p)
{
  // Apply 3d rotations using a 2d matrix by multiplying by the axes you *don't* want to rotate about
  // ie a rotation about y changes xz values whereas y remains unchanged.
  mat2 rotation = rotate(radians(100.0) * u_time);
  vec3 translation = vec3(5.0, 0.0, 0.0);

  // Since we're in camera-space, we apply transformations in opposite order
  // So if scale then rotate then translate in view-space, we must translate then rotate then scale in camera space
  vec3 extents = vec3(1.0, 1.0, 1.0);

  // Transforming the "right" way (rotate around origin):
  vec3 q = p;
  q -= translation;
  q.xy *= rotation;
  float box = sdBox(q, extents);

  // Transforming the "wrong" way (rotate around translation):
  vec3 r = p;
  r.xy *= rotation;
  r -= translation;
  float sphere = sdSphere(r, 1.0);

  vec3 s = p;
  s.xz *= rotation;
  float oct = sdOctahedron(s, 1.0);

  vec3 lemniscate = vec3(sin(u_time * 4.0) * 6.0, sin(u_time * 8.0) * 2.0, 0.0);
  float fig8 = sdSphere(p - lemniscate, 1.0);

  const int count = 4;
  float sdfs[count] = { box, sphere, oct, fig8 };
  float d = 10000.0;
  for (int i = 0; i < count; i++)
    d = min(d, sdfs[i]);
  return d;
}

vec3 normal(vec3 p)
{
    return normalize(vec3(
        map(vec3(p.x + EPSILON, p.y, p.z)) - map(vec3(p.x - EPSILON, p.y, p.z)),
        map(vec3(p.x, p.y + EPSILON, p.z)) - map(vec3(p.x, p.y - EPSILON, p.z)),
        map(vec3(p.x, p.y, p.z  + EPSILON)) - map(vec3(p.x, p.y, p.z - EPSILON))
    ));
}

vec3 shade(vec3 position)
{
  vec3 light = vec3(cos(u_time) * 5.0, 5.0, 5.0);
  vec3 color = vec3(1.0, 0.0, 0.0);

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
  
  int hit = -1;
  for (int i = 0; i < 64; i++)
  {
    // Position along ray
    vec3 p = ro + rd * t;
    
    // Distance of current step
    float d = map(p);

    // Distance along ray
    t += d;
    
    // Hit success if step is very small
    if (d < 0.01)
    {
      hit = 0;
       break;
    }

    // Hit fail if distance exceeds far plane
    if (t > 100.0)
      break;
  }
  vec3 p = ro + rd * t;
  vec3 color = hit >= 0 ? shade(p) : vec3(1.0, 1.0, 1.0);
  FragColor = vec4(color, 1.0);
}
