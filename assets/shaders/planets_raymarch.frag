#version 460

out vec4 FragColor;
in vec2 uv;

uniform float u_time;
uniform vec2 u_resolution;

uniform mat3 u_camRot;
uniform vec3 u_camPos;
uniform float u_fov;

uniform vec3 u_pos;

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
  // Formerly length(p - t) - r;
  // Removed because this forces us to translate *last*
  return length(p) - r;
}

// Since raymarching is done in camera-space instead of view-space, transformations are 1) inverted and 2) reversed.
// "inverted" --> subtract a positive number to translate in the positive direction.
// "reversed" --> translate then rotate to "rotate in-place".
// (This behaviour makes sense if you visualize the effects on an individual ray, ie "how does the scene change if we move a ray 5 units left").
float map(vec3 p)
{
  vec3 extents = vec3(1.0, 1.0, 1.0);
  vec3 translation = vec3(5.0, 0.0, 0.0);
  mat2 rotation = rotate(radians(100.0) * u_time);

  // 1) Translation then rotation (rotates in-place)
  vec3 pos1 = p;
  pos1 -= translation;
  pos1.xz *= rotation;
  float box1 = sdBox(pos1, extents);

  // 2) Rotation then translation (rotates about x = 5)
  vec3 pos2 = p;
  pos2.xz *= rotation;
  pos2 -= translation;
  float box2 = sdBox(pos2, extents);

  return min(box1, box2);
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
  
  int hit = 0;
  for (int i = 0; i < 64; i++)
  {
    // Position in scene
    vec3 p = ro + rd * t;
    
    float d = map(p);
    t += d;
    
    // Ray is near an object, register a hit!
    if (d < 0.01)
    {
      hit = 1;
      break;
    }
    
    // Ray is too far from anything, abort mission!
    if (t > 100.0)
      break;
  }
  
  // Colour based on ray distance
  vec3 color = hit > 0 ? vec3(1.0, 0.0, 0.0) : vec3(1.0, 1.0, 1.0);
  FragColor = vec4(color, 1.0);
}
