// #include "_ROOT_FOLDER_/res/shader-lib/rgb_hsv_conversion.glsl"

vec3 hue(
    vec3 in_color, float effect_intensity,
    float hue
)
{
    vec3 image_hsl = rgb2hsv(in_color);
    image_hsl.x    = fract(image_hsl.x + hue);
    vec3 out_color = hsv2rgb(image_hsl);
    return mix(in_color, out_color, effect_intensity);
}