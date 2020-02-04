#include "InternalFixedStrings.h"
#include "Util.h"

FixedString* FS_WVPMatrix = NULL;
FixedString* FS_InvWVPMatrix = NULL;
FixedString* FS_WorldMatrix = NULL;
FixedString* FS_ViewMatrix = NULL;
FixedString* FS_WorldViewMatrix = NULL;
FixedString* FS_ProjectionMatrix = NULL;
FixedString* FS_fogColor = NULL;
FixedString* FS_fogStart = NULL;
FixedString* FS_fogEnd = NULL;
FixedString* FS_fogDensity = NULL;
FixedString* FS_lightDiffuse = NULL;
FixedString* FS_lightAmbient = NULL;
FixedString* FS_lightSpecular = NULL;
FixedString* FS_ambientColor = NULL;
FixedString* FS_textureProjMatrix = NULL;
FixedString* FS_clipPlane = NULL;
FixedString* FS_sunLight = NULL;
FixedString* FS_sunColor = NULL;
FixedString* FS_cameraPos = NULL;
FixedString* FS_uvScales = NULL;
FixedString* FS_uvScales_1 = NULL;
FixedString* FS_uvScale = NULL;
FixedString* FS_shininess = NULL;
FixedString* FS_ka = NULL;
FixedString* FS_ks = NULL;
FixedString* FS_kd = NULL;
FixedString* FS_frameTime = NULL;
FixedString* FS_boneMatrix = NULL;
FixedString* FS_boneMatrix_1 = NULL;
FixedString* FS_worldMatrixArray = NULL;
FixedString* FS_positionArray = NULL;
FixedString* FS_LightPosition = NULL;
FixedString* FS_LightRadius = NULL;

FixedString* FS_LightPositionInView = NULL;
FixedString* FS_InvViewProjMatrix = NULL;
FixedString* FS_farPlane = NULL;
FixedString* FS_FrustumCorners = NULL;
FixedString* FS_halfPixel = NULL;
FixedString* FS_colorDiffuse = NULL;
FixedString* FS_ViewProjMatrix = NULL;
FixedString* FS_nClipEnable = NULL;

FixedString* FS_JitterScale = NULL;
FixedString* FS_FilterSize = NULL;
FixedString* FS_jitterDelta = NULL;
FixedString* FS_LightSpaceProjMatrix = NULL;
FixedString* FS_LightSpaceViewProjMatrix = NULL;
FixedString* FS_AlphaTest = NULL;

FixedString* FS_SHADOWMAP = NULL;
FixedString* FS_RENDER_TO_GBUFFER = NULL;
FixedString* FS_LIGHT_PRE_PASS = NULL;
FixedString* FS_RENDER_TO_SHADOWMAP = NULL;
FixedString* FS_TEX_LUMINANCE = NULL;
FixedString* FS_HAS_REFLECT = NULL;
FixedString* FS_HAS_REFRACT = NULL;
FixedString* FS_DEPTHMAP_SOFT_EDGE = NULL;
FixedString* FS_STARFIELD = NULL;
FixedString* FS_LDR = NULL;
FixedString* FS_HARDWARE_SKIN = NULL;

FixedString* FS_TADDRESS_WRAP = NULL;
FixedString* FS_TADDRESS_MIRROR = NULL;
FixedString* FS_TADDRESS_CLAMP = NULL;
FixedString* FS_TADDRESS_BORDER = NULL;
FixedString* FS_TADDRESS_MIRRORONCE = NULL;

FixedString* FS_TEXF_NONE = NULL;
FixedString* FS_TEXF_POINT = NULL;
FixedString* FS_TEXF_LINEAR = NULL;
FixedString* FS_TEXF_ANISOTROPIC = NULL;
FixedString* FS_TEXF_PYRAMIDALQUAD = NULL;
FixedString* FS_TEXF_GAUSSIANQUAD = NULL;
FixedString* FS_TEXF_CONVOLUTIONMONO = NULL;


void InitAllInternalFixedStrings()
{
    FS_WVPMatrix = new FixedString("wvpMatrix");
    FS_InvWVPMatrix = new FixedString("invWvpMatrix");
    FS_WorldMatrix = new FixedString("worldMatrix");
    FS_ViewMatrix = new FixedString("viewMatrix");
    FS_WorldViewMatrix = new FixedString("WorldViewMatrix");
    FS_ProjectionMatrix = new FixedString("projectionMatrix");
    FS_fogColor = new FixedString("fogColor");
    FS_fogStart = new FixedString("fogStart");
    FS_fogEnd = new FixedString("fogEnd");
    FS_fogDensity = new FixedString("fogDensity");
    FS_lightDiffuse = new FixedString("lightDiffuse");
    FS_lightAmbient = new FixedString("lightAmbient");
    FS_lightSpecular = new FixedString("lightSpecular");
    FS_ambientColor = new FixedString("ambientColor");
    FS_textureProjMatrix = new FixedString("textureProjMatrix");
    FS_clipPlane = new FixedString("clipPlane");
    FS_sunLight = new FixedString("sunLight");
    FS_sunColor = new FixedString("sunColor");
    FS_cameraPos = new FixedString("cameraPos");
    FS_uvScales = new FixedString("uvScales");
    FS_uvScales_1 = new FixedString("uvScales[0]");
    FS_uvScale = new FixedString("uvScale");
    FS_shininess = new FixedString("shininess");
    FS_ka = new FixedString("ka");
    FS_ks = new FixedString("ks");
    FS_kd = new FixedString("kd");
    FS_frameTime = new FixedString("frameTime");
    FS_boneMatrix = new FixedString("boneMatrix");
    FS_boneMatrix_1 = new FixedString("boneMatrix[0]");
    FS_worldMatrixArray = new FixedString("worldMatrixArray");
    FS_positionArray = new FixedString("positionArray");
    FS_LightPosition = new FixedString("LightPosition");
    FS_LightRadius = new FixedString("LightRadius");

    FS_LightPositionInView = new FixedString("LightPositionInView");
    FS_InvViewProjMatrix = new FixedString("InvViewProjMatrix");
    FS_farPlane = new FixedString("farPlane");
    FS_FrustumCorners = new FixedString("FrustumCorners");
    FS_halfPixel = new FixedString("halfPixel");
    FS_colorDiffuse = new FixedString("colorDiffuse");
    FS_ViewProjMatrix = new FixedString("ViewProjMatrix");
    FS_nClipEnable = new FixedString("nClipEnable");

    FS_JitterScale = new FixedString("JitterScale");
    FS_FilterSize = new FixedString("FilterSize");
    FS_jitterDelta = new FixedString("jitterDelta");
    FS_LightSpaceProjMatrix = new FixedString("LightSpaceProjMatrix");
    FS_LightSpaceViewProjMatrix = new FixedString("LightSpaceViewProjMatrix");
    FS_AlphaTest = new FixedString("alphaTest");

    FS_SHADOWMAP = new FixedString("SHADOWMAP");
    FS_RENDER_TO_GBUFFER = new FixedString("RENDER_TO_GBUFFER");
    FS_LIGHT_PRE_PASS = new FixedString("LIGHT_PRE_PASS");
    FS_RENDER_TO_SHADOWMAP = new FixedString("RENDER_TO_SHADOWMAP");
    FS_TEX_LUMINANCE = new FixedString("TEX_LUMINANCE");
    FS_HAS_REFLECT = new FixedString("HAS_REFLECT");
    FS_HAS_REFRACT = new FixedString("HAS_REFRACT");
    FS_DEPTHMAP_SOFT_EDGE = new FixedString("DEPTHMAP_SOFT_EDGE");
    FS_STARFIELD = new FixedString("STARFIELD");
    FS_LDR = new FixedString("LDR");
	FS_HARDWARE_SKIN = new FixedString("HARDWARE_SKIN");

    FS_TADDRESS_WRAP = new FixedString("TADDRESS_WRAP");
    FS_TADDRESS_MIRROR = new FixedString("TADDRESS_MIRROR");
    FS_TADDRESS_CLAMP = new FixedString("TADDRESS_CLAMP");
    FS_TADDRESS_BORDER = new FixedString("TADDRESS_BORDER");
    FS_TADDRESS_MIRRORONCE = new FixedString("TADDRESS_MIRRORONCE");

    FS_TEXF_NONE = new FixedString("TEXF_NONE");
    FS_TEXF_POINT = new FixedString("TEXF_POINT");
    FS_TEXF_LINEAR = new FixedString("TEXF_LINEAR");
    FS_TEXF_ANISOTROPIC = new FixedString("TEXF_ANISOTROPIC");
    FS_TEXF_PYRAMIDALQUAD = new FixedString("TEXF_PYRAMIDALQUAD");
    FS_TEXF_GAUSSIANQUAD = new FixedString("TEXF_GAUSSIANQUAD");
    FS_TEXF_CONVOLUTIONMONO = new FixedString("TEXF_CONVOLUTIONMONO");
}

void DestoryAllInternalFixedStrings()
{
    SafeDelete(FS_WVPMatrix);
    SafeDelete(FS_InvWVPMatrix);
    SafeDelete(FS_WorldMatrix);
    SafeDelete(FS_ViewMatrix);
    SafeDelete(FS_WorldViewMatrix);
    SafeDelete(FS_ProjectionMatrix);
    SafeDelete(FS_fogColor);
    SafeDelete(FS_fogStart);
    SafeDelete(FS_fogEnd);
    SafeDelete(FS_fogDensity);
    SafeDelete(FS_lightDiffuse);
    SafeDelete(FS_lightAmbient);
    SafeDelete(FS_lightSpecular);
    SafeDelete(FS_ambientColor);
    SafeDelete(FS_textureProjMatrix);
    SafeDelete(FS_clipPlane);
    SafeDelete(FS_sunLight);
    SafeDelete(FS_sunColor);

    SafeDelete(FS_cameraPos);
    SafeDelete(FS_uvScales);

    SafeDelete(FS_shininess);
    SafeDelete(FS_ka);
    SafeDelete(FS_ks);
    SafeDelete(FS_kd);
    SafeDelete(FS_frameTime);

    SafeDelete(FS_boneMatrix);
    SafeDelete(FS_boneMatrix_1);
    SafeDelete(FS_worldMatrixArray);
    SafeDelete(FS_positionArray);
    SafeDelete(FS_LightPosition);
    SafeDelete(FS_LightRadius);

    SafeDelete(FS_LightPositionInView);
    SafeDelete(FS_InvViewProjMatrix);
    SafeDelete(FS_farPlane);
    SafeDelete(FS_FrustumCorners);
    SafeDelete(FS_halfPixel);
    SafeDelete(FS_colorDiffuse);
    SafeDelete(FS_ViewProjMatrix);
    SafeDelete(FS_nClipEnable);

    SafeDelete(FS_JitterScale);
    SafeDelete(FS_FilterSize);
    SafeDelete(FS_jitterDelta);
    SafeDelete(FS_LightSpaceProjMatrix);
    SafeDelete(FS_LightSpaceViewProjMatrix);
    SafeDelete(FS_AlphaTest);

    SafeDelete(FS_SHADOWMAP);
    SafeDelete(FS_RENDER_TO_GBUFFER);
    SafeDelete(FS_LIGHT_PRE_PASS);
    SafeDelete(FS_RENDER_TO_SHADOWMAP);
    SafeDelete(FS_TEX_LUMINANCE);
    SafeDelete(FS_HAS_REFLECT);
    SafeDelete(FS_HAS_REFRACT);
    SafeDelete(FS_DEPTHMAP_SOFT_EDGE);
    SafeDelete(FS_STARFIELD);
    SafeDelete(FS_LDR);
	SafeDelete(FS_HARDWARE_SKIN);

    SafeDelete(FS_TADDRESS_WRAP);
    SafeDelete(FS_TADDRESS_MIRROR);
    SafeDelete(FS_TADDRESS_CLAMP);
    SafeDelete(FS_TADDRESS_BORDER);
    SafeDelete(FS_TADDRESS_MIRRORONCE);

    SafeDelete(FS_TEXF_NONE);
    SafeDelete(FS_TEXF_POINT);
    SafeDelete(FS_TEXF_LINEAR);
    SafeDelete(FS_TEXF_ANISOTROPIC);
    SafeDelete(FS_TEXF_PYRAMIDALQUAD);
    SafeDelete(FS_TEXF_GAUSSIANQUAD);
    SafeDelete(FS_TEXF_CONVOLUTIONMONO);
}
