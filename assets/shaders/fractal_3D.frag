#version 460

out vec4 FragColor;
in vec2 uv;

uniform float u_time;
uniform vec2 u_resolution;

uniform mat3 u_camRot;
uniform vec3 u_camPos;
uniform float u_fov;

vec3 palette( float t ) {
  vec3 a = vec3(0.5, 0.5, 0.5);
  vec3 b = vec3(0.5, 0.5, 0.5);
  vec3 c = vec3(1.0, 1.0, 1.0);
  vec3 d = vec3(0.263,0.416,0.557);

  return a + b*cos( 6.28318*(c*t+d) );
}

float smin( float d1, float d2, float k )
{
  float h = clamp( 0.5 + 0.5*(d2-d1)/k, 0.0, 1.0 );
  return mix( d2, d1, h ) - k*h*(1.0-h);
}

float sdSphere(vec3 p, float r)
{
  return length(p) - r;
}

float sdBox(vec3 p, vec3 b)
{
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float sdOctahedron( vec3 p, float s )
{
  p = abs(p);
  float m = p.x+p.y+p.z-s;
  vec3 q;
       if( 3.0*p.x < m ) q = p.xyz;
  else if( 3.0*p.y < m ) q = p.yzx;
  else if( 3.0*p.z < m ) q = p.zxy;
  else return m*0.57735027;
    
  float k = clamp(0.5*(q.z-q.y+s),0.0,s); 
  return length(vec3(q.x,q.y-s+k,q.z-k)); 
}

mat2 rotate(float angle)
{
  float s = sin(angle);
  float c = cos(angle);
  return mat2(c, -s, s, c);
}

float map(vec3 p)
{
  p.z += u_time * 0.4;
  p.xy = fract(p.xy) - 0.5;     // Repeat along X & Y every unit (subtract half of 1 to re-centre)
  p.z = mod(p.z, 0.25) - 0.125; // Repeat along Z every 0.25 units (subtract half of 0.25 to re-centre)
  
  float box = sdOctahedron(p, 0.15);
  return box;
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
      
      // Uncomment to twirl (animate) rays
      //p.y += sin(t + u_time * 2.0) * 0.35;
      //p.x += cos(t + u_time * 2.0) * 0.25;
      
      float d = map(p);
      t += d;
      
      if (t < 0.001) break; // Near plane
      if (t > 100.0) break; // Far plane
  }
  
  vec3 color = palette(t * 0.04);
  
  // Color based on raymarching result
  FragColor = vec4(color, 1.0);
}