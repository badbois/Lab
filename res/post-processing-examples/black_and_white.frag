#version 410

layout(location = 0) in vec2 _uv;
uniform float _time;
uniform float _aspect_ratio;
out vec4      out_Color;

uniform sampler2D _image;

// #include "_COOL_RES_/shaders/input_definitions.glsl"
// #include "_ROOT_FOLDER_/res/shader-lib/image.glsl"
// #include "_ROOT_FOLDER_/res/shader-lib/black_and_white.glsl"

INPUT vec3 Channels_contribution; // default vec3(0.2126, 0.7152, 0.0722)
INPUT bool Normalize;             // default true

INPUT float Effect_intensity; // default 1. min 0. max 1.

void main()
{
    vec3 in_color = image(_uv);

    vec3 out_color = black_and_white(
        in_color, Effect_intensity,
        Channels_contribution, Normalize
    );

    out_Color = vec4(out_color, 1.);
}
