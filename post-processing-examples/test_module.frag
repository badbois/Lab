#version 430

layout(location = 0) in vec2 _uv;
out vec4 out_Color;

// #include "_ROOT_FOLDER_/shader-lib/define_types.glsl"

uniform RgbColor _color;
uniform RgbColor _color2;
uniform float    _circle_radius;
uniform float    _aspect_ratio;
uniform float    _time;

// #include "_ROOT_FOLDER_/shader-lib/define_types.glsl"

void main()
{
    vec2  uv       = (_uv - 0.5) * vec2(_aspect_ratio, 1.);
    float radius   = _circle_radius - 0.1 * cos(3. * _time);
    float presence = smoothstep(-0.001, 0.001, abs(length(uv) - radius) - 0.02);
    out_Color      = presence * vec4(mix(_color, _color2, uv.y), 1.f);
}