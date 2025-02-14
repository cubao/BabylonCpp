﻿#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_PBR_FRAGMENT_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_PBR_FRAGMENT_DECLARATION_FX_H

namespace BABYLON {

extern const char* pbrFragmentDeclaration;

const char* pbrFragmentDeclaration
  = R"ShaderCode(

uniform vec3 vReflectionColor;
uniform vec4 vAlbedoColor;

// CUSTOM CONTROLS
uniform vec4 vLightingIntensity;

uniform vec4 vReflectivityColor;
uniform vec4 vMetallicReflectanceFactors;
uniform vec3 vEmissiveColor;

uniform float visibility;

// Samplers
#ifdef ALBEDO
uniform vec2 vAlbedoInfos;
#endif

#ifdef AMBIENT
uniform vec4 vAmbientInfos;
#endif

#ifdef BUMP
uniform vec3 vBumpInfos;
uniform vec2 vTangentSpaceParams;
#endif

#ifdef OPACITY
uniform vec2 vOpacityInfos;
#endif

#ifdef EMISSIVE
uniform vec2 vEmissiveInfos;
#endif

#ifdef LIGHTMAP
uniform vec2 vLightmapInfos;
#endif

#ifdef REFLECTIVITY
uniform vec3 vReflectivityInfos;
#endif

#ifdef MICROSURFACEMAP
uniform vec2 vMicroSurfaceSamplerInfos;
#endif

// Refraction Reflection
#if defined(REFLECTIONMAP_SPHERICAL) || defined(REFLECTIONMAP_PROJECTION) || defined(SS_REFRACTION)
uniform mat4 view;
#endif

// Reflection
#ifdef REFLECTION
    uniform vec2 vReflectionInfos;
    #ifdef REALTIME_FILTERING
        uniform vec2 vReflectionFilteringInfo;
    #endif
    uniform mat4 reflectionMatrix;
    uniform vec3 vReflectionMicrosurfaceInfos;

    #if defined(USE_LOCAL_REFLECTIONMAP_CUBIC) && defined(REFLECTIONMAP_CUBIC)
        uniform vec3 vReflectionPosition;
        uniform vec3 vReflectionSize;
    #endif
#endif

// Clear Coat
#ifdef CLEARCOAT
    uniform vec2 vClearCoatParams;
    uniform vec4 vClearCoatRefractionParams;

    #if defined(CLEARCOAT_TEXTURE) || defined(CLEARCOAT_TEXTURE_ROUGHNESS)
        uniform vec4 vClearCoatInfos;
    #endif

    #ifdef CLEARCOAT_TEXTURE
        uniform mat4 clearCoatMatrix;
    #endif

    #ifdef CLEARCOAT_TEXTURE_ROUGHNESS
        uniform mat4 clearCoatRoughnessMatrix;
    #endif

    #ifdef CLEARCOAT_BUMP
        uniform vec2 vClearCoatBumpInfos;
        uniform vec2 vClearCoatTangentSpaceParams;
        uniform mat4 clearCoatBumpMatrix;
    #endif

    #ifdef CLEARCOAT_TINT
        uniform vec4 vClearCoatTintParams;
        uniform float clearCoatColorAtDistance;

        #ifdef CLEARCOAT_TINT_TEXTURE
            uniform vec2 vClearCoatTintInfos;
            uniform mat4 clearCoatTintMatrix;
        #endif
    #endif
#endif

// Anisotropy
#ifdef ANISOTROPIC
    uniform vec3 vAnisotropy;

    #ifdef ANISOTROPIC_TEXTURE
        uniform vec2 vAnisotropyInfos;
        uniform mat4 anisotropyMatrix;
    #endif
#endif

// Sheen
#ifdef SHEEN
    uniform vec4 vSheenColor;
    #ifdef SHEEN_ROUGHNESS
        uniform float vSheenRoughness;
    #endif

    #if defined(SHEEN_TEXTURE) || defined(SHEEN_TEXTURE_ROUGHNESS)
        uniform vec4 vSheenInfos;
    #endif

    #ifdef SHEEN_TEXTURE
        uniform mat4 sheenMatrix;
    #endif

    #ifdef SHEEN_TEXTURE_ROUGHNESS
        uniform mat4 sheenRoughnessMatrix;
    #endif
#endif

// SubSurface
#ifdef SUBSURFACE
    #ifdef SS_REFRACTION
        uniform vec3 vRefractionMicrosurfaceInfos;
        uniform vec4 vRefractionInfos;
        uniform mat4 refractionMatrix;
        #ifdef REALTIME_FILTERING
            uniform vec2 vRefractionFilteringInfo;
        #endif
    #endif

    #ifdef SS_THICKNESSANDMASK_TEXTURE
        uniform vec2 vThicknessInfos;
        uniform mat4 thicknessMatrix;
    #endif

    uniform vec2 vThicknessParam;
    uniform vec3 vDiffusionDistance;
    uniform vec4 vTintColor;
    uniform vec3 vSubSurfaceIntensity;
#endif

#ifdef PREPASS
    #ifdef PREPASS_IRRADIANCE
        uniform float scatteringDiffusionProfile;
    #endif
#endif

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_PBR_FRAGMENT_DECLARATION_FX_H
