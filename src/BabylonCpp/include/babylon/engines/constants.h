#ifndef BABYLON_ENGINES_CONSTANTS_H
#define BABYLON_ENGINES_CONSTANTS_H

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Defines the cross module used constants to avoid circular dependencies
 */
struct BABYLON_SHARED_EXPORT Constants {
  /** Defines that alpha blending is disabled */
  static constexpr unsigned int ALPHA_DISABLE = 0;
  /** Defines that alpha blending is SRC ALPHA * SRC + DEST */
  static constexpr unsigned int ALPHA_ADD = 1;
  /** Defines that alpha blending is SRC ALPHA * SRC + (1 - SRC ALPHA) * DEST */
  static constexpr unsigned int ALPHA_COMBINE = 2;
  /** Defines that alpha blending is DEST - SRC * DEST */
  static constexpr unsigned int ALPHA_SUBTRACT = 3;
  /** Defines that alpha blending is SRC * DEST */
  static constexpr unsigned int ALPHA_MULTIPLY = 4;
  /** Defines that alpha blending is SRC ALPHA * SRC + (1 - SRC) * DEST */
  static constexpr unsigned int ALPHA_MAXIMIZED = 5;
  /** Defines that alpha blending is SRC + DEST */
  static constexpr unsigned int ALPHA_ONEONE = 6;
  /** Defines that alpha blending is SRC + (1 - SRC ALPHA) * DEST */
  static constexpr unsigned int ALPHA_PREMULTIPLIED = 7;
  /**
   * Defines that alpha blending is SRC + (1 - SRC ALPHA) * DEST
   * Alpha will be set to (1 - SRC ALPHA) * DEST ALPHA
   */
  static constexpr unsigned int ALPHA_PREMULTIPLIED_PORTERDUFF = 8;
  /** Defines that alpha blending is CST * SRC + (1 - CST) * DEST */
  static constexpr unsigned int ALPHA_INTERPOLATE = 9;
  /**
   * Defines that alpha blending is SRC + (1 - SRC) * DEST
   * Alpha will be set to SRC ALPHA + (1 - SRC ALPHA) * DEST ALPHA
   */
  static constexpr unsigned int ALPHA_SCREENMODE = 10;
  /**
   * Defines that alpha blending is SRC + DST
   * Alpha will be set to SRC ALPHA + DST ALPHA
   */
  static constexpr unsigned int ALPHA_ONEONE_ONEONE = 11;
  /**
   * Defines that alpha blending is SRC * DST ALPHA + DST
   * Alpha will be set to 0
   */
  static constexpr unsigned int ALPHA_ALPHATOCOLOR = 12;
  /**
   * Defines that alpha blending is SRC * (1 - DST) + DST * (1 - SRC)
   */
  static constexpr unsigned int ALPHA_REVERSEONEMINUS = 13;
  /**
   * Defines that alpha blending is SRC + DST * (1 - SRC ALPHA)
   * Alpha will be set to SRC ALPHA + DST ALPHA * (1 - SRC ALPHA)
   */
  static constexpr unsigned int ALPHA_SRC_DSTONEMINUSSRCALPHA = 14;
  /**
   * Defines that alpha blending is SRC + DST
   * Alpha will be set to SRC ALPHA
   */
  static constexpr unsigned int ALPHA_ONEONE_ONEZERO = 15;
  /**
   * Defines that alpha blending is SRC * (1 - DST) + DST * (1 - SRC)
   * Alpha will be set to DST ALPHA
   */
  static constexpr unsigned int ALPHA_EXCLUSION = 16;

  /** Defines that alpha blending equation a SUM */
  static constexpr unsigned int ALPHA_EQUATION_ADD = 0;
  /** Defines that alpha blending equation a SUBSTRACTION */
  static constexpr unsigned int ALPHA_EQUATION_SUBSTRACT = 1;
  /** Defines that alpha blending equation a REVERSE SUBSTRACTION */
  static constexpr unsigned int ALPHA_EQUATION_REVERSE_SUBTRACT = 2;
  /** Defines that alpha blending equation a MAX operation */
  static constexpr unsigned int ALPHA_EQUATION_MAX = 3;
  /** Defines that alpha blending equation a MIN operation */
  static constexpr unsigned int ALPHA_EQUATION_MIN = 4;
  /**
   * Defines that alpha blending equation a DARKEN operation:
   * It takes the min of the src and sums the alpha channels.
   */
  static constexpr unsigned int ALPHA_EQUATION_DARKEN = 5;

  /** Defines that the resource is not delayed*/
  static constexpr unsigned int DELAYLOADSTATE_NONE = 0;
  /** Defines that the resource was successfully delay loaded */
  static constexpr unsigned int DELAYLOADSTATE_LOADED = 1;
  /** Defines that the resource is currently delay loading */
  static constexpr unsigned int DELAYLOADSTATE_LOADING = 2;
  /** Defines that the resource is delayed and has not started loading */
  static constexpr unsigned int DELAYLOADSTATE_NOTLOADED = 4;

  // Depth or Stencil test Constants.
  /** Passed to depthFunction or stencilFunction to specify depth or stencil tests will never pass.
   * i.e. Nothing will be drawn */
  static constexpr unsigned int NEVER = 0x0200;
  /** Passed to depthFunction or stencilFunction to specify depth or stencil tests will always pass.
   * i.e. Pixels will be drawn in the order they are drawn */
  static constexpr unsigned int ALWAYS = 0x0207;
  /** Passed to depthFunction or stencilFunction to specify depth or stencil tests will pass if the
   * new depth value is less than the stored value */
  static constexpr unsigned int LESS = 0x0201;
  /** Passed to depthFunction or stencilFunction to specify depth or stencil tests will pass if the
   * new depth value is equals to the stored value */
  static constexpr unsigned int EQUAL = 0x0202;
  /** Passed to depthFunction or stencilFunction to specify depth or stencil tests will pass if the
   * new depth value is less than or equal to the stored value */
  static constexpr unsigned int LEQUAL = 0x0203;
  /** Passed to depthFunction or stencilFunction to specify depth or stencil tests will pass if the
   * new depth value is greater than the stored value */
  static constexpr unsigned int GREATER = 0x0204;
  /** Passed to depthFunction or stencilFunction to specify depth or stencil tests will pass if the
   * new depth value is greater than or equal to the stored value */
  static constexpr unsigned int GEQUAL = 0x0206;
  /** Passed to depthFunction or stencilFunction to specify depth or stencil tests will pass if the
   * new depth value is not equal to the stored value */
  static constexpr unsigned int NOTEQUAL = 0x0205;

  // Stencil Actions Constants.
  /** Passed to stencilOperation to specify that stencil value must be kept */
  static constexpr unsigned int KEEP = 0x1E00;
  /** Passed to stencilOperation to specify that stencil value must be zero */
  static constexpr unsigned int ZERO = 0x0000;
  /** Passed to stencilOperation to specify that stencil value must be replaced */
  static constexpr unsigned int REPLACE = 0x1E01;
  /** Passed to stencilOperation to specify that stencil value must be incremented */
  static constexpr unsigned int INCR = 0x1E02;
  /** Passed to stencilOperation to specify that stencil value must be decremented */
  static constexpr unsigned int DECR = 0x1E03;
  /** Passed to stencilOperation to specify that stencil value must be inverted */
  static constexpr unsigned int INVERT = 0x150A;
  /** Passed to stencilOperation to specify that stencil value must be incremented with wrapping */
  static constexpr unsigned int INCR_WRAP = 0x8507;
  /** Passed to stencilOperation to specify that stencil value must be decremented with wrapping */
  static constexpr unsigned int DECR_WRAP = 0x8508;

  /** Texture is not repeating outside of 0..1 UVs */
  static constexpr unsigned int TEXTURE_CLAMP_ADDRESSMODE = 0;
  /** Texture is repeating outside of 0..1 UVs */
  static constexpr unsigned int TEXTURE_WRAP_ADDRESSMODE = 1;
  /** Texture is repeating and mirrored */
  static constexpr unsigned int TEXTURE_MIRROR_ADDRESSMODE = 2;

  /** ALPHA */
  static constexpr unsigned int TEXTUREFORMAT_ALPHA = 0;
  /** LUMINANCE */
  static constexpr unsigned int TEXTUREFORMAT_LUMINANCE = 1;
  /** LUMINANCE_ALPHA */
  static constexpr unsigned int TEXTUREFORMAT_LUMINANCE_ALPHA = 2;
  /** RGB */
  static constexpr unsigned int TEXTUREFORMAT_RGB = 4;
  /** RGBA */
  static constexpr unsigned int TEXTUREFORMAT_RGBA = 5;
  /** RED */
  static constexpr unsigned int TEXTUREFORMAT_RED = 6;
  /** RED (2nd reference) */
  static constexpr unsigned int TEXTUREFORMAT_R = 6;
  /** RG */
  static constexpr unsigned int TEXTUREFORMAT_RG = 7;
  /** RED_INTEGER */
  static constexpr unsigned int TEXTUREFORMAT_RED_INTEGER = 8;
  /** RED_INTEGER (2nd reference) */
  static constexpr unsigned int TEXTUREFORMAT_R_INTEGER = 8;
  /** RG_INTEGER */
  static constexpr unsigned int TEXTUREFORMAT_RG_INTEGER = 9;
  /** RGB_INTEGER */
  static constexpr unsigned int TEXTUREFORMAT_RGB_INTEGER = 10;
  /** RGBA_INTEGER */
  static constexpr unsigned int TEXTUREFORMAT_RGBA_INTEGER = 11;
  /** BGRA */
  static constexpr unsigned int TEXTUREFORMAT_BGRA = 12;

  /** Depth 24 bits + Stencil 8 bits */
  static constexpr unsigned int TEXTUREFORMAT_DEPTH24_STENCIL8 = 13;
  /** Depth 32 bits float */
  static constexpr unsigned int TEXTUREFORMAT_DEPTH32_FLOAT = 14;

  /** Compressed BC7 */
  static constexpr unsigned int TEXTUREFORMAT_COMPRESSED_RGBA_BPTC_UNORM = 36492;
  /** Compressed BC6 unsigned float */
  static constexpr unsigned int TEXTUREFORMAT_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT = 36495;
  /** Compressed BC6 signed float */
  static constexpr unsigned int TEXTUREFORMAT_COMPRESSED_RGB_BPTC_SIGNED_FLOAT = 36494;
  /** Compressed BC3 */
  static constexpr unsigned int TEXTUREFORMAT_COMPRESSED_RGBA_S3TC_DXT5 = 33779;
  /** Compressed BC2 */
  static constexpr unsigned int TEXTUREFORMAT_COMPRESSED_RGBA_S3TC_DXT3 = 33778;
  /** Compressed BC1 */
  static constexpr unsigned int TEXTUREFORMAT_COMPRESSED_RGBA_S3TC_DXT1 = 33777;

  /** UNSIGNED_BYTE */
  static constexpr unsigned int TEXTURETYPE_UNSIGNED_BYTE = 0;
  /** UNSIGNED_BYTE (2nd reference) */
  static constexpr unsigned int TEXTURETYPE_UNSIGNED_INT = 0;
  /** FLOAT */
  static constexpr unsigned int TEXTURETYPE_FLOAT = 1;
  /** HALF_FLOAT */
  static constexpr unsigned int TEXTURETYPE_HALF_FLOAT = 2;
  /** BYTE */
  static constexpr unsigned int TEXTURETYPE_BYTE = 3;
  /** SHORT */
  static constexpr unsigned int TEXTURETYPE_SHORT = 4;
  /** UNSIGNED_SHORT */
  static constexpr unsigned int TEXTURETYPE_UNSIGNED_SHORT = 5;
  /** INT */
  static constexpr unsigned int TEXTURETYPE_INT = 6;
  /** UNSIGNED_INT */
  static constexpr unsigned int TEXTURETYPE_UNSIGNED_INTEGER = 7;
  /** UNSIGNED_SHORT_4_4_4_4 */
  static constexpr unsigned int TEXTURETYPE_UNSIGNED_SHORT_4_4_4_4 = 8;
  /** UNSIGNED_SHORT_5_5_5_1 */
  static constexpr unsigned int TEXTURETYPE_UNSIGNED_SHORT_5_5_5_1 = 9;
  /** UNSIGNED_SHORT_5_6_5 */
  static constexpr unsigned int TEXTURETYPE_UNSIGNED_SHORT_5_6_5 = 10;
  /** UNSIGNED_INT_2_10_10_10_REV */
  static constexpr unsigned int TEXTURETYPE_UNSIGNED_INT_2_10_10_10_REV = 11;
  /** UNSIGNED_INT_24_8 */
  static constexpr unsigned int TEXTURETYPE_UNSIGNED_INT_24_8 = 12;
  /** UNSIGNED_INT_10F_11F_11F_REV */
  static constexpr unsigned int TEXTURETYPE_UNSIGNED_INT_10F_11F_11F_REV = 13;
  /** UNSIGNED_INT_5_9_9_9_REV */
  static constexpr unsigned int TEXTURETYPE_UNSIGNED_INT_5_9_9_9_REV = 14;
  /** FLOAT_32_UNSIGNED_INT_24_8_REV */
  static constexpr unsigned int TEXTURETYPE_FLOAT_32_UNSIGNED_INT_24_8_REV = 15;

  /** nearest is mag = nearest and min = nearest and no mip */
  static constexpr unsigned int TEXTURE_NEAREST_SAMPLINGMODE = 1;
  /** mag = nearest and min = nearest and mip = none */
  static constexpr unsigned int TEXTURE_NEAREST_NEAREST = 1;

  /** Bilinear is mag = linear and min = linear and no mip */
  static constexpr unsigned int TEXTURE_BILINEAR_SAMPLINGMODE = 2;
  /** mag = linear and min = linear and mip = none */
  static constexpr unsigned int TEXTURE_LINEAR_LINEAR = 2;

  /** Trilinear is mag = linear and min = linear and mip = linear */
  static constexpr unsigned int TEXTURE_TRILINEAR_SAMPLINGMODE = 3;
  /** Trilinear is mag = linear and min = linear and mip = linear */
  static constexpr unsigned int TEXTURE_LINEAR_LINEAR_MIPLINEAR = 3;

  /** mag = nearest and min = nearest and mip = nearest */
  static constexpr unsigned int TEXTURE_NEAREST_NEAREST_MIPNEAREST = 4;
  /** mag = nearest and min = linear and mip = nearest */
  static constexpr unsigned int TEXTURE_NEAREST_LINEAR_MIPNEAREST = 5;
  /** mag = nearest and min = linear and mip = linear */
  static constexpr unsigned int TEXTURE_NEAREST_LINEAR_MIPLINEAR = 6;
  /** mag = nearest and min = linear and mip = none */
  static constexpr unsigned int TEXTURE_NEAREST_LINEAR = 7;
  /** nearest is mag = nearest and min = nearest and mip = linear */
  static constexpr unsigned int TEXTURE_NEAREST_NEAREST_MIPLINEAR = 8;
  /** mag = linear and min = nearest and mip = nearest */
  static constexpr unsigned int TEXTURE_LINEAR_NEAREST_MIPNEAREST = 9;
  /** mag = linear and min = nearest and mip = linear */
  static constexpr unsigned int TEXTURE_LINEAR_NEAREST_MIPLINEAR = 10;
  /** Bilinear is mag = linear and min = linear and mip = nearest */
  static constexpr unsigned int TEXTURE_LINEAR_LINEAR_MIPNEAREST = 11;
  /** mag = linear and min = nearest and mip = none */
  static constexpr unsigned int TEXTURE_LINEAR_NEAREST = 12;

  /** Explicit coordinates mode */
  static constexpr unsigned int TEXTURE_EXPLICIT_MODE = 0;
  /** Spherical coordinates mode */
  static constexpr unsigned int TEXTURE_SPHERICAL_MODE = 1;
  /** Planar coordinates mode */
  static constexpr unsigned int TEXTURE_PLANAR_MODE = 2;
  /** Cubic coordinates mode */
  static constexpr unsigned int TEXTURE_CUBIC_MODE = 3;
  /** Projection coordinates mode */
  static constexpr unsigned int TEXTURE_PROJECTION_MODE = 4;
  /** Skybox coordinates mode */
  static constexpr unsigned int TEXTURE_SKYBOX_MODE = 5;
  /** Inverse Cubic coordinates mode */
  static constexpr unsigned int TEXTURE_INVCUBIC_MODE = 6;
  /** Equirectangular coordinates mode */
  static constexpr unsigned int TEXTURE_EQUIRECTANGULAR_MODE = 7;
  /** Equirectangular Fixed coordinates mode */
  static constexpr unsigned int TEXTURE_FIXED_EQUIRECTANGULAR_MODE = 8;
  /** Equirectangular Fixed Mirrored coordinates mode */
  static constexpr unsigned int TEXTURE_FIXED_EQUIRECTANGULAR_MIRRORED_MODE = 9;

  /** Offline (baking) quality for texture filtering */
  static constexpr unsigned int TEXTURE_FILTERING_QUALITY_OFFLINE = 4096;

  /** High quality for texture filtering */
  static constexpr unsigned int TEXTURE_FILTERING_QUALITY_HIGH = 64;

  /** Medium quality for texture filtering */
  static constexpr unsigned int TEXTURE_FILTERING_QUALITY_MEDIUM = 16;

  /** Low quality for texture filtering */
  static constexpr unsigned int TEXTURE_FILTERING_QUALITY_LOW = 8;

  // Texture rescaling mode
  /** Defines that texture rescaling will use a floor to find the closer power of 2 size */
  static constexpr unsigned int SCALEMODE_FLOOR = 1;
  /** Defines that texture rescaling will look for the nearest power of 2 size */
  static constexpr unsigned int SCALEMODE_NEAREST = 2;
  /** Defines that texture rescaling will use a ceil to find the closer power of 2 size */
  static constexpr unsigned int SCALEMODE_CEILING = 3;

  /**
   * The dirty texture flag value
   */
  static constexpr unsigned int MATERIAL_TextureDirtyFlag = 1;
  /**
   * The dirty light flag value
   */
  static constexpr unsigned int MATERIAL_LightDirtyFlag = 2;
  /**
   * The dirty fresnel flag value
   */
  static constexpr unsigned int MATERIAL_FresnelDirtyFlag = 4;
  /**
   * The dirty attribute flag value
   */
  static constexpr unsigned int MATERIAL_AttributesDirtyFlag = 8;
  /**
   * The dirty misc flag value
   */
  static constexpr unsigned int MATERIAL_MiscDirtyFlag = 16;
  /**
   * The dirty prepass flag value
   */
  static constexpr unsigned int MATERIAL_PrePassDirtyFlag = 32;
  /**
   * The all dirty flag value
   */
  static constexpr unsigned int MATERIAL_AllDirtyFlag = 63;

  /**
   * Returns the triangle fill mode
   */
  static constexpr unsigned int MATERIAL_TriangleFillMode = 0;
  /**
   * Returns the wireframe mode
   */
  static constexpr unsigned int MATERIAL_WireFrameFillMode = 1;
  /**
   * Returns the point fill mode
   */
  static constexpr unsigned int MATERIAL_PointFillMode = 2;
  /**
   * Returns the point list draw mode
   */
  static constexpr unsigned int MATERIAL_PointListDrawMode = 3;
  /**
   * Returns the line list draw mode
   */
  static constexpr unsigned int MATERIAL_LineListDrawMode = 4;
  /**
   * Returns the line loop draw mode
   */
  static constexpr unsigned int MATERIAL_LineLoopDrawMode = 5;
  /**
   * Returns the line strip draw mode
   */
  static constexpr unsigned int MATERIAL_LineStripDrawMode = 6;

  /**
   * Returns the triangle strip draw mode
   */
  static constexpr unsigned int MATERIAL_TriangleStripDrawMode = 7;
  /**
   * Returns the triangle fan draw mode
   */
  static constexpr unsigned int MATERIAL_TriangleFanDrawMode = 8;

  /**
   * Stores the clock-wise side orientation
   */
  static constexpr unsigned int MATERIAL_ClockWiseSideOrientation = 0;
  /**
   * Stores the counter clock-wise side orientation
   */
  static constexpr unsigned int MATERIAL_CounterClockWiseSideOrientation = 1;

  /**
   * Nothing
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int ACTION_NothingTrigger = 0;
  /**
   * On pick
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int ACTION_OnPickTrigger = 1;
  /**
   * On left pick
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int ACTION_OnLeftPickTrigger = 2;
  /**
   * On right pick
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int ACTION_OnRightPickTrigger = 3;
  /**
   * On center pick
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int ACTION_OnCenterPickTrigger = 4;
  /**
   * On pick down
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int ACTION_OnPickDownTrigger = 5;
  /**
   * On double pick
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int ACTION_OnDoublePickTrigger = 6;
  /**
   * On pick up
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int ACTION_OnPickUpTrigger = 7;
  /**
   * On pick out.
   * This trigger will only be raised if you also declared a OnPickDown
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int ACTION_OnPickOutTrigger = 16;
  /**
   * On long press
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int ACTION_OnLongPressTrigger = 8;
  /**
   * On pointer over
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int ACTION_OnPointerOverTrigger = 9;
  /**
   * On pointer out
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int ACTION_OnPointerOutTrigger = 10;
  /**
   * On every frame
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int ACTION_OnEveryFrameTrigger = 11;
  /**
   * On intersection enter
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int ACTION_OnIntersectionEnterTrigger = 12;
  /**
   * On intersection exit
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int ACTION_OnIntersectionExitTrigger = 13;
  /**
   * On key down
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int ACTION_OnKeyDownTrigger = 14;
  /**
   * On key up
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int ACTION_OnKeyUpTrigger = 15;

  /**
   * Billboard mode will only apply to Y axis
   */
  static constexpr unsigned int PARTICLES_BILLBOARDMODE_Y = 2;
  /**
   * Billboard mode will apply to all axes
   */
  static constexpr unsigned int PARTICLES_BILLBOARDMODE_ALL = 7;
  /**
   * Special billboard mode where the particle will be biilboard to the camera but rotated to align
   * with direction
   */
  static constexpr unsigned int PARTICLES_BILLBOARDMODE_STRETCHED = 8;

  /** Default culling strategy : this is an exclusion test and it's the more accurate.
   *  Test order :
   *  Is the bounding sphere outside the frustum ?
   *  If not, are the bounding box vertices outside the frustum ?
   *  It not, then the cullable object is in the frustum.
   */
  static constexpr unsigned int MESHES_CULLINGSTRATEGY_STANDARD = 0;
  /** Culling strategy : Bounding Sphere Only.
   *  This is an exclusion test. It's faster than the standard strategy because the bounding box is
   * not tested. It's also less accurate than the standard because some not visible objects can
   * still be selected. Test : is the bounding sphere outside the frustum ? If not, then the
   * cullable object is in the frustum.
   */
  static constexpr unsigned int MESHES_CULLINGSTRATEGY_BOUNDINGSPHERE_ONLY = 1;
  /** Culling strategy : Optimistic Inclusion.
   *  This in an inclusion test first, then the standard exclusion test.
   *  This can be faster when a cullable object is expected to be almost always in the camera
   * frustum. This could also be a little slower than the standard test when the tested object
   * center is not the frustum but one of its bounding box vertex is still inside. Anyway, it's as
   * accurate as the standard strategy. Test : Is the cullable object bounding sphere center in the
   * frustum ? If not, apply the default culling strategy.
   */
  static constexpr unsigned int MESHES_CULLINGSTRATEGY_OPTIMISTIC_INCLUSION = 2;
  /** Culling strategy : Optimistic Inclusion then Bounding Sphere Only.
   *  This in an inclusion test first, then the bounding sphere only exclusion test.
   *  This can be the fastest test when a cullable object is expected to be almost always in the
   * camera frustum. This could also be a little slower than the BoundingSphereOnly strategy when
   * the tested object center is not in the frustum but its bounding sphere still intersects it.
   *  It's less accurate than the standard strategy and as accurate as the BoundingSphereOnly
   * strategy. Test : Is the cullable object bounding sphere center in the frustum ? If not, apply
   * the Bounding Sphere Only strategy. No Bounding Box is tested here.
   */
  static constexpr unsigned int MESHES_CULLINGSTRATEGY_OPTIMISTIC_INCLUSION_THEN_BSPHERE_ONLY = 3;

  /**
   * No logging while loading
   */
  static constexpr unsigned int SCENELOADER_NO_LOGGING = 0;
  /**
   * Minimal logging while loading
   */
  static constexpr unsigned int SCENELOADER_MINIMAL_LOGGING = 1;
  /**
   * Summary logging while loading
   */
  static constexpr unsigned int SCENELOADER_SUMMARY_LOGGING = 2;
  /**
   * Detailed logging while loading
   */
  static constexpr unsigned int SCENELOADER_DETAILED_LOGGING = 3;

  /**
   * Constant used to retrieve the irradiance texture index in the textures array in the prepass
   * using getIndex(Constants.PREPASS_IRRADIANCE_TEXTURE_TYPE)
   */
  static constexpr unsigned int PREPASS_IRRADIANCE_TEXTURE_TYPE = 0;
  /**
   * Constant used to retrieve the position texture index in the textures array in the prepass
   * using getIndex(Constants.PREPASS_POSITION_TEXTURE_INDEX)
   */
  static constexpr unsigned int PREPASS_POSITION_TEXTURE_TYPE = 1;
  /**
   * Constant used to retrieve the velocity texture index in the textures array in the prepass
   * using getIndex(Constants.PREPASS_VELOCITY_TEXTURE_INDEX)
   */
  static constexpr unsigned int PREPASS_VELOCITY_TEXTURE_TYPE = 2;
  /**
   * Constant used to retrieve the reflectivity texture index in the textures array in the prepass
   * using the getIndex(Constants.PREPASS_REFLECTIVITY_TEXTURE_TYPE)
   */
  static constexpr unsigned int PREPASS_REFLECTIVITY_TEXTURE_TYPE = 3;
  /**
   * Constant used to retrieve the lit color texture index in the textures array in the prepass
   * using the getIndex(Constants.PREPASS_COLOR_TEXTURE_TYPE)
   */
  static constexpr unsigned int PREPASS_COLOR_TEXTURE_TYPE = 4;
  /**
   * Constant used to retrieve depth index in the textures array in the prepass
   * using the getIndex(Constants.PREPASS_DEPTH_TEXTURE_TYPE)
   */
  static constexpr unsigned int PREPASS_DEPTH_TEXTURE_TYPE = 5;
  /**
   * Constant used to retrieve normal index in the textures array in the prepass
   * using the getIndex(Constants.PREPASS_NORMAL_TEXTURE_TYPE)
   */
  static constexpr unsigned int PREPASS_NORMAL_TEXTURE_TYPE = 6;
  /**
   * Constant used to retrieve albedo index in the textures array in the prepass
   * using the getIndex(Constants.PREPASS_ALBEDO_TEXTURE_TYPE)
   */
  static constexpr unsigned int PREPASS_ALBEDO_TEXTURE_TYPE = 7;

  /**
   * Prefixes used by the engine for custom effects
   */
  static constexpr const char* CUSTOMEFFECT_PREFIX_SHADOWGENERATOR = "bjs_shadowgenerator_";
}; // end of struct Constants

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_CONSTANTS_H
