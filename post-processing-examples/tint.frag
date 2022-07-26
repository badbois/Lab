#version 410

layout(location = 0) in vec2 _uv;
uniform float _time;
uniform float _aspect_ratio;
out vec4      out_Color;

uniform sampler2D _image;

// #include "_COOL_RES_/shaders/input_definitions.glsl"
// #include "_ROOT_FOLDER_/shader-lib/image.glsl"
// #include "_ROOT_FOLDER_/shader-lib/tint.glsl"

INPUT float Tint; // default 0.0 / default bounds -1.0 -> 1.0

INPUT float Effect_intensity;

void main()
{
    vec3 in_color = image(_uv);

    vec3 out_color = tint(
        in_color, Effect_intensity,
        Tint
    );

    out_Color = vec4(out_color, 1.);
}