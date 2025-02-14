﻿#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_BACKGROUND_UBO_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_BACKGROUND_UBO_DECLARATION_FX_H

namespace BABYLON {

extern const char* backgroundUboDeclaration;

const char* backgroundUboDeclaration
  = R"ShaderCode(

layout(std140, column_major) uniform;

uniform Material
{
    uniform vec4 vPrimaryColor;
    uniform vec4 vPrimaryColorShadow;
    uniform vec2 vDiffuseInfos;
    uniform vec2 vReflectionInfos;
    uniform mat4 diffuseMatrix;
    uniform mat4 reflectionMatrix;
    uniform vec3 vReflectionMicrosurfaceInfos;
    uniform float fFovMultiplier;

    uniform float pointSize;
    uniform float shadowLevel;
    uniform float alpha;

    #if defined(REFLECTIONFRESNEL) || defined(OPACITYFRESNEL)
        uniform vec3 vBackgroundCenter;
    #endif

    #ifdef REFLECTIONFRESNEL
        uniform vec4 vReflectionControl;
    #endif
};

uniform Scene {
    mat4 viewProjection;
#ifdef MULTIVIEW
    mat4 viewProjectionR;
#endif
    mat4 view;
};

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_BACKGROUND_UBO_DECLARATION_FX_H
