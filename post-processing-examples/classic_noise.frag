//
// GLSL textureless classic 2D noise "cnoise",
// with an RSL-style periodic variant "pnoise".
// Author:  Stefan Gustavson (stefan.gustavson@liu.se)
// Version: 2011-08-22
//
// Many thanks to Ian McEwan of Ashima Arts for the
// ideas for permutation and gradient selection.
//
// Copyright (c) 2011 Stefan Gustavson. All rights reserved.
// Distributed under the MIT license. See LICENSE file.
// https://github.com/stegu/webgl-noise

#version 410

layout(location = 0) in vec2 _uv;
uniform float _time;
uniform float _aspect_ratio;
out vec4      out_Color;

uniform sampler2D _image;

// #include "_COOL_RES_/shaders/input_definitions.glsl"
// #include "_ROOT_FOLDER_/shader-lib/image.glsl"
// #include "_ROOT_FOLDER_/shader-lib/classic_noise.glsl"

input float Frequency;

input float Scale; // 100

input float a; // min = 1, max = 3; default = 1.5;  `a` increases the fractalness of the noise

input float Effect_intensity;

void main()
{
    vec2 uv = _uv;
    uv.x *= _aspect_ratio;

    float out_color = classic_noise(
        uv,
        Frequency, a, Scale
    );

    out_Color = vec4(RgbColor(out_color), 1.);
}
