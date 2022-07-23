#version 410

// #include "_COOL_RES_/shaders/math.glsl"

layout(location = 0) in vec2 _uv;
uniform float _time;
uniform float _aspect_ratio;
out vec4      out_Color;

uniform sampler2D _image;

// #include "_COOL_RES_/shaders/input_definitions.glsl"

input Angle angle_in_turns;    // 0 to 1 (1 == 1 turn)
input float hole_radius;       // positive values
input float hole_intensity;    // 0 to 8
input float hole_attractivity; // -1 to 1
input float hole_smooth;       // -.5 to 1
input float hole_center_x;     // default 0.5 range 0 to 1
input float hole_center_y;     // default 0.5 range0 to 1

input RgbColor hole_color;

input int nb_div; // 0 forbidden

RgbColor blob(vec2 point, vec2 uv, RgbColor color, float invIntensity, float invGlow)
{
    float d    = distance(uv, point);
    float invD = 1.0 / d;
    invD       = pow(invD / invIntensity, invGlow);
    return color * invD;
}

vec4 image(vec2 uv)
{
    return texture2D(_image, uv);
}

void main()
{
    // [0, res] to [-1, 1]
    vec2 hole_center = vec2(hole_center_x, hole_center_y);
    vec2 coord       = -hole_center + (_uv)*2.;
    vec2 ncoord      = -hole_center + coord * vec2(_aspect_ratio, 1.0);

    // change look at
    // ncoord.x += sin(radians(time_mod * 40.0)) * .4;
    // ncoord.y += cos(radians(time_mod * 40.0)) * .7;

    // rotation
    float angleRot = angle_in_turns * TAU;
    vec2  tcoord   = vec2(ncoord.x * cos(angleRot) - ncoord.y * sin(angleRot), ncoord.y * cos(angleRot) + ncoord.x * sin(angleRot));

    // sink hole projection
    float angle  = atan(tcoord.x, tcoord.y);
    float radius = length(tcoord);
    vec2  uv     = vec2(angle, radius);

    uv /= radians(360. / float(nb_div));
    uv.y += hole_attractivity; // scrolling

    vec4 col = texture(_image, uv);

    // hole
    float invIntensity = (6.0 + sin(radians(hole_intensity * 40)) * 5.8) / (hole_radius);
    float invGlow      = hole_smooth;
    col.rgb += blob(vec2(0, 0), tcoord, -hole_color, invIntensity, invGlow);

    out_Color = col;
}