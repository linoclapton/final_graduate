uniform vec2 screenDim_;
uniform vec2 screenDimRCP_;

/*
 * This struct contains information about a texture, like
 * its dimensions. Additionally, the reciprocal
 * values of all parameters are available (suffix RCP) .
 */
struct TEXTURE_PARAMETERS {
    vec2 dimensions_;        // the texture's resolution, e.g. [256.0, 128.0]
    vec2 dimensionsRCP_;
    mat4 matrix_;            // texture coordinate transformation
};

#define SAMPLER2D_TYPE sampler2D

// Texture lookup function for 2D textures,
// expecting texture coordinates as pixel coordinates, i.e, [(0,0) , textureSize].
vec4 textureLookup2D(in sampler2D myTexture, in TEXTURE_PARAMETERS texParams, in vec2 texCoords) {
    vec2 texCoordsNormalized = texCoords * texParams.dimensionsRCP_;
    vec2 texCoordsTransformed = (texParams.matrix_ * vec4(texCoordsNormalized, 0.0, 1.0)).xy;
#if defined(GLSL_VERSION_130)
    return texture(myTexture, texCoordsTransformed);
#else
    return texture2D(myTexture, texCoordsTransformed);
#endif
}

// Texture lookup function for 2D textures,
// expecting normalized texture coordinates, i.e., [0,1].
vec4 textureLookup2Dnormalized(in sampler2D myTexture, in TEXTURE_PARAMETERS texParams, in vec2 texCoords) {
    vec2 texCoordsTransformed = (texParams.matrix_ * vec4(texCoords, 0.0, 1.0)).xy;
#if defined(GLSL_VERSION_130)
    return texture(myTexture, texCoordsTransformed);
#else
    return texture2D(myTexture, texCoordsTransformed);
#endif
}

// Standard texture lookup function for RenderPort images.
// Texture coordinates are expected in fragment coordinates, i.e, [(0,0) , viewportSize].
//
// @note This function may only be used, if the processor's input and output images (RenderPorts)
//       have the same dimensions.
//
vec4 textureLookup2Dscreen(in SAMPLER2D_TYPE texture, in TEXTURE_PARAMETERS texParams, in vec2 texCoords) {
    vec2 texCoordsNormalized = texCoords * screenDimRCP_;
    return textureLookup2Dnormalized(texture, texParams, texCoordsNormalized);
}


 #define MOD_SAMPLER3D

/*
 * This struct contains a volume and information about it, as
 * its dimensions and spacing. Additionally, the reciprocal
 * values of all parameters are available (suffix RCP) .
 * The values are set automatically by
 * VolumeRenderer::bindVolumes() if necessary.
 */
struct VOLUME_STRUCT {
    sampler3D volume_;              // the actual dataset

    vec3 datasetDimensions_;        // the dataset's resolution, e.g. [ 256.0, 128.0, 128.0]
    vec3 datasetDimensionsRCP_;

    vec3 datasetSpacing_;           // set dataset's voxel size, e.g. [ 2.0, 0.5, 1.0]
    vec3 datasetSpacingRCP_;

    vec3 volumeCubeSize_;          // the volume's size in physical coordinates, e.g. [ 1.0, 0.5, 0.5]
    vec3 volumeCubeSizeRCP_;

    vec3 volumeOffset_;              // see VolumeHandle::getOffset()
    vec3 volumeTextureTranslation_;  // translation for scaling of texture coordinates when performing slicing
    
    vec3 volumeBorderOffset_;    // the offset to add to texCoords based on border of faces connected to [0.0,0.0,0.0]
    vec3 volumeBorderScaling_;  // the scaling to multiply to texCoords based on border of faces connected to [1.0,1.0,1.0] 
    bool volumeHasBorder_;      // true if volume has border

    int bitDepth_;                  // the volume's bit depth
    float bitDepthScale_;           // scaling factor that must be applied for normalizing the fetched texture value.
                                    // currently just used for 12 bit volumes, which actually use only 12 out of 16 bits.

    float rwmScale_;                // RealWorldMapping slope
    float rwmOffset_;               // RealWorldMapping intercept 

    int numChannels_;

    mat4 volumeTransformation_;     // dataset transformation matrix (see Volume)
    mat4 volumeTransformationINV_;  // inverse dataset transformation matrix

    vec3 cameraPositionOBJ_;        // camera position in volume object coordinates (see mod_shading.frag)
    vec3 lightPositionOBJ_;         // light position in volume object coordinates (see mod_shading.frag)
};

/*
 * Function to apply correct texture coordinates when for instance using borders
 */
vec3 getCorrectTextureCoordinate(VOLUME_STRUCT volumeStruct, vec3 texCoords){
/*
 * IGNORE_VOLUME_STRUCT_UNIFORMS_WHEN_SAMPLING is a needed define for the MultiVolumeRaycaster as the 
 * VOLUME_STRUCT uniforms are not valid to use in the mod_sampler3D shader, when multiple volumes are attached.
 * This is a problem only on Nvidia Driver version larger than 295.73, tested to 296.10.
*/
#if defined(IGNORE_VOLUME_STRUCT_UNIFORMS_WHEN_SAMPLING)
    return texCoords;
#else
    return volumeStruct.volumeBorderOffset_ + texCoords*volumeStruct.volumeBorderScaling_;
#endif
}

/*
 * Function for volume texture lookup. In addition to the texture coordinates
 * the corresponding VOLUME_STRUCT has to be passed.
 * Before returning the fetched value it is normalized to the interval [0,1], in order to deal
 * with 12 bit data sets.
 */
vec4 textureLookup3D(VOLUME_STRUCT volumeStruct, vec3 texCoords) {
    vec4 result;
#if defined(GLSL_VERSION_130)
    result = texture(volumeStruct.volume_, getCorrectTextureCoordinate(volumeStruct, texCoords));
#else
    result = texture3D(volumeStruct.volume_, getCorrectTextureCoordinate(volumeStruct, texCoords));
#endif
    result *= volumeStruct.bitDepthScale_;
    result.a *= volumeStruct.rwmScale_;
    result.a += volumeStruct.rwmOffset_;
    return result;
}

/*
 * Function for volume texture lookup. In addition to the texture coordinates
 * the corresponding VOLUME_STRUCT has to be passed .
 * In contrast to textureLookup3D() this function does not normalize the intensity values,
 * in order to deal with 12 bit data sets.
 */
vec4 textureLookup3DUnnormalized(VOLUME_STRUCT volumeStruct, vec3 texCoords) {
#if defined(GLSL_VERSION_130)
    return texture(volumeStruct.volume_, getCorrectTextureCoordinate(volumeStruct, texCoords));
#else
    return texture3D(volumeStruct.volume_, getCorrectTextureCoordinate(volumeStruct, texCoords));
#endif
}

/*
 * This function should be called by all raycasters in order to get the intensity from the volume.
 * In cases where volumeStruct indicates, that gradients are stored in the volume, these are
 * also fetched from the volume. Therefore, in addition to the volume and the texture coordinates
 * the corresponding VOLUME_STRUCT has to be passed.
 * Before returning the intensity value it is normalized to the interval [0,1], in order to deal
 * with 12 bit data sets.
 *
 * @return The result vec4 stores the intensity in the a channel and the gradient (if available)
 *         in the rgb channels.
 *
 */
vec4 getVoxel(VOLUME_STRUCT volumeStruct, vec3 sampleOrg) {
    return textureLookup3D(volumeStruct, sampleOrg);
}

/*
 * This function should be called by all raycasters in order to get the intensity from the volume.
 * In cases where volumeStruct indicates, that gradients are stored in the volume, these are
 * also fetched from the volume. Therefore, in addition to the volume and the texture coordinates
 * the corresponding VOLUME_STRUCT has to be passed.
 * In contrast to getVoxel() this function does not normalize the intensity values, in order to deal
 * with 12 bit data sets.
 *
 * @return The result vec4 stores the intensity in the a channel and the gradient (if available)
 *         in the rgb channels.
 */
vec4 getVoxelUnnormalized(VOLUME_STRUCT volumeStruct, vec3 sampleOrg) {
    return textureLookup3DUnnormalized(volumeStruct, sampleOrg);
}

vec3 texToPhysical(vec3 samplePos, VOLUME_STRUCT volumeParams) {
    return ((samplePos*volumeParams.datasetDimensions_)*volumeParams.datasetSpacing_)+volumeParams.volumeOffset_;
}

vec3 physicalToTex(vec3 samplePos, VOLUME_STRUCT volumeStruct) {
    return ((samplePos - volumeStruct.volumeOffset_) * volumeStruct.datasetSpacingRCP_) * volumeStruct.datasetDimensionsRCP_;
}

vec3 worldToTex(vec3 worldSamplePos, VOLUME_STRUCT volumeStruct) {
    //From world space to physical space:
    vec3 samplePos = (volumeStruct.volumeTransformationINV_ * vec4(worldSamplePos, 1.0)).xyz;

    return physicalToTex(samplePos, volumeStruct);
}

vec3 texToWorld(vec3 texSamplePos, VOLUME_STRUCT volumeStruct) {
    vec3 samplePos = texToPhysical(texSamplePos, volumeStruct);

    //From physical space to world space:
    return (volumeStruct.volumeTransformation_ * vec4(samplePos, 1.0)).xyz;
}

bool inUnitCube(vec3 sample_val) {
    if(sample_val.x > 1.0 || sample_val.x < 0.0 || sample_val.y > 1.0 || sample_val.y < 0.0 || sample_val.z > 1.0 || sample_val.z < 0.0)
        return false;
    return true;
}

bool onBorder(vec3 samplePos, VOLUME_STRUCT volumeStruct) {
    if(volumeStruct.volumeHasBorder_){
        if(samplePos.x == 1.0 || samplePos.x == 0.0 || samplePos.y == 1.0 || samplePos.y == 0.0 || samplePos.z == 1.0 || samplePos.z == 0.0){
            vec3 texCoord = getCorrectTextureCoordinate(volumeStruct, samplePos);
            if(texCoord.x == 1.0 || texCoord.x == 0.0 || texCoord.y == 1.0 || texCoord.y == 0.0 || texCoord.z == 1.0 || texCoord.z == 0.0)
                return true;
        }
    }
    return false;
}

float fetchCurvature(in VOLUME_STRUCT curvatureVolumeParameters, in vec3 samplePos) {
    float curvature = getVoxel(curvatureVolumeParameters, samplePos).a;
    curvature -= 0.5;
    curvature *= 2.0;
    return curvature;
}

mat3 computeHessian(in VOLUME_STRUCT gradientVolumeParameters, in vec3 samplePos) {
    vec3 offset = gradientVolumeParameters.datasetDimensionsRCP_;
    vec3 gradientr = textureLookup3DUnnormalized(gradientVolumeParameters, samplePos + vec3(offset.x,0.0,0.0)).xyz;
    vec3 gradientl = textureLookup3DUnnormalized(gradientVolumeParameters, samplePos + vec3(-offset.x,0.0,0.0)).xyz;
    vec3 gradientu = textureLookup3DUnnormalized(gradientVolumeParameters, samplePos + vec3(0.0,offset.y,0.0)).xyz;
    vec3 gradientd = textureLookup3DUnnormalized(gradientVolumeParameters, samplePos + vec3(0.0,-offset.y,0.0)).xyz;
    vec3 gradientf = textureLookup3DUnnormalized(gradientVolumeParameters, samplePos + vec3(0.0,0.0,offset.z)).xyz;
    vec3 gradientb = textureLookup3DUnnormalized(gradientVolumeParameters, samplePos + vec3(0.0,0.0,-offset.z)).xyz;
    mat3 H;
    H[0][0] = gradientl.x - gradientr.x;
    H[0][1] = gradientl.y - gradientr.y;
    H[0][2] = gradientl.z - gradientr.z;
    H[1][0] = gradientl.y - gradientr.y;
    H[1][1] = gradientd.y - gradientu.y;
    H[1][2] = gradientd.z - gradientu.z;
    H[2][0] = gradientl.z - gradientr.z;
    H[2][1] = gradientd.z - gradientu.z;
    H[2][2] = gradientb.z - gradientf.z;
    return H;
}

vec2 computeCurvature(in VOLUME_STRUCT gradientVolumeParameters, in vec3 samplePos) {
    mat3 H = computeHessian(gradientVolumeParameters, samplePos);
    vec3 gradient = (textureLookup3DUnnormalized(gradientVolumeParameters, samplePos).xyz-vec3(0.5))*2.0;

    vec3 n = -gradient / length(gradient);
    mat3 nnT;
    nnT[0][0] = n[0]*n[0];
    nnT[0][1] = n[0]*n[1];
    nnT[0][2] = n[0]*n[2];
    nnT[1][0] = n[1]*n[0];
    nnT[1][1] = n[1]*n[1];
    nnT[1][2] = n[1]*n[2];
    nnT[2][0] = n[2]*n[0];
    nnT[2][1] = n[2]*n[1];
    nnT[2][2] = n[2]*n[2];
    mat3 P = mat3(1.0) - nnT;

    mat3 G = -P*H*P / length(gradient);

    // compute trace of G
    float trace = G[0][0] + G[1][1] + G[2][2];

    // compute Frobenius norm of G
    float F = 0.0;
    F += pow(abs(G[0][0]), 2.0);
    F += pow(abs(G[0][1]), 2.0);
    F += pow(abs(G[0][2]), 2.0);
    F += pow(abs(G[1][0]), 2.0);
    F += pow(abs(G[1][1]), 2.0);
    F += pow(abs(G[1][2]), 2.0);
    F += pow(abs(G[2][0]), 2.0);
    F += pow(abs(G[2][1]), 2.0);
    F += pow(abs(G[2][2]), 2.0);
    F = sqrt(F);

    vec2 curvature;
    curvature.x = (trace + sqrt(2.0 * (F*F) - (trace*trace))) / 2.0; // kappa1
    curvature.y = (trace - sqrt(2.0 * (F*F) - (trace*trace))) / 2.0; // kappa2
    return curvature;
}

float computeViewCurvature(in VOLUME_STRUCT gradientVolumeParameters, in vec3 samplePos, in vec3 v, in float T) {
    mat3 H = computeHessian(gradientVolumeParameters, samplePos);
    vec3 gradient = (textureLookup3DUnnormalized(gradientVolumeParameters, samplePos).xyz-vec3(0.5))*2.0;

    vec3 n = -gradient / length(gradient);
    mat3 nnT;
    nnT[0][0] = n[0]*n[0];
    nnT[0][1] = n[0]*n[1];
    nnT[0][2] = n[0]*n[2];
    nnT[1][0] = n[1]*n[0];
    nnT[1][1] = n[1]*n[1];
    nnT[1][2] = n[1]*n[2];
    nnT[2][0] = n[2]*n[0];
    nnT[2][1] = n[2]*n[1];
    nnT[2][2] = n[2]*n[2];
    mat3 P = mat3(1.0) - nnT;

    mat3 G = -P*H*P / length(gradient);

    float kv = dot(v, G * v) / dot(v, P * v);
    float result = sqrt(T * kv * (2.0 - (T * kv)));
    return result;
}


/**
 * This module contains all functions which are used for the raysetup
 * as well as the ray traversal within a raycaster.
 * RC_BEGIN_LOOP and RC_END_LOOP are used within the raycaster fragment
 * shaders to control the ray traversal.
 */

uniform float samplingStepSize_;
uniform float samplingRate_;
uniform float isoValue_;

/**
 * This parameter defines the minimum opacity saturation
 * a ray has to reach before it is terminated.
 * Setting this value to 1.0 disables early ray termination.
 */
#define EARLY_RAY_TERMINATION_OPACITY 1.0

/***
 * Calculates the direction of the ray and returns the number
 * of steps and the direction.
 ***/
void raySetup(in vec3 first, in vec3 last, in vec3 dimension, out vec3 rayDirection, out float tIncr, out float tEnd) {
    // calculate ray parameters
    rayDirection = last - first;
    tEnd = length(rayDirection);
    rayDirection = normalize(rayDirection);
    tIncr = 1.0/(samplingRate_*length(rayDirection*dimension));
    #ifdef ADAPTIVE_SAMPLING
        directionWithStepSize = rayDirection * tIncr;
    #endif
}

/***
 * Applies early ray termination. The current opacity is compared to
 * the maximum opacity. In case it is greater, the opacity is set to
 * 1.0 and true is returned, otherwise false is returned.
 ***/
bool earlyRayTermination(inout float opacity, in float maxOpacity) {
    if (opacity >= maxOpacity) {
        opacity = 1.0;
        return true;
    } else {
        return false;
    }
}

#if defined(VRN_OS_APPLE) && defined(VRN_VENDOR_ATI)
  // We do manual inlining in order to deal with older ATI boards on Mac where these function
  // calls seem to drastically reduce rendering speed (triggering fallback to software mode).
  #define RC_EARLY_RAY_TERMINATION(opacity, maxOpacity, finished) \
    if (opacity >= maxOpacity) {                                  \
        opacity = 1.0;                                            \
        finished = true;                                          \
    }
#else
  // Just wrap the usual functions
  #define RC_EARLY_RAY_TERMINATION(opacity, maxOpacity, finished) \
    finished = earlyRayTermination(opacity, maxOpacity)
  #define WRITE_DEPTH_VALUE(t, tEnd, entryPointsDepth, entryParameters, exitPointsDepth, exitParameters) \
    gl_FragDepth = getDepthValue(t, tEnd, entryPointsDepth, entryParameters, exitPointsDepth, exitParameters);
#endif


// Does the hardwarr support a shader program length that allows us to use a single loop or do
// we need two nested loops?
#ifdef VRN_MAX_PROGRAM_LOOP_COUNT
    // - ATI preprocessor doesn't support shortcut evaluation)
    // - Mac/GeForce 9400M reports integer 'constant overflow'
    #if (VRN_MAX_PROGRAM_LOOP_COUNT >= 256*256) && !defined(VRN_OS_APPLE)
        #define USE_SINGLE_LOOP
    #endif
#endif

#ifdef USE_SINGLE_LOOP
    // Nvidia seems to support a loop count > 256 only for
    // for-loops and not for while-loops on Geforce 8800 GTX.
    // On GTX280 a for-loop is still slightly faster than a while-loop. joerg
    #if defined(VRN_MAX_PROGRAM_LOOP_COUNT)
        #define RAYCASTING_LOOP_COUNT VRN_MAX_PROGRAM_LOOP_COUNT
    #else
        #define RAYCASTING_LOOP_COUNT 255*255
    #endif
    #define RC_BEGIN_LOOP_FOR                                              \
      for (int loop=0; !finished && loop<RAYCASTING_LOOP_COUNT; loop++) {
    #define RC_END_LOOP_BRACES }
#else
    // Use two nested loops, should be supported everywhere
    #define RC_BEGIN_LOOP_FOR                                   \
      for (int loop0=0; !finished && loop0<255; loop0++) {      \
          for (int loop1=0; !finished && loop1<255; loop1++) {
    #define RC_END_LOOP_BRACES } }
#endif

/***
 * The beginning of a typical raycasting loop.
 */
#define RC_BEGIN_LOOP                                         \
    float tDepth = -1.0;                                      \
    bool finished = false;                                    \
    RC_BEGIN_LOOP_FOR

/***
 * The end of a typical raycasting loop. If adaptive sampling
 * is used for rendering bricked volumes, t is increased by a
 * multiple of tIncr, thereby skipping several samples.
 */
#ifdef ADAPTIVE_SAMPLING
#define RC_END_LOOP(result)                                        \
            RC_EARLY_RAY_TERMINATION(result.a, EARLY_RAY_TERMINATION_OPACITY, finished);    \
            t += (tIncr * float(numberOfSkippedSamples));          \
            finished = finished || (t > tEnd);                     \
    RC_END_LOOP_BRACES                                             \
    WRITE_DEPTH_VALUE(tDepth, tEnd, entryPointsDepth_, entryParameters_, exitPointsDepth_, exitParameters_);
#else
#define RC_END_LOOP(result)                                        \
            RC_EARLY_RAY_TERMINATION(result.a, EARLY_RAY_TERMINATION_OPACITY, finished);    \
            t += tIncr;                                            \
            finished = finished || (t > tEnd);                     \
    RC_END_LOOP_BRACES                                             \
    WRITE_DEPTH_VALUE(tDepth, tEnd, entryPointsDepth_, entryParameters_, exitPointsDepth_, exitParameters_);

#endif

/**
* In order to keep the shaders as free as possible from dealing
* with bricking and adaptive sampling, these defines can be placed
* before and after the compositing function calls to enable adaptive
* sampling when bricking is used. For normal datasets these defines
* will have no impact at all.
*/
#ifdef ADAPTIVE_SAMPLING
#define RC_BEGIN_COMPOSITING \
    for (int i=0; i<numberOfSkippedSamples; i++) {
#else
    #define RC_BEGIN_COMPOSITING
#endif

#ifdef ADAPTIVE_SAMPLING
#define RC_END_COMPOSITING \
    }
#else

/**
 * This module contains all functions which can be used for performing
 * classifications of a voxel within a raycaster.
 * The functions below are referenced by RC_APPLY_CLASSIFICATION which
 * is used in the raycaster fragment shaders.
 */
 
#ifndef DEPRECATED_TRANSFUNC_ONLY

struct TransFunc1D {
    sampler1D texture_;

    float domainLower_;
    float domainUpper_;
};

struct TransFunc2D {
    sampler2D texture_;

    vec2 domainLower_;
    vec2 domainUpper_;
};

float realWorldToTexture(TransFunc1D tf, float v) {
    if(v <= tf.domainLower_)
       return 0.0;
    else if(v >= tf.domainUpper_)
       return 1.0; 
    else
        return (v - tf.domainLower_) / (tf.domainUpper_ - tf.domainLower_);
}

vec4 applyTF(TransFunc1D transfunc, float intensity) {
    intensity = realWorldToTexture(transfunc, intensity);
    #if defined(GLSL_VERSION_130)
        return texture(transfunc.texture_, intensity);
    #else
        return texture1D(transfunc.texture_, intensity);
    #endif
}

vec4 applyTF(TransFunc1D transfunc, vec4 intensity) {
    intensity.a = realWorldToTexture(transfunc, intensity.a);
    #if defined(GLSL_VERSION_130)
        return texture(transfunc.texture_, intensity.a);
    #else
        return texture1D(transfunc.texture_, intensity.a);
    #endif
}

vec4 applyTF(TransFunc2D transfunc, float intensity, float gradientMagnitude) {
    #if defined(GLSL_VERSION_130)
        return texture(transfunc.texture_, vec2(intensity, gradientMagnitude));
    #else
        return texture2D(transfunc.texture_, vec2(intensity, gradientMagnitude));
    #endif
}

vec4 applyTF(TransFunc2D transfunc, vec4 intensityGradient) {
    #if defined(GLSL_VERSION_130)
        return texture(transfunc.texture_, vec2(intensityGradient.a, length(intensityGradient.rgb)));
    #else
        return texture2D(transfunc.texture_, vec2(intensityGradient.a, length(intensityGradient.rgb)));
    #endif
}

#endif

// Deprecated:

#if defined(TF_SAMPLER_TYPE)
    uniform TF_SAMPLER_TYPE transferFunc_;
#endif

vec4 applyTF(sampler1D transfunc, float intensity) {
    #if defined(GLSL_VERSION_130)
        return texture(transfunc, intensity);
    #else
        return texture1D(transfunc, intensity);
    #endif
}

vec4 applyTF(sampler1D transfunc, vec4 intensity) {
    #if defined(GLSL_VERSION_130)
        return texture(transfunc, intensity.a);
    #else
        return texture1D(transfunc, intensity.a);
    #endif
}

vec4 applyTF(sampler2D transfunc, float intensity, float gradientMagnitude) {
    #if defined(GLSL_VERSION_130)
        return texture(transfunc, vec2(intensity, gradientMagnitude));
    #else
        return texture2D(transfunc, vec2(intensity, gradientMagnitude));
    #endif
}

vec4 applyTF(sampler2D transfunc, vec4 intensityGradient) {
    #if defined(GLSL_VERSION_130)
        return texture(transfunc, vec2(intensityGradient.a, length(intensityGradient.rgb)));
    #else
        return texture2D(transfunc, vec2(intensityGradient.a, length(intensityGradient.rgb)));
    #endif
}



#define RC_END_COMPOSITING
#endif

/**
 * This module contains all functions which can be used for calculating
 * gradients on the fly within a raycaster.
 * The functions below are referenced by RC_CALC_GRADIENTS which is
 * used in the raycaster fragment shaders.
 */

/**
 * Calculates a correct gradient for clipping and volume boundaries.
 *
 * @param samplePos the sample's position in texture space
 * @param rayDirection the ray direction
 */
vec3 fixClipBorderGradient(vec3 samplePos, vec3 rayDirection, SAMPLER2D_TYPE entryPoints, TEXTURE_PARAMETERS entryParameters) {
    vec3 v0 = normalize(textureLookup2D(entryPoints, entryParameters, vec2(gl_FragCoord.x+2.0, gl_FragCoord.y) ).rgb - samplePos);
    vec3 v1 = normalize(textureLookup2D(entryPoints, entryParameters, vec2(gl_FragCoord.x, gl_FragCoord.y+2.0) ).rgb - samplePos);
    //FIXME: error handling if v0 or v1 is (0,0,0)
    vec3 gradient = cross(v0, v1);
    gradient = faceforward(gradient, rayDirection, gradient);
    return gradient;
}

/**
 * Calculates a voxel's gradient in volume object space based on the red
 * channel using forward differences.
 *
 * @param volume the voxel's volume
 * @param volumeStruct additional information about the passed volume
 * @param samplePos the sample's position in texture space
 * @param t the ray parameter, needed to fix gradients on clipping and
 *          volume borders
 * @param rayDirection the ray direction
 */
vec3 calcGradientRFD(VOLUME_STRUCT volumeStruct, vec3 samplePos, float t, vec3 rayDirection, SAMPLER2D_TYPE entryPoints, TEXTURE_PARAMETERS entryParameters) {
    vec3 gradient;
    if (t == 0.0 && !onBorder(samplePos, volumeStruct))
        gradient = fixClipBorderGradient(samplePos, rayDirection, entryPoints, entryParameters);
    else {
        vec3 offset = volumeStruct.datasetDimensionsRCP_;
        float v = textureLookup3DUnnormalized(volumeStruct, samplePos).r;
        float v0 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(offset.x, 0.0, 0.0)).r;
        float v1 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(0, offset.y, 0)).r;
        float v2 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(0, 0, offset.z)).r;
        gradient = vec3(v - v0, v - v1, v - v2);
        gradient *= volumeStruct.datasetSpacingRCP_;
        gradient *= volumeStruct.bitDepthScale_;
    }
    return gradient;
}

/**
 * Calculates a voxel's gradient in volume object space based on the green
 * channel using forward differences.
 *
 * @param volume the voxel's volume
 * @param volumeStruct additional information about the passed volume
 * @param samplePos the sample's position in texture space
 * @param t the ray parameter, needed to fix gradients on clipping and
 *          volume borders
 * @param rayDirection the ray direction
 */
vec3 calcGradientGFD(VOLUME_STRUCT volumeStruct, vec3 samplePos, float t, vec3 rayDirection, SAMPLER2D_TYPE entryPoints, TEXTURE_PARAMETERS entryParameters) {
    vec3 gradient;
    if (t == 0.0 && !onBorder(samplePos, volumeStruct))
        gradient = fixClipBorderGradient(samplePos, rayDirection, entryPoints, entryParameters);
    else {
        vec3 offset = volumeStruct.datasetDimensionsRCP_;
        float v = textureLookup3DUnnormalized(volumeStruct, samplePos).g;
        float v0 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(offset.x, 0.0, 0.0)).g;
        float v1 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(0, offset.y, 0)).g;
        float v2 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(0, 0, offset.z)).g;
        gradient = vec3(v - v0, v - v1, v - v2);
        gradient *= volumeStruct.datasetSpacingRCP_;
        gradient *= volumeStruct.bitDepthScale_;
    }
    return gradient;
}

/**
 * Calculates a voxel's gradient in volume object space based on the blue
 * channel using forward differences.
 *
 * @param volume the voxel's volume
 * @param volumeStruct additional information about the passed volume
 * @param samplePos the sample's position in texture space
 * @param t the ray parameter, needed to fix gradients on clipping and
 *          volume borders
 * @param rayDirection the ray direction
 */
vec3 calcGradientBFD(VOLUME_STRUCT volumeStruct, vec3 samplePos, float t, vec3 rayDirection, SAMPLER2D_TYPE entryPoints, TEXTURE_PARAMETERS entryParameters) {
    vec3 gradient;
    if (t == 0.0 && !onBorder(samplePos, volumeStruct))
        gradient = fixClipBorderGradient(samplePos, rayDirection, entryPoints, entryParameters);
    else {
        vec3 offset = volumeStruct.datasetDimensionsRCP_;
        float v = textureLookup3DUnnormalized(volumeStruct, samplePos).b;
        float v0 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(offset.x, 0.0, 0.0)).b;
        float v1 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(0, offset.y, 0)).b;
        float v2 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(0, 0, offset.z)).b;
        gradient = vec3(v - v0, v - v1, v - v2);
        gradient *= volumeStruct.datasetSpacingRCP_;
        gradient *= volumeStruct.bitDepthScale_;
    }
    return gradient;
}

/**
 * Calculates a voxel's gradient in volume object space based on the alpha
 * channel using forward differences.
 *
 * @param volume the voxel's volume
 * @param volumeStruct additional information about the passed volume
 * @param samplePos the sample's position in texture space
 * @param t the ray parameter, needed to fix gradients on clipping and
 *          volume borders
 * @param rayDirection the ray direction
 */
vec3 calcGradientAFD(VOLUME_STRUCT volumeStruct, vec3 samplePos, float t, vec3 rayDirection, SAMPLER2D_TYPE entryPoints, TEXTURE_PARAMETERS entryParameters) {
    vec3 gradient;
    if (t == 0.0 && !onBorder(samplePos, volumeStruct))
        gradient = fixClipBorderGradient(samplePos, rayDirection, entryPoints, entryParameters);
    else {
        vec3 offset = volumeStruct.datasetDimensionsRCP_;
        float v = textureLookup3DUnnormalized(volumeStruct, samplePos).a;
        float v0 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(offset.x, 0.0, 0.0)).a;
        float v1 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(0, offset.y, 0)).a;
        float v2 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(0, 0, offset.z)).a;
        gradient = vec3(v - v0, v - v1, v - v2);
        gradient *= volumeStruct.datasetSpacingRCP_;
        gradient *= volumeStruct.bitDepthScale_;
    }
    return gradient;
}

/**
 * Calculates a voxel's gradient in volume object space based on the red
 * channel using central differences.
 *
 * @param volume the voxel's volume
 * @param volumeStruct additional information about the passed volume
 * @param samplePos the sample's position in texture space
 * @param t the ray parameter, needed to fix gradients on clipping and
 *          volume borders
 * @param rayDirection the ray direction
 */
vec3 calcGradientR(VOLUME_STRUCT volumeStruct, vec3 samplePos, float t, vec3 rayDirection, SAMPLER2D_TYPE entryPoints, TEXTURE_PARAMETERS entryParameters) {
    vec3 gradient;
    if (t == 0.0 && !onBorder(samplePos, volumeStruct))
        gradient = fixClipBorderGradient(samplePos, rayDirection, entryPoints, entryParameters);
    else {
        vec3 offset = volumeStruct.datasetDimensionsRCP_;
        float v0 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(offset.x, 0.0, 0.0)).r;
        float v1 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(0, offset.y, 0)).r;
        float v2 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(0, 0, offset.z)).r;
        float v3 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(-offset.x, 0, 0)).r;
        float v4 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(0, -offset.y, 0)).r;
        float v5 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(0, 0, -offset.z)).r;
        gradient = vec3(v3 - v0, v4 - v1, v5 - v2);
        gradient *= (volumeStruct.datasetSpacingRCP_ * 0.5);
        gradient *= volumeStruct.bitDepthScale_;
    }
    return gradient;
}

/**
 * Calculates a voxel's gradient in volume object space based on the green
 * channel using central differences.
 *
 * @param volume the voxel's volume
 * @param volumeStruct additional information about the passed volume
 * @param samplePos the sample's position in texture space
 * @param t the ray parameter, needed to fix gradients on clipping and
 *          volume borders
 * @param rayDirection the ray direction
 */
vec3 calcGradientG(VOLUME_STRUCT volumeStruct, vec3 samplePos, float t, vec3 rayDirection, SAMPLER2D_TYPE entryPoints, TEXTURE_PARAMETERS entryParameters) {
    vec3 gradient;
    if (t == 0.0 && !onBorder(samplePos, volumeStruct))
        gradient = fixClipBorderGradient(samplePos, rayDirection, entryPoints, entryParameters);
    else {
        vec3 offset = volumeStruct.datasetDimensionsRCP_;
        float v0 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(offset.x, 0.0, 0.0)).g;
        float v1 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(0, offset.y, 0)).g;
        float v2 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(0, 0, offset.z)).g;
        float v3 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(-offset.x, 0, 0)).g;
        float v4 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(0, -offset.y, 0)).g;
        float v5 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(0, 0, -offset.z)).g;
        gradient = vec3(v3 - v0, v4 - v1, v5 - v2);
        gradient *= (volumeStruct.datasetSpacingRCP_ * 0.5);
        gradient *= volumeStruct.bitDepthScale_;
    }
    return gradient;
}

/**
 * Calculates a voxel's gradient in volume object space based on the blue
 * channel using central differences.
 *
 * @param volume the voxel's volume
 * @param volumeStruct additional information about the passed volume
 * @param samplePos the sample's position in texture space
 * @param t the ray parameter, needed to fix gradients on clipping and
 *          volume borders
 * @param rayDirection the ray direction
 */
vec3 calcGradientB(VOLUME_STRUCT volumeStruct, vec3 samplePos, float t, vec3 rayDirection, SAMPLER2D_TYPE entryPoints, TEXTURE_PARAMETERS entryParameters) {
    vec3 gradient;
    if (t == 0.0 && !onBorder(samplePos, volumeStruct))
        gradient = fixClipBorderGradient(samplePos, rayDirection, entryPoints, entryParameters);
    else {
        vec3 offset = volumeStruct.datasetDimensionsRCP_;
        float v0 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(offset.x, 0.0, 0.0)).b;
        float v1 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(0, offset.y, 0)).b;
        float v2 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(0, 0, offset.z)).b;
        float v3 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(-offset.x, 0, 0)).b;
        float v4 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(0, -offset.y, 0)).b;
        float v5 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(0, 0, -offset.z)).b;
        gradient = vec3(v3 - v0, v4 - v1, v5 - v2);
        gradient *= (volumeStruct.datasetSpacingRCP_ * 0.5);
        gradient *= volumeStruct.bitDepthScale_;
    }
    return gradient;
}

/**
 * Calculates a voxel's gradient in volume object space based on the alpha
 * channel using central differences.
 *
 * @param volume the voxel's volume
 * @param volumeStruct additional information about the passed volume
 * @param samplePos the sample's position in texture space
 * @param t the ray parameter, needed to fix gradients on clipping and
 *          volume borders
 * @param rayDirection the ray direction
 */
vec3 calcGradientA(VOLUME_STRUCT volumeStruct, vec3 samplePos, float t, vec3 rayDirection, SAMPLER2D_TYPE entryPoints, TEXTURE_PARAMETERS entryParameters) {
    vec3 gradient;
    if (t == 0.0 && !onBorder(samplePos, volumeStruct))
        gradient = fixClipBorderGradient(samplePos, rayDirection, entryPoints, entryParameters);
    else {
        vec3 offset = volumeStruct.datasetDimensionsRCP_;
        float v0 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(offset.x, 0.0, 0.0)).a;
        float v1 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(0, offset.y, 0)).a;
        float v2 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(0, 0, offset.z)).a;
        float v3 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(-offset.x, 0, 0)).a;
        float v4 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(0, -offset.y, 0)).a;
        float v5 = textureLookup3DUnnormalized(volumeStruct, samplePos + vec3(0, 0, -offset.z)).a;
        gradient = vec3(v3 - v0, v4 - v1, v5 - v2);
        gradient *= (volumeStruct.datasetSpacingRCP_ * 0.5);
        gradient *= volumeStruct.bitDepthScale_;
    }
    return gradient;
}

/**
 * Calculates a voxel's gradient in volume object space by calculating the
 * gradients for all eight neighbours based on the alpha channel using
 * central differences and filtering between the reuslts.
 *
 * @param volume the voxel's volume
 * @param volumeStruct additional information about the passed volume
 * @param samplePos the sample's position in texture space
 */
vec3 calcGradientFiltered(VOLUME_STRUCT volumeStruct, vec3 samplePos, SAMPLER2D_TYPE entryPoints, TEXTURE_PARAMETERS entryParameters) {
    vec3 delta = volumeStruct.datasetDimensionsRCP_;
    vec3 g0 = calcGradientA(volumeStruct, samplePos, 0.5, vec3(0.0), entryPoints, entryParameters);
    vec3 g1 = calcGradientA(volumeStruct, samplePos+vec3(-delta.x, -delta.y, -delta.z), 0.5, vec3(0.0), entryPoints, entryParameters);
    vec3 g2 = calcGradientA(volumeStruct, samplePos+vec3( delta.x,  delta.y,  delta.z), 0.5, vec3(0.0), entryPoints, entryParameters);
    vec3 g3 = calcGradientA(volumeStruct, samplePos+vec3(-delta.x,  delta.y, -delta.z), 0.5, vec3(0.0), entryPoints, entryParameters);
    vec3 g4 = calcGradientA(volumeStruct, samplePos+vec3( delta.x, -delta.y,  delta.z), 0.5, vec3(0.0), entryPoints, entryParameters);
    vec3 g5 = calcGradientA(volumeStruct, samplePos+vec3(-delta.x, -delta.y,  delta.z), 0.5, vec3(0.0), entryPoints, entryParameters);
    vec3 g6 = calcGradientA(volumeStruct, samplePos+vec3( delta.x,  delta.y, -delta.z), 0.5, vec3(0.0), entryPoints, entryParameters);
    vec3 g7 = calcGradientA(volumeStruct, samplePos+vec3(-delta.x,  delta.y,  delta.z), 0.5, vec3(0.0), entryPoints, entryParameters);
    vec3 g8 = calcGradientA(volumeStruct, samplePos+vec3( delta.x, -delta.y, -delta.z), 0.5, vec3(0.0), entryPoints, entryParameters);
    vec3 mix0 = mix(mix(g1, g2, 0.5), mix(g3, g4, 0.5), 0.5);
    vec3 mix1 = mix(mix(g5, g6, 0.5), mix(g7, g8, 0.5), 0.5);
    return mix(g0, mix(mix0, mix1, 0.5), 0.75);
}

/**
 * Calculates a voxel's gradient in volume object space based on the alpha
 * channel using central differences.
 *
 * @param volume the voxel's volume
 * @param volumeStruct additional information about the passed volume
 * @param samplePos the sample's position in texture space
 */
vec3 calcGradient(VOLUME_STRUCT volumeStruct, vec3 samplePos, SAMPLER2D_TYPE entryPoints, TEXTURE_PARAMETERS entryParameters) {
    return calcGradientA(volumeStruct, samplePos, 0.5, vec3(0.0), entryPoints, entryParameters);
}

/**
 * This module contains all functions which can be used for shading
 * the current sample within a raycaster.
 * The functions below, which implement a full shading model as for
 * instance Phong or Toon shading, are referenced by RC_APPLY_SHADING
 * which is used in the raycaster fragment shaders.
 */

/*
 * This struct contains all information about reuqired
 * to access a light source.
 */
struct LIGHT_SOURCE {
    vec3 position_;        // light position in world space
    vec3 ambientColor_;    // ambient color (r,g,b)
    vec3 diffuseColor_;    // diffuse color (r,g,b)
    vec3 specularColor_;   // specular color (r,g,b)
    vec3 attenuation_;     // attenuation (constant, linear, quadratic)
};


// uniforms needed for shading
uniform vec3 cameraPosition_;   // in world coordinates
uniform float shininess_;       // material shininess parameter
uniform LIGHT_SOURCE lightSource_;


/**
 * Returns attenuation based on the currently set opengl values.
 * Incorporates constant, linear and quadratic attenuation.
 *
 * @param d Distance to the light source.
 */
float getAttenuation(in float d) {
    float att = 1.0 / (lightSource_.attenuation_.x +
                       lightSource_.attenuation_.y * d +
                       lightSource_.attenuation_.z * d * d);
    return min(att, 1.0);
}


/**
 * Returns the ambient term, considering the user defined lighting
 * parameters.
 *
 * @param ka The ambient color to be used, which is fetched from the
 * transfer function.
 */
vec3 getAmbientTerm(in vec3 ka) {
    return ka * lightSource_.ambientColor_;
}


/**
 * Returns the diffuse term, considering the user defined lighting
 * parameters.
 *
 * @param kd The diffuse color to be used, which is fetched from the
 * transfer function.
 * @param N The surface normal used for lambert shading.
 * @param L The normalized light vector used for lambert shading.
 */
vec3 getDiffuseTerm(in vec3 kd, in vec3 N, in vec3 L) {
    float NdotL = max(dot(N, L), 0.0);
    return kd * lightSource_.diffuseColor_ * NdotL;
}


/**
 * This function implements the soft lighting technique described by
 * Josip Basic in the technote 'A cheap soft lighting for real-time 3D
 * environments.
 *
 * @param kd The diffuse color to be used, which is fetched from the
 * transfer function.
 * @param N The surface normal used for lambert shading.
 * @param L The normalized light vector used for lambert shading.
 */
vec3 getLerpDiffuseTerm(in vec3 kd, in vec3 N, in vec3 L) {
    float alpha = 0.5;
    vec3 NV = mix(N, L, alpha);
    float NVdotL = max(dot(NV, L), 0.0);
    return kd * lightSource_.diffuseColor_ * NVdotL;
}


/**
 * Returns the specular term, considering the user defined lighting
 * parameters.
 *
 * @param ks The specular material color to be used.
 * @param N The surface normal used.
 * @param L The normalized light vector used.
 * @param V The viewing vector used.
 * @param alpha The shininess coefficient used.
 */
vec3 getSpecularTerm(in vec3 ks, in vec3 N, in vec3 L, in vec3 V, in float alpha) {
    vec3 H = normalize(V + L);
    float NdotH = pow(max(dot(N, H), 0.0), alpha);
    return ks * lightSource_.specularColor_ * NdotH;
}

/**
 * Calculates phong shading by considering the currently set OpenGL lighting
 * and material parameters.  The front material's shininess parameter is used
 * in the calculation of the specular term.
 * Attenuation is applied, if the symbol PHONG_APPLY_ATTENUATION is defined.
 *
 * @param pos The position to to be lighted given in world space
 * @param normal The object normal given in world space (does not need to be normalized).
 */
vec3 phongShading(in vec3 pos, in vec3 normal, in vec3 ka, in vec3 kd, in vec3 ks) {

    vec3 N = normalize(normal);
    vec3 L = lightSource_.position_ - pos;
    vec3 V = normalize(cameraPosition_ - pos);

    // get light source distance for attenuation and normalize light vector
    float d = length(L);
    L /= d;

    vec3 shadedColor = vec3(0.0);
    shadedColor += getAmbientTerm(ka);
    shadedColor += getDiffuseTerm(kd, N, L);
    shadedColor += getSpecularTerm(ks, N, L, V, shininess_);
    #ifdef PHONG_APPLY_ATTENUATION
        shadedColor *= getAttenuation(d);
    #endif
    return shadedColor;
}

/**********************************************************************
 *    The following functions require volume access and can therefore *
 *    only be defined, when the texture access has been defined in    *
 *    mod_sampler3d.frag or mod_bricking.frag.                        *
 **********************************************************************/

 #ifdef MOD_SAMPLER3D

/**
 * Calculates phong shading for a volume voxel by considering the user defined lighting
 * parameters.
 * The front material's shininess parameter is used in the calculation of the specular term.
 * Attenuation is applied, if the symbol PHONG_APPLY_ATTENUATION is defined.
 *
 * @param gradient The gradient given in volume object space (does not need to be normalized).
 * @param vposTex The voxel position given in volume texture space.
 * @param volumeParams the parameters of the volume to be shaded
 * @param ka The ambient material color to be used.
 * @param kd The diffuse material color to be used.
 * @param ks The specular material color to be used.
 */
vec3 phongShading(in vec3 gradient, in vec3 vposTex, in VOLUME_STRUCT volumeParams, in vec3 ka, in vec3 kd, in vec3 ks) {
    // transform voxel position to the volume's object space
    vec3 vpos = texToPhysical(vposTex, volumeParams);
    vec3 N = normalize(gradient);
    vec3 L = volumeParams.lightPositionOBJ_ - vpos;              // using light position in volume object space
    vec3 V = normalize(volumeParams.cameraPositionOBJ_ - vpos);  // using camera position in volume object space

    // get light source distance for attenuation and normalize light vector
    float d = length(L);
    L /= d;

    vec3 shadedColor = vec3(0.0);
    shadedColor += getAmbientTerm(ka);
    shadedColor += getDiffuseTerm(kd, N, L);
    shadedColor += getSpecularTerm(ks, N, L, V, shininess_);
    #ifdef PHONG_APPLY_ATTENUATION
        shadedColor *= getAttenuation(d);
    #endif
    return shadedColor;
}


/**
 * Calculates phong shading without considering the ambient term.
 * The front material's shininess parameter is used in the calculation of the specular term.
 * Attenuation is applied, if the symbol PHONG_APPLY_ATTENUATION is defined.
 *
 * @param gradient The gradient given in volume object space (does not need to be normalized).
 * @param vposTex The voxel position given in volume texture space.
 * @param volumeParams the parameters of the volume to be shaded
 * @param kd The diffuse material color to be used.
 * @param ks The specular material color to be used.
 */
vec3 phongShadingDS(in vec3 gradient, in vec3 vposTex, in VOLUME_STRUCT volumeParams, in vec3 kd, in vec3 ks) {
    // transform voxel position to the volume's object space
    vec3 vpos = texToPhysical(vposTex, volumeParams);
    vec3 N = normalize(gradient);
    vec3 L = volumeParams.lightPositionOBJ_ - vpos;              // using light position in volume object space
    vec3 V = normalize(volumeParams.cameraPositionOBJ_ - vpos);  // using camera position in volume object space

    // get light source distance for attenuation and normalize light vector
    float d = length(L);
    L /= d;

    vec3 shadedColor = vec3(0.0);
    shadedColor += getDiffuseTerm(kd, N, L);
    shadedColor += getSpecularTerm(ks, N, L, V, shininess_);
    #ifdef PHONG_APPLY_ATTENUATION
        shadedColor *= getAttenuation(d);
    #endif
    return shadedColor;
}


/**
 * Calculates phong shading with only considering the specular term.
 * The front material's shininess parameter is used in the calculation of the specular term.
 * Attenuation is applied, if the symbol PHONG_APPLY_ATTENUATION is defined.
 *
 * @param gradient The gradient given in volume object space (does not need to be normalized).
 * @param vposTex The voxel position given in volume texture space.
 * @param volumeParams the parameters of the volume to be shaded
 * @param ks The specular material color to be used.
 */
vec3 phongShadingS(in vec3 gradient, in vec3 vposTex, in VOLUME_STRUCT volumeParams, in vec3 ks) {
    // transform voxel position to the volume's object space
    vec3 vpos = texToPhysical(vposTex, volumeParams);
    vec3 N = normalize(gradient);
    vec3 L = volumeParams.lightPositionOBJ_ - vpos;              // using light position in volume object space
    vec3 V = normalize(volumeParams.cameraPositionOBJ_ - vpos);  // using camera position in volume object space

    // get light source distance for attenuation and normalize light vector
    float d = length(L);
    L /= d;

    vec3 shadedColor = vec3(0.0);
    shadedColor += getSpecularTerm(ks, N, L, V, shininess_);
    #ifdef PHONG_APPLY_ATTENUATION
        shadedColor *= getAttenuation(d);
    #endif
    return shadedColor;
}


/**
 * Calculates phong shading without considering the specular term.
 * Attenuation is applied, if the symbol PHONG_APPLY_ATTENUATION is defined.
 *
 * @param gradient The gradient given in volume object space (does not need to be normalized).
 * @param vposTex The voxel position given in volume texture space.
 * @param volumeParams the parameters of the volume to be shaded
 * @param kd The diffuse material color to be used.
 * @param ka The ambient material color to be used.
 */
vec3 phongShadingDA(in vec3 gradient, in vec3 vposTex, in VOLUME_STRUCT volumeParams, in vec3 kd, in vec3 ka) {
    // transform voxel position to the volume's object space
    vec3 vpos = texToPhysical(vposTex, volumeParams);
    vec3 N = normalize(gradient);
    vec3 L = volumeParams.lightPositionOBJ_ - vpos;    // using light position in volume object space

    // get light source distance for attenuation and normalize light vector
    float d = length(L);
    L /= d;

    vec3 shadedColor = vec3(0.0);
    shadedColor += getAmbientTerm(ka);
    shadedColor += getDiffuseTerm(kd, N, L);
    #ifdef PHONG_APPLY_ATTENUATION
        shadedColor *= getAttenuation(d);
    #endif
    return shadedColor;
}


/**
 * Calculates lambertian shading.
 * Attenuation is applied, if the symbol PHONG_APPLY_ATTENUATION is defined.
 *
 * @param gradient The gradient given in volume object space (does not need to be normalized).
 * @param vposTex The voxel position given in volume texture space.
 * @param volumeParams the parameters of the volume to be shaded
 * @param kd The diffuse material color to be used.
 */
vec3 phongShadingD(in vec3 gradient, in vec3 vposTex, in VOLUME_STRUCT volumeParams, in vec3 kd) {
    // transform voxel position to the volume's object space
    vec3 vpos = texToPhysical(vposTex, volumeParams);
    vec3 N = normalize(gradient);
    vec3 L = volumeParams.lightPositionOBJ_ - vpos;    // using light position in volume object space

    // get light source distance for attenuation and normalize light vector
    float d = length(L);
    L /= d;

    vec3 shadedColor = vec3(0.0);
    shadedColor += getDiffuseTerm(kd, N, L);
    #ifdef PHONG_APPLY_ATTENUATION
        shadedColor *= getAttenuation(d);
    #endif
    return shadedColor;
}

/**
 * Calculates a quantized toon shading.
 *
 * @param gradient The gradient given in volume object space (does not need to be normalized).
 * @param vposTex The voxel position given in volume texture space.
 * @param volumeParams the parameters of the volume to be shaded
 * @param numShades The number of different shadings.
 */
vec3 toonShading(in vec3 gradient, in vec3 vposTex, in VOLUME_STRUCT volumeParams, in vec3 kd, in int numShades) {
    // transform voxel position to the volume's object space
    vec3 vpos = texToPhysical(vposTex, volumeParams);
    vec3 N = normalize(gradient);
    vec3 L = volumeParams.lightPositionOBJ_ - vpos;    // using light position in volume object space
    float NdotL = max(dot(N,L),0.0);

    kd *= lightSource_.diffuseColor_;

    for (int i=1; i <= numShades; i++) {
        if (NdotL <= float(i) / float(numShades)) {
            NdotL = float(i) / float(numShades);
            break;
        }
    }
    return vec3(kd.rgb * NdotL);
}

#endif // MOD_SAMPLER3D

/**
 * This module contains all functions which can be used for compositing
 * voxels within a raycaster.
 * The functions are referenced by RC_APPLY_COMPOSITING as used in the
 * raycaster fragment shaders.
 */

/**
 * Reciprocal value of the reference sampling interval used
 * for the opacity correction that is necessary to compensate
 * variable sampling rates.
 *
 * See Engel et. al.: "Real-Time Volume Graphics" - Ch 9.1.3
 */
const float SAMPLING_BASE_INTERVAL_RCP = 200.0;
const float SAMPLING_BASE_INTERVAL_SLICE_RCP = 105.0;

/**
 * Performs regular DVR compositing. Expects the current voxels color
 * and the intermediate result. Returns the result after compositing.
 *
 */
vec4 compositeDVR(in vec4 curResult, in vec4 color, in float t, inout float tDepth) {
    vec4 result = curResult;

    // apply opacity correction to accomodate for variable sampling intervals
    color.a = 1.0 - pow(1.0 - color.a, samplingStepSize_ * SAMPLING_BASE_INTERVAL_RCP);

    result.rgb = result.rgb + (1.0 - result.a) * color.a * color.rgb;
    result.a = result.a + (1.0 -result.a) * color.a;
    // save first hit ray parameter for depth value calculation
    if (tDepth < 0.0)
        tDepth = t;
    return result;
}

/**
 * Performs MIP (maximum intensity projection) compositing. Expects the current
 * voxels color and the intermediate result. Returns the result after compositing.
 *
 */
vec4 compositeMIP(in vec4 curResult, in vec4 color, in float t, inout float tDepth) {
    vec4 result;
    if (color.a > curResult.a) {
        result = color;
        // save ray parameter for depth value calculation
        tDepth = t;
    }
    else result = curResult;
    return result;
}

/// Acutally performs the MIDA raycasting compositing
vec4 compositeMIDAhelper(in vec4 curResult, in vec4 voxel, in vec4 color, inout float f_max_i, in float t, inout float tDepth, in float gamma) {
    vec4 result = curResult;
    float delta_i = 0.0;
    if (voxel.a > f_max_i) {
        delta_i = voxel.a - f_max_i;
    }
    float beta_i = 1.0 - delta_i * (1.0 + gamma);

    result.rgb = beta_i * result.rgb + (1.0 - beta_i * result.a) * color.a * color.rgb;
    result.a = beta_i * result.a + (1.0 - beta_i * result.a) * color.a;

    if (tDepth < 0.0)
        tDepth = t;

    return result;

}

/**
 * Performs MIDA (maximum intensity difference accumulation) compositing as described in
 * the paper "Instant Volume Visualization using Maximum Intensity Difference Accumulation"
 * by Bruckner et al. published in Eurographics 2009
 */
vec4 compositeMIDA(in vec4 curResult, in vec4 voxel, in vec4 color, inout float f_max_i, in float t, inout float tDepth, in float gamma) {
    // apply opacity correction to accomodate for variable sampling intervals
    color.a = 1.0 - pow(1.0 - color.a, samplingStepSize_ * SAMPLING_BASE_INTERVAL_RCP);

    vec4 result = curResult;

    if (gamma <= 0.0) {
        result = compositeMIDAhelper(result, voxel, color, f_max_i, t, tDepth, gamma);
    }
    else {
        vec4 mipResult = compositeMIP(result, color, t, tDepth);
        vec4 midaResult = compositeMIDAhelper(result, voxel, color, f_max_i, t, tDepth, 0.0);

        result = gamma * mipResult + (1.0 - gamma) * midaResult;
    }

    return result;
}

/**
 * Performs isosurface rendering compositing. Expects the current voxels color
 * and the intermediate result. Returns the result after compositing.
 *
 */
vec4 compositeISO(in vec4 curResult, in vec4 color, in float t, inout float tDepth, in float isoValue) {
    vec4 result = curResult;
    float epsilon = 0.02;
    if (color.a >= isoValue-epsilon && color.a <= isoValue+epsilon) {
        result = color;
        result.a = 1.0;
        // save ray parameter for depth value calculation
        tDepth = t;
    }
    return result;
}

/**
 * Performs first hit point compositing.
 */
vec4 compositeFHP(in vec3 samplePos, in vec4 curResult, in float t, inout float tDepth) {
    vec4 result = curResult;
    // save first hit point
    if (result.xyz == vec3(0.0)) {
        result.xyz = samplePos;
        result.a = 1.0;
        // save first hit ray parameter for depth value calculation
        if (tDepth < 0.0)
            tDepth = t;
    }
    return result;
}

/**
 * Performs first hit normals (gradients) compositing.
 */
vec4 compositeFHN(in vec3 gradient, in vec4 curResult, in float t, inout float tDepth) {
    vec4 result = curResult;
    // save first hit normal
    if (result.xyz == vec3(0.0)) {
        result.xyz = normalize(gradient) / 2.0 + 0.5;
        result.a = 1.0;
        // save first hit ray parameter for depth value calculation
        if (tDepth < 0.0)
            tDepth = t;
    }
    return result;
}

/**
 * Performs first hit texture coordinate compositing.
 */
vec4 compositeFHT(in vec3 texCoords, in vec4 curResult, in float t, inout float tDepth) {
    vec4 result = curResult;
    // save first hit normal
    if (result.xyz == vec3(0.0)) {
        result.xyz = texCoords;
        result.a = 1.0;
        // save first hit ray parameter for depth value calculation
        if (tDepth < 0.0)
            tDepth = t;
    }
    return result;
}

uniform float const_to_z_w_1;
uniform float const_to_z_w_2;
uniform float const_to_z_e_1;
uniform float const_to_z_e_2;


/**
 * Calculates the depth value for the current sample specified by the parameter t.
 **/
float calculateDepthValue(float t, float entryPointsDepth, float exitPointsDepth) {
   /*
    Converting eye coordinate depth values to windows coordinate depth values:
    (see http://www.opengl.org/resources/faq/technical/depthbuffer.htm 12.050, assuming w_e = 1)

    z_w = (1.0/z_e)*((f*n)/(f-n)) + 0.5*((f+n)/(f-n))+0.5; (f=far plane, n=near plane)

    We calculate constant terms outside:
    const_to_z_w_1 = ((f*n)/(f-n))
    const_to_z_w_2 = 0.5*((f+n)/(f-n))+0.5

    Converting windows coordinates to eye coordinates:

    z_e = 1.0/([z_w - 0.5 - 0.5*((f+n)/(f-n))]*((f-n)/(f*n)));

    with constant terms
    const_to_z_e_1 = 0.5 + 0.5*((f+n)/(f-n))
    const_to_z_e_2 = ((f-n)/(f*n))
   */

    // assign front value given in windows coordinates
    float zw_front = entryPointsDepth;
    // and convert it into eye coordinates
    float ze_front = 1.0/((zw_front - const_to_z_e_1)*const_to_z_e_2);

    // assign back value given in windows coordinates
    float zw_back = exitPointsDepth;
    // and convert it into eye coordinates
    float ze_back = 1.0/((zw_back - const_to_z_e_1)*const_to_z_e_2);

    // interpolate in eye coordinates
    float ze_current = ze_front + t*(ze_back-ze_front);

    // convert back to window coordinates
    float zw_current = (1.0/ze_current)*const_to_z_w_1 + const_to_z_w_2;

    return zw_current;
}

/**
 * Returns the depth value for a given t by considering the ray
 * length as well as the depth of the entry and exit points.
 */
float getDepthValue(float t, float tEnd, float entryPointsDepth, float exitPointsDepth) {
    if (t >= 0.0)
        return calculateDepthValue(t/tEnd, entryPointsDepth, exitPointsDepth);
    else
        return 1.0;
}

/**
 * Returns the depth value for a given t by considering the ray
 * length as well as the depth of the entry and exit points.
 */
float getDepthValue(float t, float tEnd, SAMPLER2D_TYPE entryPointsDepth, TEXTURE_PARAMETERS entryParameters, SAMPLER2D_TYPE exitPointsDepth, TEXTURE_PARAMETERS exitParameters) {
    if (t >= 0.0)
        return calculateDepthValue(t/tEnd, textureLookup2D(entryPointsDepth, entryParameters, gl_FragCoord.xy).z,
                                           textureLookup2D(exitPointsDepth, exitParameters, gl_FragCoord.xy).z);
    else
        return 1.0;
}

/**
 * Calculates the depth value for a given t by considering the ray
 * length as well as the depth of the entry and exit points and
 * writes it to gl_FragDepth.
 *
 * This macro is an inlining of the getDepthValue() and calculateDepthValue() functions above.
 * It is introduced in order to deal with older ATI boards on Mac where function calls seem
 * to drastically reduce rendering speed.
 */
#if defined(VRN_OS_APPLE) && defined(VRN_VENDOR_ATI)
  // We do manual inlining in order to deal with older ATI boards on Mac where these function
  // calls seem to drastically reduce rendering speed (triggering fallback to software mode).
  #define WRITE_DEPTH_VALUE(t, tEnd, entryPointsDepth, entryParameters, exitPointsDepth, exitParameters)  \
    if (t >= 0.0) {                                                             \
        float zw_front = textureLookup2D(entryPointsDepth, entryParameters, gl_FragCoord.xy).z;  \
        float ze_front = 1.0/((zw_front - const_to_z_e_1)*const_to_z_e_2);      \
        float zw_back = textureLookup2D(exitPointsDepth, exitParameters, gl_FragCoord.xy).z;    \
        float ze_back = 1.0/((zw_back - const_to_z_e_1)*const_to_z_e_2);        \
        float ze_current = ze_front + (t/tEnd)*(ze_back-ze_front);              \
        float zw_current = (1.0/ze_current)*const_to_z_w_1 + const_to_z_w_2;    \
        gl_FragDepth = zw_current;                                              \
    }                                                                           \
    else                                                                        \
        gl_FragDepth = 1.0;
#endif

uniform SAMPLER2D_TYPE entryPoints_;          // ray entry points
uniform SAMPLER2D_TYPE entryPointsDepth_;     // ray entry points depth
uniform TEXTURE_PARAMETERS entryParameters_;
uniform SAMPLER2D_TYPE exitPoints_;           // ray exit points
uniform SAMPLER2D_TYPE exitPointsDepth_;      // ray exit points depth
uniform TEXTURE_PARAMETERS exitParameters_;

uniform VOLUME_STRUCT volume_;            // texture lookup parameters for volume_

/***
 * Performs direct volume rendering and
 * returns the final fragment color.
 ***/
vec4 directRendering(in vec3 first, in vec3 last, vec2 p) {

    vec4 result = vec4(0.0);
    float depthT = -1.0;
    bool finished = false;

    // calculate ray parameters
    float stepIncr, tend;
    float t = 0.0;
    vec3 direction;
    raySetup(first, last, volume_.datasetDimensions_, direction, stepIncr, tend);

    // 2 nested loops allow for more than 255 iterations,
    // but is slower than while (t < tend)
    for (int loop0=0; !finished && loop0<255; loop0++) {
        for (int loop1=0; !finished && loop1<255; loop1++) {

            vec3 sampleVal = first.rgb + t * direction;
            vec4 voxel = getVoxel(volume_, sampleVal);

            // no shading is applied
            vec4 color = applyTF(transferFunc_, voxel);

            // perform compositing
            if (color.a > 0.0) {
                // accomodate for variable sampling rates (base interval defined by mod_compositing.frag)
                color.a = 1.0 - pow(1.0 - color.a, samplingStepSize_ * SAMPLING_BASE_INTERVAL_RCP);
                result.rgb = result.rgb + (1.0 - result.a) * color.a * color.rgb;
                result.a = result.a + (1.0 -result.a) * color.a;
            }

            // save first hit ray parameter for depth value calculation
            if (depthT < 0.0 && result.a > 0.0)
                depthT = t;

            // early ray termination
            if (result.a >= 1.0) {
                result.a = 1.0;
                finished = true;
            }

            t += stepIncr;
            finished = finished || (t > tend);
        }
    }

    // calculate depth value from ray parameter
    gl_FragDepth = 1.0;
    if (depthT >= 0.0)
        gl_FragDepth = calculateDepthValue(depthT/tend, textureLookup2Dnormalized(entryPointsDepth_, entryParameters_, p).z,
                                                        textureLookup2Dnormalized(exitPointsDepth_, exitParameters_, p).z);


    return result;
}

/***
 * The main method.
 ***/
void main() {
    vec2 p = gl_FragCoord.xy * screenDimRCP_;
    vec3 frontPos = textureLookup2Dnormalized(entryPoints_, entryParameters_, p).rgb;
    vec3 backPos = textureLookup2Dnormalized(exitPoints_, exitParameters_, p).rgb;

    //determine whether the ray has to be casted
    if (frontPos == backPos) {
        //background need no raycasting
        discard;
    } else {
        //fragCoords are lying inside the boundingbox
        FragData0 = directRendering(frontPos, backPos, p);
    }
}

