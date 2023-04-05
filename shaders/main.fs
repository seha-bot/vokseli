#version 330 core
uniform vec2 uResolution;
uniform float uTime;

#define MAX_STEPS 100
#define MAX_RAY 100.0
#define MIN_RAY 0.0001

mat2 rot(float a)
{
    float s = sin(a);
    float c = cos(a);
    return mat2(c, -s, s, c);
}

float nearest(vec3 p)
{
    vec3 k = p-vec3(1.0, 0.0, 5.0);
    k.xz *= rot(uTime);
    float fkocka = length(max(abs(k)-0.5,0.0));
    float fkockica = length(max(abs(p-vec3(1.0+sin(uTime), 2.0, 5.0+cos(uTime)*3.0))-0.1,0.0));
    return min(min(fkocka, fkockica), abs(p.y + 1.0));
}

float march(vec3 ro, vec3 rd)
{
    float total = 0.0, safe = 1.0;
    for(int i = 0; i < MAX_STEPS && total < MAX_RAY && safe > MIN_RAY; i++)
    {
        total += safe = nearest(ro + rd * total);
    }
    return total;
}

vec3 normal(vec3 p)
{
    vec2 o = vec2(0.001, 0.0);
    return normalize(vec3(
        nearest(p+o.xyy) - nearest(p-o.xyy),
        nearest(p+o.yxy) - nearest(p-o.yxy),
        nearest(p+o.yyx) - nearest(p-o.yyx)
    ));
}

float light(vec3 p)
{
    vec3 sun = vec3(-5.0, 10.0, -5.0);
    vec3 dir = sun - p;
    vec3 ndir = normalize(dir);
    vec3 npos = normal(p);
    float hit = march(p + npos * 0.05, ndir);
    return dot(ndir, npos) * (length(dir) > hit ? 0.1 : 1.0);
}

void main()
{
    vec2 uv = gl_FragCoord.xy / uResolution - 0.5;
    uv.x *= uResolution.x / uResolution.y;

    vec3 cam = vec3(0.0, 1.0, 0.0);
    vec3 dir = normalize(vec3(uv, 1.0));

    float ray = march(cam, dir);
    vec3 c = vec3(0.0);
    if(ray < MAX_RAY)
    {
        vec3 p = cam + dir * ray;
        c = vec3(light(p));
    }

    gl_FragColor = vec4(c, 1.0f);
}

