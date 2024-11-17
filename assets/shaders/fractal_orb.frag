#version 460
const float EPSILON = 0.0001;

out vec4 FragColor;
in vec2 uv;

uniform float u_time;
uniform vec2 u_resolution;

vec3 palette(float d)
{
	return mix(vec3(0.2, 0.7 ,0.9), vec3(1.0, 0.0, 1.0), d);
}

vec2 rotate(vec2 p,float a)
{
	float c = cos(a);
    float s = sin(a);
    return p * mat2(c, s, -s, c);
}

float map(vec3 p)
{
    // Original, (loop repeats objects):
    for( int i = 0; i < 8; ++i)
    {
        float t = u_time * 0.2;
        p.xz = rotate(p.xz, t);
        p.xy = rotate(p.xy, t * 1.89);
        p.xz = abs(p.xz);
        p.xz -= 0.5;
	}

    // Disabled loop to understand what is being rendered before repitition
    //float t = u_time * 0.2;
    //p.xz = rotate(p.xz, t);
    //p.xy = rotate(p.xy, t * 1.89);
    //p.xz = abs(p.xz);
    //p.xz -= 0.5;

    // Mirrors about xz due to abs xz + rotation xz then taking sign of all 3 axes (only sign of y is unchanged)
	return dot(sign(p), p) / 5.0;
}

vec4 rm (vec3 ro, vec3 rd)
{
    float t = 0.0;
    vec3 col = vec3(0.0);
    float d;
    for(float i = 0.0; i < 64.0; i++)
    {
		vec3 p = ro + rd * t;
        d = map(p) * 0.5;

        if(d < 0.02)
        {
            break;
        }
        if(d > 100.0)
        {
        	break;
        }

        col += palette(length(p) * 0.1) / (400.0 * (d));
        t += d;
    }
    return vec4(col, 1.0 / (d * 100.0));
}

void main()
{
    vec2 uv = (gl_FragCoord.xy - (u_resolution / 2.0)) / u_resolution.x;

    // Rotates rays about y
	vec3 ro = vec3(0.0, 0.0, -50.0);
    ro.xz = rotate(ro.xz, u_time);

    vec3 cf = normalize(-ro);
    vec3 cs = normalize(cross(cf, vec3(0.0, 1.0, 0.0)));
    vec3 cu = normalize(cross(cf, cs));
    
    // Scrolls uvs in 3d based on local rotation axes
    vec3 uuv = ro + cf * 3.0 + uv.x * cs + uv.y * cu;
    vec3 rd = normalize(uuv - ro);

    // Uncomment this and comment the loop within map() to deconstruct this shader!
    //vec3 rd = normalize(vec3(uv, 1.0));
    vec4 col = rm(ro, rd);

    FragColor = col;
}
