#pragma once

#include "FixedString.h"


extern FixedString* FS_WVPMatrix;
extern FixedString* FS_InvWVPMatrix;
extern FixedString* FS_WorldMatrix;
extern FixedString* FS_ViewMatrix;
extern FixedString* FS_WorldViewMatrix;
extern FixedString* FS_ProjectionMatrix;
extern FixedString* FS_fogColor;
extern FixedString* FS_fogStart;
extern FixedString* FS_fogEnd;
extern FixedString* FS_fogDensity;
extern FixedString* FS_lightDiffuse;
extern FixedString* FS_lightAmbient;
extern FixedString* FS_lightSpecular;
extern FixedString* FS_ambientColor;
extern FixedString* FS_textureProjMatrix;
extern FixedString* FS_clipPlane;
extern FixedString* FS_sunLight;
extern FixedString* FS_sunColor;
extern FixedString* FS_cameraPos;
extern FixedString* FS_uvScales;
extern FixedString* FS_uvScales_1;
extern FixedString* FS_uvScale;
extern FixedString* FS_shininess;
extern FixedString* FS_ka;
extern FixedString* FS_ks;
extern FixedString* FS_kd;
extern FixedString* FS_frameTime;
extern FixedString* FS_boneMatrix;
extern FixedString* FS_boneMatrix_1;
extern FixedString* FS_worldMatrixArray;
extern FixedString* FS_positionArray;
extern FixedString* FS_LightPosition;
extern FixedString* FS_LightRadius;

extern FixedString* FS_LightPositionInView;
extern FixedString* FS_InvViewProjMatrix;
extern FixedString* FS_farPlane;
extern FixedString* FS_FrustumCorners;
extern FixedString* FS_halfPixel;
extern FixedString* FS_colorDiffuse;
extern FixedString* FS_ViewProjMatrix;
extern FixedString* FS_nClipEnable;

extern FixedString* FS_JitterScale;
extern FixedString* FS_FilterSize;
extern FixedString* FS_jitterDelta;
extern FixedString* FS_LightSpaceProjMatrix;
extern FixedString* FS_LightSpaceViewProjMatrix;
extern FixedString* FS_AlphaTest;

//œ¬√Ê «∫Í
extern FixedString* FS_SHADOWMAP;
extern FixedString* FS_RENDER_TO_GBUFFER;
extern FixedString* FS_LIGHT_PRE_PASS;
extern FixedString* FS_RENDER_TO_SHADOWMAP;
extern FixedString* FS_TEX_LUMINANCE;
extern FixedString* FS_HAS_REFLECT;
extern FixedString* FS_HAS_REFRACT;
extern FixedString* FS_DEPTHMAP_SOFT_EDGE;
extern FixedString* FS_STARFIELD;
extern FixedString* FS_LDR;
extern FixedString* FS_HARDWARE_SKIN;

extern FixedString* FS_TADDRESS_WRAP;
extern FixedString* FS_TADDRESS_MIRROR;
extern FixedString* FS_TADDRESS_CLAMP;
extern FixedString* FS_TADDRESS_BORDER;
extern FixedString* FS_TADDRESS_MIRRORONCE;

extern FixedString* FS_TEXF_NONE;
extern FixedString* FS_TEXF_POINT;
extern FixedString* FS_TEXF_LINEAR;
extern FixedString* FS_TEXF_ANISOTROPIC;
extern FixedString* FS_TEXF_PYRAMIDALQUAD;
extern FixedString* FS_TEXF_GAUSSIANQUAD;
extern FixedString* FS_TEXF_CONVOLUTIONMONO;

void InitAllInternalFixedStrings();
void DestoryAllInternalFixedStrings();
