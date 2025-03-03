#version 410

// https://www.shadertoy.com/view/MdfXRs

// #include "_COOL_RES_/shaders/math.glsl"

layout(location = 0) in vec2 _uv;
uniform float _time;
uniform float _aspect_ratio;
out vec4      out_Color;

uniform sampler2D _image;

// #include "_COOL_RES_/shaders/input_definitions.glsl"
// #include "_ROOT_FOLDER_/res/shader-lib/image.glsl"

INPUT float center_x;   // default 0.5 range 0 to 1
INPUT float center_y;   // default 0.5 range 0 to 1
INPUT float distortion; // 0 to 1

void main()
{
    vec2 uv = _uv;

    vec2  med    = vec2(center_x, center_y); // (uv*0.5) - 1.0;
    vec2  disVec = med - uv;
    float l      = length(disVec);
    float ll     = distortion - l * l;
    vec2  dist   = med - disVec * ll;

    out_Color = image(dist);
}