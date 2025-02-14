﻿#ifndef BABYLON_SHADERS_LAYER_FRAGMENT_FX_H
#define BABYLON_SHADERS_LAYER_FRAGMENT_FX_H

namespace BABYLON {

extern const char* layerPixelShader;

const char* layerPixelShader
  = R"ShaderCode(

// Samplers
varying vec2 vUV;
uniform sampler2D textureSampler;

// Color
uniform vec4 color;

// Helper functions
#include<helperFunctions>

void main(void) {
    vec4 baseColor = texture2D(textureSampler, vUV);

#ifdef LINEAR
    baseColor.rgb = toGammaSpace(baseColor.rgb);
#endif

#ifdef ALPHATEST
    if (baseColor.a < 0.4)
        discard;
#endif



    gl_FragColor = baseColor * color;
}

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_LAYER_FRAGMENT_FX_H
