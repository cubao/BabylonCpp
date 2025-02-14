﻿#ifndef BABYLON_SHADERS_SPRITES_FRAGMENT_FX_H
#define BABYLON_SHADERS_SPRITES_FRAGMENT_FX_H

namespace BABYLON {

extern const char* spritesPixelShader;

const char* spritesPixelShader
  = R"ShaderCode(

uniform bool alphaTest;

varying vec4 vColor;

// Samplers
varying vec2 vUV;
uniform sampler2D diffuseSampler;

// Fog
#include<fogFragmentDeclaration>

void main(void) {
    vec4 color = texture2D(diffuseSampler, vUV);

    if (alphaTest)
    {
        if (color.a < 0.95)
            discard;
    }

    color *= vColor;

#include<fogFragment>

    gl_FragColor = color;

#include<imageProcessingCompatibility>
}

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SPRITES_FRAGMENT_FX_H
