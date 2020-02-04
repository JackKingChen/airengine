// ***************************************************************
//  文件: EngineDefination.h
//  日期: 2010-3-11
//  作者: liangairan
//  说明: 引擎的一些公共定义
// ***************************************************************

#pragma once 

#include "EngineConfig.h"
#include "AIRTypes.h"
#include "MemoryAllocatorConfig.h"
#include <assert.h>
#include <map>
#include <string>

enum ETextureFilter
{
    ETYPE_MAGNIFICATION					= 0,
    ETYPE_MINIFICATION					= 1,
    ETYPE_MIPMAP						= 2,
};


enum EDeviceState
{
    EDEVICESTATE_OK						= 0,
    EDEVICESTATE_DEVICELOST				= 1,
    EDEVICESTATE_DEVICENOTRESET			= 2,
    EDEVICESTATE_DRIVERINTERNALERROR	= 3,
};

/** Fog modes. */
enum FogMode
{
    /// No fog. Duh.
    eFOG_NONE,
    /// Fog density increases  exponentially from the camera (fog = 1/e^(distance * density))
    eFOG_EXP,
    /// Fog density increases at the square of FOG_EXP, i.e. even quicker (fog = 1/e^(distance * density)^2)
    eFOG_EXP2,
    /// Fog density increases linearly between the start and end distances
    eFOG_LINEAR
};

//shader types
enum GPUShaderType
{
    eGST_Vertex,
    eGST_Pixel,
    eGST_Geometry,
};

enum RenderParamType
{
    eRPT_TextureFilter,
    eRPT_TextureAddress,
    eRPT_TextureStageState,
    eRPT_RenderState,
    eRPT_SamplerType,
};

enum RenderSystemResourceType
{
    eRSRT_Surface = 1,
    eRSRT_Volume,
    eRSRT_Texture,
    eRSRT_VolumeTexture,
    eRSRT_CubeTexture,
    eRSRT_VertexBuffer,
    eRSRT_IndexBuffer,
};

enum RenderSystemUsage
{
    eRSU_Default = 0,
    eRSU_AutoGenMipmap = 1,
    eRSU_DepthStencil,
    eRSU_DMap,
    eRSU_DoNotClip,
    eRSU_Dynamic,
    eRSU_Nonsecure,
    eRSU_NPatches,
    eRSU_Points,
    eRSU_RenderTarget,
    eRSU_RTPATCHES,
    eRSU_SoftwareProcessing,
    eRSU_TextApi,
    eRSU_WriteOnly,
	eRSU_Shadowmap, //这个用处主要是适应dx9与dx11的不同之处
};

enum RendererPixelFormat
{
    RPF_A8R8G8B8 = 21,
    RPF_X8R8G8B8 = 22,
    RPF_D24S8, 
    RPF_L8,
    RPF_L16,
    RPF_R8G8B8,
    RPF_R16F,
    RPF_A16B16G16R16F  = 113,
    RPF_R32F = 114,
    RPF_G32R32F   = 115,
    RPF_A32B32G32R32F,
	RPF_D32F,
    RPF_A8L8,
	RPF_R24X8,
	RPF_ALPHA,
	RPF_R4G4B4A4,
};

enum RenderSystemFillMode
{
    RSFM_Point,
    RSFM_Solid,
    RSFM_WireFrame,
};

enum RenderStateBlendValue
{
    RSRV_Zero,
    RSRV_One,
    RSRV_SrcColor,
    RSRV_InvSrcColor,
    RSRV_SrcAlpha,
    RSRV_InvSrcAlpha,
    RSRV_DestColor,
    RSRV_InvDestColor,
    RSRV_SrcAlphaSat,
    RSRV_BothSrcAlpha,
    RSRV_BothInvSrcAlpha,
    RSRV_BlendFactor,
    RSRV_InvBlendFactor,
    RSRV_SrcColor2,
    RSRV_InvSrcColor2,
    RSRV_DestAlpha,
    RSRV_InvDestAlpha,
};

enum BlendOpt
{
    BO_Add,
    BO_Subtract,
    BO_RevSubtract,
    BO_Min,
    BO_Max,
};

enum TextureStageStateType
{
    TSST_ColorOp,
    TSST_ColorArg1,
    TSST_ColorArg2,
    TSST_AlphaOp,
    TSST_AlphaArg1,
    TSST_AlphaArg2,
    TSST_BumpenvMat00,
    TSST_BumpenvMat01,
    TSST_BumpenvMat10,
    TSST_BumpenvMat11,
    TSST_TexcoordIndex,
    TSST_BumpenvlScale,
    TSST_BumpenvlOffset,
    TSST_TextureTransformFlags,
    TSST_ColorArg0,
    TSST_AlphaArg0,
    TSST_ResultArg,
    TSST_Constant,
};

enum TextureOp
{
    TO_Disable,
    TO_SelectArg1,
    TO_SelectArg2,
    TO_Modulate,
    TO_Modulate2x,
    TO_Modulate4x,
    TO_Add,
    TO_AddSigned,
    TO_AddSigned2x,
    TO_Subtract,
    TO_AddSmooth,
    TO_BlendDiffuseAlpha,
    TO_BlendTextureAlpha,
    TO_BlendFactorAlpha,
    TO_BlendTextureAlphaPM,
    TO_BlendCurrentAlpha,
    TO_PreModulate,
    TO_ModulateAlpha_AddColor,
    TO_ModulateColor_AddAlpha,
    TO_ModulateInvAlpha_AddColor,
    TO_ModulateInvColor_AddAlpha,
    TO_BumpenvMap,
    TO_BumpenvMapLuminance,
    TO_DotProduct3,
    TO_MultipyAdd,
    TO_Lerp,
};

enum TextureArgValue
{
    TAV_Constant,
    TAV_Diffuse,
    TAV_SelectMask,
    TAV_Specular,
    TAV_Temp,
    TAV_Texture,
    TAV_TFactor,
    TAV_Current,
    TAV_Complement,
    TAV_AlphaReplicate,
};

enum CullMode
{
    CM_None,
    CM_CW,
    CM_CCW,
};

enum TextureSamplerType
{
    TST_AddressU,
    TST_AddressV,
    TST_AddressW,
    TST_BorderColor,
    TST_Magfilter,
    TST_Minfilter,
    TST_Mipfilter,
    TST_MipmapLodbias,
    TST_MaxMiplevel,
    TST_MaxAnisotropy,
    TST_SRGBTexture,
    TST_ElementIndex,
    TST_DMapOffset,
};

enum TextureAddressValue
{
    TAV_Warp,
    TAV_Mirror,
    TAV_Clamp,
    TAV_Border,
    TAV_Mirroronce,
};

enum TextureFilterOption
{
    TFT_None,
    TFT_Point,
    TFT_Linear,
    TFT_Anisoropic,
    TFT_Pyramidaquad,
    TFT_Gaussianquad,
    TFT_Convolutionmono,
	TFT_Comparison_Point,
	TFT_Comparison_Linear,
	TFT_Comparison_Anisoropic,
};

enum ShadeOption
{
    eSO_Flat,
    eSO_Gouraud,
    eSO_Phong,
};

enum StencilOperation
{
    /// Leave the stencil buffer unchanged
    eSOP_KEEP,
    /// Set the stencil value to zero
    eSOP_ZERO,
    /// Set the stencil value to the reference value
    eSOP_REPLACE,
    /// Increase the stencil value by 1, clamping at the maximum value
    eSOP_INCREMENT,
    /// Decrease the stencil value by 1, clamping at 0
    eSOP_DECREMENT,
    /// Increase the stencil value by 1, wrapping back to 0 when incrementing the maximum value
    eSOP_INCREMENT_WRAP,
    /// Decrease the stencil value by 1, wrapping when decrementing 0
    eSOP_DECREMENT_WRAP,
    /// Invert the bits of the stencil buffer
    eSOP_INVERT
};

enum CompareFunction
{
    eCF_ALWAYS_FAIL,
    eCF_ALWAYS_PASS,
    eCF_LESS,
    eCF_LESS_EQUAL,
    eCF_EQUAL,
    eCF_NOT_EQUAL,
    eCF_GREATER_EQUAL,
    eCF_GREATER
};

enum MaterialConstantType
{
    eMCT_FLOAT,
    eMCT_INT,
    eMCT_Sampler,
};

enum VARIANT_T
{
    VAR_NULL     = 0,
    VAR_BOOL     = 1,
    VAR_INT      = 2,
    VAR_FLOAT    = 3,
    VAR_FLOAT2   = 4,
    VAR_FLOAT3   = 5,
    VAR_FLOAT4   = 6,
    VAR_FLOAT4X4 = 7,
    VAR_TEXTURE  = 8,
    VAR_POINTER  = 9,
};

enum eRenderFlow
{
    FLOW_SCENE = 1,	
    FLOW_REFLECTION = 2,
    FLOW_RETRACTION = 4,  //折射
    FLOW_OCCLUDE = 8,				
    FLOW_SHADOWMAP = 16,
    FLOW_GLOW = 32,
    FLOW_LIGHTBUFFER = 64,
    FLOW_GBUFFER = 128,
    FLOW_COLOR = 256,
    FLOW_OVERDRAW = 512,
	FLOW_NORMAL = 1024,
    MAX_FLOW
};

using namespace AIR;

class EngineExport EngineParamSetting
{
public:
    // 渲染状态MAP表
    typedef AIRMap< AIRString, u32 > MAP_RENDER_PARM;
    // 反向查找渲染状态名字
    typedef AIRMap< u32, AIRString > MAP_RENDER_NAME;

    EngineParamSetting() {}
    ~EngineParamSetting() {}

    static u32 GetEngineRenderParam(const AIRString& strName)
    {
        if (m_mapRenderParm.size() == 0)
        {
            GenRenderParmMap();
        }

        MAP_RENDER_PARM::iterator it = m_mapRenderParm.find(strName);
        if (it == m_mapRenderParm.end())
        {
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
            assert(false);
#endif
            return 0;
        }
        return it->second;
    }

	//get the pixel size in bytes
	static u8 GetPixelFormatSize(RendererPixelFormat pixfmt)
	{
		switch (pixfmt)
		{
		case RPF_A8R8G8B8:
		case RPF_X8R8G8B8:
		case RPF_D24S8:
		case RPF_R32F:
			return 4;
		case RPF_L16:
		case RPF_R16F:
		case RPF_A8L8:
		case RPF_R4G4B4A4:
			return 2;
		case RPF_R8G8B8:
			return 3;
		case RPF_A16B16G16R16F:
		case RPF_G32R32F:
			return 8;
		case RPF_A32B32G32R32F:
			return 16;
		case RPF_L8:
			return 1;
		case  RPF_ALPHA:
			return 1;
		}
		return 4;
	}

    static VARIANT_T GetVariantType(const char* szName)
    {
        if (strcmp(szName, "VAR_FLOAT") == 0)
        {
            return VAR_FLOAT;
        }
        else if (strcmp(szName, "VAR_BOOL") == 0)
        {
            return VAR_BOOL;
        }
        else if (strcmp(szName, "VAR_INT") == 0)
        {
            return VAR_INT;
        }
        else if (strcmp(szName, "VAR_FLOAT2") == 0)
        {
            return VAR_FLOAT2;
        }
        else if (strcmp(szName, "VAR_FLOAT3") == 0)
        {
            return VAR_FLOAT3;
        }
        else if (strcmp(szName, "VAR_FLOAT4") == 0)
        {
            return VAR_FLOAT4;
        }
        else if (strcmp(szName, "VAR_FLOAT4X4") == 0)
        {
            return VAR_FLOAT4X4;
        }
        else if (strcmp(szName, "VAR_TEXTURE") == 0)
        {
            return VAR_TEXTURE;
        }

        return VAR_NULL;
    }

	static MaterialConstantType VariantTypeToMtlType(VARIANT_T vType)
	{
		switch (vType)
		{
		case VAR_FLOAT:
		case VAR_FLOAT2:
		case VAR_FLOAT3:
		case VAR_FLOAT4:
		case VAR_FLOAT4X4:
		case VAR_BOOL:
			return eMCT_FLOAT;
		case VAR_TEXTURE:
			return eMCT_Sampler;
		case VAR_INT:
			return eMCT_INT;
		}

		return eMCT_FLOAT;
	}

    static eRenderFlow GetRenderFlow(const char* szFlowName)
    {
        if (strcmp(szFlowName, "FLOW_SCENE") == 0)
        {
            return FLOW_SCENE;
        }
        else if (strcmp(szFlowName, "FLOW_REFLECTION") == 0)
        {
            return FLOW_REFLECTION;
        }
        else if (strcmp(szFlowName, "FLOW_OCCLUDE") == 0)
        {
            return FLOW_OCCLUDE;
        }
        else if (strcmp(szFlowName, "FLOW_SHADOWMAP") == 0)
        {
            return FLOW_SHADOWMAP;
        }
        else if (strcmp(szFlowName, "FLOW_GLOW") == 0)
        {
            return FLOW_GLOW;
        }
        else if (strcmp(szFlowName, "FLOW_LIGHTBUFFER") == 0)
        {
            return FLOW_LIGHTBUFFER;
        }
        else if (strcmp(szFlowName, "FLOW_GBUFFER") == 0)
        {
            return FLOW_GBUFFER;
        }
        else if (strcmp(szFlowName, "FLOW_COLOR") == 0)
        {
            return FLOW_COLOR;
        }

        return FLOW_SCENE;
    }

protected:
private:

    static void GenRenderParmMap()
    {
        //shade option
        m_mapRenderParm.insert( std::make_pair( "SHADE_FLAT",	eSO_Flat) );
        m_mapRenderParm.insert( std::make_pair( "SHADE_GOURAUD", eSO_Gouraud) );
        m_mapRenderParm.insert( std::make_pair( "SHADE_PHONG",	eSO_Phong) );

        //fill mode
        m_mapRenderParm.insert( std::make_pair( "FILL_POINT", RSFM_Point) );
        m_mapRenderParm.insert( std::make_pair( "FILL_WIREFRAME", RSFM_WireFrame) );
        m_mapRenderParm.insert( std::make_pair( "FILL_SOLID", RSFM_Solid) );

        //blend value
        m_mapRenderParm.insert( std::make_pair( "BLEND_ZERO", RSRV_Zero) );
        m_mapRenderParm.insert( std::make_pair( "BLEND_ONE",	RSRV_One) );
        m_mapRenderParm.insert( std::make_pair( "BLEND_SRCCOLOR", RSRV_SrcColor) );
        m_mapRenderParm.insert( std::make_pair( "BLEND_INVSRCCOLOR", RSRV_InvSrcColor) );
        m_mapRenderParm.insert( std::make_pair( "BLEND_SRCALPHA", RSRV_SrcAlpha) );
        m_mapRenderParm.insert( std::make_pair( "BLEND_INVSRCALPHA", RSRV_InvSrcAlpha) );
        m_mapRenderParm.insert( std::make_pair( "BLEND_DESTALPHA", RSRV_DestAlpha) );
        m_mapRenderParm.insert( std::make_pair( "BLEND_INVDESTALPHA", RSRV_InvDestAlpha) );
        m_mapRenderParm.insert( std::make_pair( "BLEND_DESTCOLOR",	RSRV_DestColor) );
        m_mapRenderParm.insert( std::make_pair( "BLEND_INVDESTCOLOR",	RSRV_InvDestColor) );
        m_mapRenderParm.insert( std::make_pair( "BLEND_SRCALPHASAT", RSRV_SrcAlphaSat) );
        m_mapRenderParm.insert( std::make_pair( "BLEND_BOTHSRCALPHA",	RSRV_BothSrcAlpha) );
        m_mapRenderParm.insert( std::make_pair( "BLEND_BOTHINVSRCALPHA", RSRV_BothInvSrcAlpha) );
        m_mapRenderParm.insert( std::make_pair( "BLEND_BLENDFACTOR",	RSRV_BlendFactor) );
        m_mapRenderParm.insert( std::make_pair( "BLEND_INVBLENDFACTOR",	RSRV_InvBlendFactor) );
        m_mapRenderParm.insert( std::make_pair( "BLEND_SRCCOLOR2",  RSRV_SrcColor2) );
        m_mapRenderParm.insert( std::make_pair( "BLEND_INVSRCCOLOR2",	RSRV_InvSrcColor2) );

        //blend option混合方式
        m_mapRenderParm.insert( std::make_pair( "BLENDOP_ADD",			BO_Add) );
        m_mapRenderParm.insert( std::make_pair( "BLENDOP_SUBTRACT",		BO_Subtract) );
        m_mapRenderParm.insert( std::make_pair( "BLENDOP_REVSUBTRACT",	BO_RevSubtract) );
        m_mapRenderParm.insert( std::make_pair( "BLENDOP_MIN",			BO_Min) );
        m_mapRenderParm.insert( std::make_pair( "BLENDOP_MAX",			BO_Max) );

        //texture address mode
        m_mapRenderParm.insert( std::make_pair( "TADDRESS_WRAP",	 TAV_Warp) );
        m_mapRenderParm.insert( std::make_pair( "TADDRESS_MIRROR",	TAV_Mirror) );
        m_mapRenderParm.insert( std::make_pair( "TADDRESS_CLAMP",	 TAV_Clamp) );
        m_mapRenderParm.insert( std::make_pair( "TADDRESS_BORDER",	 TAV_Border) );
        m_mapRenderParm.insert( std::make_pair( "TADDRESS_MIRRORONCE",	TAV_Mirroronce) );

        //cull mode
        m_mapRenderParm.insert( std::make_pair( "CULL_NONE",	 CM_None) );
        m_mapRenderParm.insert( std::make_pair( "CULL_CW",		CM_CW) );
        m_mapRenderParm.insert( std::make_pair( "CULL_CCW",		CM_CCW) );

        //fog mode
        m_mapRenderParm.insert( std::make_pair( "FOG_NONE",		eFOG_NONE) );
        m_mapRenderParm.insert( std::make_pair( "FOG_EXP",		eFOG_EXP) );
        m_mapRenderParm.insert( std::make_pair( "FOG_EXP2",		eFOG_EXP2) );
        m_mapRenderParm.insert( std::make_pair( "FOG_LINEAR",	eFOG_LINEAR) );

        //texture operation
        m_mapRenderParm.insert( std::make_pair( "TSS_COLOROP",		TSST_ColorOp) );
        m_mapRenderParm.insert( std::make_pair( "TSS_COLORARG1",	TSST_ColorArg1) );
        m_mapRenderParm.insert( std::make_pair( "TSS_COLORARG2",	TSST_ColorArg2) );
        m_mapRenderParm.insert( std::make_pair( "TSS_ALPHAOP",		TSST_AlphaOp) );
        m_mapRenderParm.insert( std::make_pair( "TSS_ALPHAARG1",	TSST_AlphaArg1) );
        m_mapRenderParm.insert( std::make_pair( "TSS_ALPHAARG2",	TSST_AlphaArg2) );
        m_mapRenderParm.insert( std::make_pair( "TSS_BUMPENVMAT00",	TSST_BumpenvMat00) );
        m_mapRenderParm.insert( std::make_pair( "TSS_BUMPENVMAT01",	TSST_BumpenvMat01) );
        m_mapRenderParm.insert( std::make_pair( "TSS_BUMPENVMAT10",	TSST_BumpenvMat10) );
        m_mapRenderParm.insert( std::make_pair( "TSS_BUMPENVMAT11",	TSST_BumpenvMat11) );
        m_mapRenderParm.insert( std::make_pair( "TSS_TEXCOORDINDEX", TSST_TexcoordIndex) );
        m_mapRenderParm.insert( std::make_pair( "TSS_BUMPENVLSCALE", TSST_BumpenvlScale) );
        m_mapRenderParm.insert( std::make_pair( "TSS_BUMPENVLOFFSET",	TSST_BumpenvlOffset ) );
        m_mapRenderParm.insert( std::make_pair( "TSS_TEXTURETRANSFORMFLAGS", TSST_TextureTransformFlags) );
        m_mapRenderParm.insert( std::make_pair( "TSS_COLORARG0",	TSST_ColorArg0) );
        m_mapRenderParm.insert( std::make_pair( "TSS_ALPHAARG0",	TSST_AlphaArg0) );
        m_mapRenderParm.insert( std::make_pair( "TSS_RESULTARG",	TSST_ResultArg) );
        m_mapRenderParm.insert( std::make_pair( "TSS_CONSTANT",		TSST_Constant) );

        //texture sampler value
        m_mapRenderParm.insert( std::make_pair( "SAMP_ADDRESSU", TST_AddressU) );
        m_mapRenderParm.insert( std::make_pair( "SAMP_ADDRESSV", TST_AddressV) );
        m_mapRenderParm.insert( std::make_pair( "SAMP_ADDRESSW", TST_AddressW) );
        m_mapRenderParm.insert( std::make_pair( "SAMP_BORDERCOLOR", TST_BorderColor) );
        m_mapRenderParm.insert( std::make_pair( "SAMP_MAGFILTER", TST_Magfilter) );
        m_mapRenderParm.insert( std::make_pair( "SAMP_MINFILTER", TST_Minfilter) );
        m_mapRenderParm.insert( std::make_pair( "SAMP_MIPFILTER", TST_Mipfilter) );
        m_mapRenderParm.insert( std::make_pair( "SAMP_MIPMAPLODBIAS", TST_MipmapLodbias) );
        m_mapRenderParm.insert( std::make_pair( "SAMP_MAXMIPLEVEL", TST_MaxMiplevel) );
        m_mapRenderParm.insert( std::make_pair( "SAMP_MAXANISOTROPY",	TST_MaxAnisotropy) );
        m_mapRenderParm.insert( std::make_pair( "SAMP_SRGBTEXTURE",		TST_SRGBTexture) );
        m_mapRenderParm.insert( std::make_pair( "SAMP_ELEMENTINDEX",	TST_ElementIndex) );
        m_mapRenderParm.insert( std::make_pair( "SAMP_DMAPOFFSET",		TST_DMapOffset) );

        /*
        m_mapRenderParm.insert( std::make_pair( "DMAPSAMPLER",			D3DDMAPSAMPLER) );
        m_mapRenderParm.insert( std::make_pair( "VERTEXTEXTURESAMPLER0", D3DVERTEXTEXTURESAMPLER0) );
        m_mapRenderParm.insert( std::make_pair( "VERTEXTEXTURESAMPLER1", D3DVERTEXTEXTURESAMPLER1) );
        m_mapRenderParm.insert( std::make_pair( "VERTEXTEXTURESAMPLER2", D3DVERTEXTEXTURESAMPLER2) );
        m_mapRenderParm.insert( std::make_pair( "VERTEXTEXTURESAMPLER3", D3DVERTEXTEXTURESAMPLER3) );

        m_mapRenderParm.insert( std::make_pair( "TSS_TCI_PASSTHRU",						D3DTSS_TCI_PASSTHRU) );
        m_mapRenderParm.insert( std::make_pair( "TSS_TCI_CAMERASPACENORMAL",				D3DTSS_TCI_CAMERASPACENORMAL) );
        m_mapRenderParm.insert( std::make_pair( "TSS_TCI_CAMERASPACEPOSITION",			D3DTSS_TCI_CAMERASPACEPOSITION) );
        m_mapRenderParm.insert( std::make_pair( "TSS_TCI_CAMERASPACEREFLECTIONVECTOR",	D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR) );
        m_mapRenderParm.insert( std::make_pair( "TSS_TCI_SPHEREMAP",						D3DTSS_TCI_SPHEREMAP) );
        */

        //texture operation type
        m_mapRenderParm.insert( std::make_pair( "TOP_DISABLE",		TO_Disable) );
        m_mapRenderParm.insert( std::make_pair( "TOP_SELECTARG1",	TO_SelectArg1) );
        m_mapRenderParm.insert( std::make_pair( "TOP_SELECTARG2",	TO_SelectArg2) );
        m_mapRenderParm.insert( std::make_pair( "TOP_MODULATE",		TO_Modulate) );
        m_mapRenderParm.insert( std::make_pair( "TOP_MODULATE2X",	TO_Modulate2x) );
        m_mapRenderParm.insert( std::make_pair( "TOP_MODULATE4X",	TO_Modulate4x) );
        m_mapRenderParm.insert( std::make_pair( "TOP_ADD",			TO_Add) );
        m_mapRenderParm.insert( std::make_pair( "TOP_ADDSIGNED",	TO_AddSigned) );
        m_mapRenderParm.insert( std::make_pair( "TOP_ADDSIGNED2X",	TO_AddSigned2x) );
        m_mapRenderParm.insert( std::make_pair( "TOP_SUBTRACT",		TO_Subtract) );
        m_mapRenderParm.insert( std::make_pair( "TOP_ADDSMOOTH",	TO_AddSmooth) );
        m_mapRenderParm.insert( std::make_pair( "TOP_BLENDDIFFUSEALPHA", TO_BlendDiffuseAlpha) );
        m_mapRenderParm.insert( std::make_pair( "TOP_BLENDTEXTUREALPHA", TO_BlendTextureAlpha) );
        m_mapRenderParm.insert( std::make_pair( "TOP_BLENDFACTORALPHA",	 TO_BlendFactorAlpha) );
        m_mapRenderParm.insert( std::make_pair( "TOP_BLENDTEXTUREALPHAPM",	TO_BlendTextureAlphaPM) );
        m_mapRenderParm.insert( std::make_pair( "TOP_BLENDCURRENTALPHA",	TO_BlendCurrentAlpha) );
        m_mapRenderParm.insert( std::make_pair( "TOP_PREMODULATE",		TO_PreModulate) );
        m_mapRenderParm.insert( std::make_pair( "TOP_MODULATEALPHA_ADDCOLOR",	TO_ModulateAlpha_AddColor) );
        m_mapRenderParm.insert( std::make_pair( "TOP_MODULATECOLOR_ADDALPHA",	TO_ModulateColor_AddAlpha) );
        m_mapRenderParm.insert( std::make_pair( "TOP_MODULATEINVALPHA_ADDCOLOR", TO_ModulateInvAlpha_AddColor) );
        m_mapRenderParm.insert( std::make_pair( "TOP_MODULATEINVCOLOR_ADDALPHA", TO_ModulateInvColor_AddAlpha) );
        m_mapRenderParm.insert( std::make_pair( "TOP_BUMPENVMAP",	TO_BumpenvMap) );
        m_mapRenderParm.insert( std::make_pair( "TOP_BUMPENVMAPLUMINANCE",	TO_BumpenvMapLuminance) );
        m_mapRenderParm.insert( std::make_pair( "TOP_DOTPRODUCT3",	TO_DotProduct3) );
        m_mapRenderParm.insert( std::make_pair( "TOP_MULTIPLYADD",	TO_MultipyAdd) );
        m_mapRenderParm.insert( std::make_pair( "TOP_LERP",	 TO_Lerp) );

        //texture arg values
        m_mapRenderParm.insert( std::make_pair( "TA_SELECTMASK", TAV_SelectMask) );
        m_mapRenderParm.insert( std::make_pair( "TA_DIFFUSE",	TAV_Diffuse) );
        m_mapRenderParm.insert( std::make_pair( "TA_CURRENT",	TAV_Current) );
        m_mapRenderParm.insert( std::make_pair( "TA_TEXTURE",	TAV_Texture) );
        m_mapRenderParm.insert( std::make_pair( "TA_TFACTOR",	TAV_TFactor) );
        m_mapRenderParm.insert( std::make_pair( "TA_SPECULAR",	TAV_Specular) );
        m_mapRenderParm.insert( std::make_pair( "TA_TEMP",		TAV_Temp) );
        m_mapRenderParm.insert( std::make_pair( "TA_CONSTANT",	TAV_Constant) );
        m_mapRenderParm.insert( std::make_pair( "TA_COMPLEMENT",	TAV_Complement) );
        m_mapRenderParm.insert( std::make_pair( "TA_ALPHAREPLICATE", TAV_AlphaReplicate) );

        //texture filter type
        m_mapRenderParm.insert( std::make_pair( "TEXF_NONE", TFT_None) );
        m_mapRenderParm.insert( std::make_pair( "TEXF_POINT", TFT_Point) );
        m_mapRenderParm.insert( std::make_pair( "TEXF_LINEAR", TFT_Linear) );
        m_mapRenderParm.insert( std::make_pair( "TEXF_ANISOTROPIC", TFT_Anisoropic) );
        m_mapRenderParm.insert( std::make_pair( "TEXF_PYRAMIDALQUAD", TFT_Pyramidaquad) );
        m_mapRenderParm.insert( std::make_pair( "TEXF_GAUSSIANQUAD", TFT_Gaussianquad) );
        m_mapRenderParm.insert( std::make_pair( "TEXF_CONVOLUTIONMONO",	TFT_Convolutionmono) );

        //m_mapRenderParm.insert( std::make_pair( "FMT_UNKNOWN",	D3DFMT_UNKNOWN) );
        //pixel format
        m_mapRenderParm.insert( std::make_pair( "FMT_R8G8B8",	RPF_R8G8B8) );
        m_mapRenderParm.insert( std::make_pair( "FMT_A8R8G8B8",	RPF_A8R8G8B8) );
        m_mapRenderParm.insert( std::make_pair( "FMT_X8R8G8B8",	RPF_X8R8G8B8) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_R5G6B5",	RPF_R5G6B5) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_X1R5G5B5",	RPF_X1R5G5B5) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_A1R5G5B5",	RPF_A1R5G5B5) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_A4R4G4B4",	RPF_A4R4G4B4) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_R3G3B2",	RPF_R3G3B2) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_A8",		RPF_A8) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_A8R3G3B2",	RPF_A8R3G3B2) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_X4R4G4B4",	RPF_X4R4G4B4) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_A2B10G10R10",	RPF_A2B10G10R10) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_A8B8G8R8",		RPF_A8B8G8R8) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_X8B8G8R8",		RPF_X8B8G8R8) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_G16R16",		RPF_G16R16) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_A2R10G10B10",	RPF_A2R10G10B10 ) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_A16B16G16R16",	RPF_A16B16G16R16) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_A8P8",	RPF_A8P8) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_P8",	RPF_P8) );
        m_mapRenderParm.insert( std::make_pair( "FMT_L8",	RPF_L8) );
        m_mapRenderParm.insert( std::make_pair( "FMT_A8L8",	RPF_A8L8) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_A4L4",	RPF_A4L4) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_V8U8",	RPF_V8U8) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_L6V5U5",	RPF_L6V5U5) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_X8L8V8U8",	RPF_X8L8V8U8) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_Q8W8V8U8",	RPF_Q8W8V8U8) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_V16U16",		RPF_V16U16) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_A2W10V10U10",	RPF_A2W10V10U10) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_UYVY",		RPF_UYVY) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_R8G8_B8G8", RPF_R8G8_B8G8) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_YUY2",		RPF_YUY2) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_G8R8_G8B8", RPF_G8R8_G8B8) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_DXT1",		RPF_DXT1) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_DXT2",		RPF_DXT2) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_DXT3",		RPF_DXT3) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_DXT4",		RPF_DXT4) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_DXT5",		RPF_DXT5) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_D16_LOCKABLE",	RPF_D16_LOCKABLE) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_D32",			RPF_D32) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_D15S1", RPF_D15S1) );
        m_mapRenderParm.insert( std::make_pair( "FMT_D24S8", RPF_D24S8) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_D24X8", RPF_D24X8) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_D24X4S4",		RPF_D24X4S4) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_D16",			RPF_D16) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_D32F_LOCKABLE", RPF_D32F_LOCKABLE) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_D24FS8",		RPF_D24FS8) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_D32_LOCKABLE",	RPF_D32_LOCKABLE) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_S8_LOCKABLE",	RPF_S8_LOCKABLE) );
        m_mapRenderParm.insert( std::make_pair( "FMT_L16",			RPF_L16) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_VERTEXDATA",	RPF_VERTEXDATA) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_INDEX16",		RPF_INDEX16) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_INDEX32",		RPF_INDEX32) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_Q16W16V16U16",	RPF_Q16W16V16U16 ) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_MULTI2_ARGB8",	RPF_MULTI2_ARGB8) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_CxV8U8",		RPF_CxV8U8) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_A1",			RPF_A1) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_BINARYBUFFER",	RPF_BINARYBUFFER) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_G16R16F",		RPF_G16R16F) );
        //m_mapRenderParm.insert( std::make_pair( "FMT_G32R32F",		RPFT_G32R32F) );
        m_mapRenderParm.insert( std::make_pair( "FMT_R16F",			RPF_R16F) );
        m_mapRenderParm.insert( std::make_pair( "FMT_A16B16G16R16F", RPF_A16B16G16R16F) );
        m_mapRenderParm.insert( std::make_pair( "FMT_R32F",			RPF_R32F) );
        m_mapRenderParm.insert( std::make_pair( "FMT_A32B32G32R32F", RPF_A32B32G32R32F) );
		m_mapRenderParm.insert( std::make_pair( "FMT_ALPHA", RPF_ALPHA) );
        

        //comparefunction
        m_mapRenderParm.insert(std::make_pair("eCF_ALWAYS_FAIL", eCF_ALWAYS_FAIL));
        m_mapRenderParm.insert(std::make_pair("eCF_ALWAYS_PASS", eCF_ALWAYS_PASS));
        m_mapRenderParm.insert(std::make_pair("eCF_LESS", eCF_LESS));
        m_mapRenderParm.insert(std::make_pair("eCF_LESS_EQUAL", eCF_LESS_EQUAL));
        m_mapRenderParm.insert(std::make_pair("eCF_EQUAL", eCF_EQUAL));
        m_mapRenderParm.insert(std::make_pair("eCF_NOT_EQUAL", eCF_NOT_EQUAL));
        m_mapRenderParm.insert(std::make_pair("eCF_GREATER_EQUAL", eCF_GREATER_EQUAL));
        m_mapRenderParm.insert(std::make_pair("eCF_GREATER", eCF_GREATER));
    }

    
    static MAP_RENDER_PARM m_mapRenderParm;
    //MAP_RENDER_NAME m_mapRenderName;
};


