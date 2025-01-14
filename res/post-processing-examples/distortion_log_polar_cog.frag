#version 410

// https://www.shadertoy.com/view/7tGGDW

// #include "_COOL_RES_/shaders/math.glsl"

layout(location = 0) in vec2 _uv;
uniform float _time;
uniform float _aspect_ratio;
out vec4      out_Color;

uniform sampler2D _image;

// #include "_COOL_RES_/shaders/input_definitions.glsl"
// #include "_ROOT_FOLDER_/res/shader-lib/image.glsl"

INPUT float center_x;       // default 0.5 range 0 to 1
INPUT float center_y;       // default 0.5 range 0 to 1
INPUT float scale;          //  range 0 to 7
INPUT float details;        //
INPUT Angle angle_in_turns; // 0 to 1 (1 == 1 turn)
INPUT float strip;          // default 0.01

INPUT int nb_div;

float thc(float a, float b)
{
    return tanh(a * cos(b)) / tanh(a);
}

float ths(float a, float b)
{
    return tanh(a * sin(b)) / tanh(a);
}

vec2 thc(float a, vec2 b)
{
    return tanh(a * cos(b)) / tanh(a);
}

vec2 ths(float a, vec2 b)
{
    return tanh(a * sin(b)) / tanh(a);
}

vec3 pal(in float t, in vec3 a, in vec3 b, in vec3 c, in vec3 d)
{
    return a + b * cos(6.28318 * (c * t + d));
}

float h21(vec2 a)
{
    return fract(sin(dot(a.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

float mlength(vec2 uv)
{
    return max(abs(uv.x), abs(uv.y));
}

void main()
{
    vec2 center = vec2(center_x, center_y);
    vec2 uv     = _uv - center;

    uv /= cosh(length(uv) * scale);

    float a = atan(uv.y, uv.x);
    float r = length(uv);

    uv = vec2(thc(2., a), thc(2., strip * thc(3., nb_div * a + angle_in_turns * TAU * 6.) + log(r) - scale));

    float sc = 0. + 1.5 * (1. + strip * thc(3., log(r))) * thc(1., details * r);
    // sc = tan(sc);
    // vec2 ipos = floor(sc * uv) + 0.5;
    vec2 fpos = fract(sc * uv) - 0.5;

    float d = length(fpos);
    float v = 0.5 + 0.5 * cos(6. * uv.y);
    float k = 0.4 * (1. - v);                                         // anything * (1.-v) works
    float s = smoothstep(-k, k, v - d) - smoothstep(-k, k, 0.03 - d); // was 0.1

    // s * X + ..., higher X = more distortion
    // s        = cos(s * 4. + 2. * a + 8. * r + time_mod);
    vec3 col = image(s * pal(uv.y + s, vec3(1.), vec3(1.), vec3(1.), vec3(0., uv.y, 2. * uv.y)).xy * angle_in_turns * TAU * 6. - center);

    // Output to screen
    out_Color = vec4(col, 1.0);
}