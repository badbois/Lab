#version 430

layout(location = 0) in vec2 _uv;
uniform float _time;
uniform float _aspect_ratio;
out vec4      out_Color;

uniform sampler2D _image;

// #include "_ROOT_FOLDER_/shader-lib/define_types.glsl"
// #include "_ROOT_FOLDER_/shader-lib/rgb_hsv_conversion.glsl"
// #include "_ROOT_FOLDER_/shader-lib/image.glsl"
// #include "_ROOT_FOLDER_/shader-lib/color_detection.glsl"

// BEGIN DYNAMIC PARAMS

uniform RgbColor Color;

uniform float Saturation;

uniform float Effect_intensity;

// END DYNAMIC PARAMS

void main()
{
    RgbColor in_color  = image(_uv);
    float    hue       = color_detection(in_color, Color);
    vec3     image_hsl = rgb2hsv(in_color);
    image_hsl += vec3(0., Saturation * hue, 0.);
    RgbColor out_color = hsv2rgb(image_hsl);
    out_Color          = vec4(out_color, 1.);
}