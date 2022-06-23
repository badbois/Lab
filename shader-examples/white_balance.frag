#version 430

layout(location = 0) in vec2 _uv;
uniform float _time;
uniform float _aspect_ratio;
out vec4      out_Color;

uniform sampler2D _image;

const mat3 M             = mat3(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
const mat3 xfm_cat       = mat3(.40024, -.2263, 0.0, .7076, 1.16532, 0.0, -.0881, 0.0457, .91822);
const mat3 inv_xfm_cat   = mat3(1.860, 0.361, 0.0, -1.12938162e+00, 6.38812463e-01, 0.0, 2.19897410e-01, -6.37059684e-06, 1.08906362e+00);
const vec3 xyz_D65       = vec3(95.04, 100.0, 108.88);
const mat3 sRGBtoXYZ     = mat3(0.4124564, 0.2126729, 0.0193339, 0.3575761, 0.7151522, 0.1191920, 0.1804375, 0.0721750, 0.9503041);
const mat3 inv_sRGBtoXYZ = mat3(3.24045484, -0.96926639, 0.05564342, -1.53713885, 1.87601093, -0.20402585, -0.49853155, 0.04155608, 1.05722516);

// BEGIN DYNAMIC PARAMS

uniform vec2 Coordinate_for_white_balance;

// END DYNAMIC PARAMS

vec4 image(vec2 uv)
{
    return texture2D(_image, uv);
}

vec3 xy2XYZ(vec2 xy, float Y)
{
    return vec3(xy[0] / xy[1] * Y, Y, (1.0 - xy[0] - xy[1]) / xy[1] * Y);
}

vec2 XYZ2xy(vec3 XYZ)
{
    float sum0 = XYZ[0] + XYZ[1] + XYZ[2];
    return vec2(XYZ[0] / sum0, XYZ[1] / sum0);
}

mat3 cbCAT(vec3 xyz_est, vec3 xyz_target)
{
    vec3 gain1  = xfm_cat * xyz_target;
    vec3 gain2  = xfm_cat * xyz_est;
    vec3 gain3  = gain1 / gain2;
    mat3 outMat = inv_xfm_cat * mat3(gain3[0], 0.0, 0.0, 0.0, gain3[1], 0.0, 0.0, 0.0, gain3[2]);
    outMat      = outMat * xfm_cat;
    outMat      = inv_sRGBtoXYZ * outMat;
    outMat      = outMat * sRGBtoXYZ;
    return outMat;
}

/*uniform vec3 color_pick=vec3(iMouse.x,iMouse.y,1.0);*/
void main()
{
    // vec3 color_pick = texture(iChannel0,-iMouse.xy/iResolution.xy).rgb;
    vec2 uv = _uv;
    uv.x *= _aspect_ratio;
    vec3 base       = image(_uv).rgb;
    vec2 color_pick = Coordinate_for_white_balance;
    color_pick.x *= _aspect_ratio;
    float dist          = length(uv - color_pick);
    float r             = 0.01;
    float circle_picker = 0;
    if (dist <= r)
        circle_picker = 1.;
    else if (dist > r && dist < r + 0.001)
        circle_picker = -1.;
    // float circle_picker = smoothstep(r, r - 0.001, dist);
    vec2 xyEst  = XYZ2xy(sRGBtoXYZ * image(Coordinate_for_white_balance).rgb);
    vec3 xyzEst = xy2XYZ(xyEst, 100.0);
    mat3 M      = cbCAT(xyzEst, xyz_D65);
    vec3 res    = M * base + circle_picker;
    out_Color   = vec4(res, 1.0);
}