#version 410

layout(location = 0) in vec2 _uv;
uniform float _time;
uniform float _aspect_ratio;
out vec4      out_Color;

uniform sampler2D _image;

// #include "_COOL_RES_/shaders/input_definitions.glsl"
// #include "_ROOT_FOLDER_/res/shader-lib/image.glsl"
// #include "_ROOT_FOLDER_/res/shader-lib/tint_shadows_and_highlights.glsl"

INPUT RgbColor Color_shadows;
INPUT RgbColor Color_highlights;

INPUT float Shadows_intensity;
INPUT float Highlights_intensity;

INPUT float Effect_intensity;

void main()
{
    vec3 in_color = image(_uv);

    vec3 out_color = tint_shadows_and_highlights(
        in_color, Effect_intensity,
        Color_shadows, Color_highlights, Shadows_intensity, Highlights_intensity
    );

    out_Color = vec4(out_color, 1.);
}