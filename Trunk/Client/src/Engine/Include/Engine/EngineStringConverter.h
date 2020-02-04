#pragma once 

//#include "EngineConfig.h"
#include "EngineDefination.h"
#include "StringUtil.h"
#include "MemoryAllocatorConfig.h"
#include "InternalFixedStrings.h"

class EngineExport EngineStringConverter
{
public:
    typedef AIRMap<AIRString, u32> MapString;
    typedef MapString::iterator MapStringIter;
public:
    EngineStringConverter();
    ~EngineStringConverter();

    static AIRString GetTextureStateTypeName()
    {
        return CStringUtil::BLANK;
    }

    static AIRString GetTextureSamplerTypeString(TextureSamplerType type)
    {
        return CStringUtil::BLANK;
    }

    static AIRString GetPixelFormatString(RendererPixelFormat pixelFormat)
    {
        switch (pixelFormat)
        {
        case RPF_A8R8G8B8:
            return "FMT_A8R8G8B8";
        case RPF_X8R8G8B8:
            return "FMT_X8R8G8B8";
        case RPF_D24S8:
            return "FMT_D24S8";
        case RPF_R32F:
            return "FMT_R32F";
        case RPF_L8:
            return "FMT_L8";
        case RPF_L16:
            return "FMT_L16";
        case RPF_R8G8B8:
            return "FMT_R8G8B8";
        case RPF_R16F:
            return "FMT_R16F";
        case RPF_A16B16G16R16F:
            return "FMT_A16B16G16R16F";
        case RPF_G32R32F:
            return "FMT_G32R32F";
        case RPF_A32B32G32R32F:
            return "FMT_A32B32G32R32F";
        case RPF_A8L8:
            return "FMT_A8L8";
		case RPF_ALPHA:
			return "FMT_ALPHA";
        }
        return CStringUtil::BLANK;
    }

    static AIRString GetBlendOptString(BlendOpt opt)
    {
        switch (opt)
        {
        case BO_Add:
            return "BLENDOP_ADD";
        case BO_Subtract:
            return "BLENDOP_SUBTRACT";
        case BO_RevSubtract:
            return "BLENDOP_REVSUBTRACT";
        case BO_Min:
            return "BLENDOP_MIN";
        case BO_Max:
            return "BLENDOP_MAX";
        }

        return CStringUtil::BLANK;
    }

    static AIRString GetBlendValueString(RenderStateBlendValue blend)
    {
        switch (blend)
        {
        case RSRV_Zero:
            return "BLEND_ZERO";
        case RSRV_One:
            return "BLEND_ONE";
        case RSRV_SrcColor:
            return "BLEND_SRCCOLOR";
        case RSRV_InvSrcColor:
            return "BLEND_INVSRCCOLOR";
        case RSRV_SrcAlpha:
            return "BLEND_SRCALPHA";
        case RSRV_InvSrcAlpha:
            return "BLEND_INVSRCALPHA";
        case RSRV_DestColor:
            return "BLEND_DESTCOLOR";
        case RSRV_InvDestColor:
            return "BLEND_INVDESTCOLOR";
        case RSRV_SrcAlphaSat:
            return "BLEND_SRCALPHASAT";
        case RSRV_BothSrcAlpha:
            return "BLEND_BOTHSRCALPHA";
        case RSRV_BothInvSrcAlpha:
            return "BLEND_BOTHINVSRCALPHA";
        case RSRV_BlendFactor:
            return "BLEND_BLENDFACTOR";
        case RSRV_InvBlendFactor:
            return "BLEND_INVBLENDFACTOR";
        case RSRV_SrcColor2:
            return "BLEND_SRCCOLOR2";
        case RSRV_InvSrcColor2:
            return "BLEND_INVSRCCOLOR2";
        case RSRV_DestAlpha:
            return "BLEND_DESTALPHA";
        case RSRV_InvDestAlpha:
            return "BLEND_INVDESTALPHA";
        }

        return CStringUtil::BLANK;
    }

    static AIRString GetTextureAddressString(TextureAddressValue value)
    {
        switch (value)
        {
        case TAV_Warp:
            return "TADDRESS_WRAP";
        case TAV_Mirror:
            return "TADDRESS_MIRROR";
        case TAV_Clamp:
            return "TADDRESS_CLAMP";
        case TAV_Border:
            return "TADDRESS_BORDER";
        case TAV_Mirroronce:
            return "TADDRESS_MIRRORONCE";
        }
        return CStringUtil::BLANK;
    }

    static TextureAddressValue GetTextureAddress(const FixedString& strName)
    {
        if (strName == *FS_TADDRESS_WRAP)
        {
            return TAV_Warp;
        }
        else if (strName == *FS_TADDRESS_MIRROR)
        {
            return TAV_Mirror;
        }
        else if (strName == *FS_TADDRESS_CLAMP)
        {
            return TAV_Clamp;
        }
        else if (strName == *FS_TADDRESS_BORDER)
        {
            return TAV_Border;
        }
        else if (strName == *FS_TADDRESS_MIRRORONCE)
        {
            return TAV_Mirroronce;
        }

        return TAV_Warp;
    }

    static AIRString GetTextureFilterOptionString(TextureFilterOption opt)
    {
        switch (opt)
        {
        case TFT_None:
            return "TEXF_NONE";
        case TFT_Point:
            return "TEXF_POINT";
        case TFT_Linear:
            return "TEXF_LINEAR";
        case TFT_Anisoropic:
            return "TEXF_ANISOTROPIC";
        case TFT_Pyramidaquad:
            return "TEXF_PYRAMIDALQUAD";
        case TFT_Gaussianquad:
            return "TEXF_GAUSSIANQUAD";
        case TFT_Convolutionmono:
            return "TEXF_CONVOLUTIONMONO";
        }
        return CStringUtil::BLANK;
    }

    static TextureFilterOption GetTextureFilterOption(const FixedString& strName)
    {
        if (strName == *FS_TEXF_NONE)
        {
            return TFT_None;
        }
        else if (strName == *FS_TEXF_POINT)
        {
            return TFT_Point;
        }
        else if (strName == *FS_TEXF_LINEAR)
        {
            return TFT_Linear;
        }
        else if (strName == *FS_TEXF_ANISOTROPIC)
        {
            return TFT_Anisoropic;
        }
        else if (strName == *FS_TEXF_PYRAMIDALQUAD)
        {
            return TFT_Pyramidaquad;
        }
        else if (strName == *FS_TEXF_GAUSSIANQUAD)
        {
            return TFT_Gaussianquad;
        }
        else if (strName == *FS_TEXF_CONVOLUTIONMONO)
        {
            return TFT_Convolutionmono;
        }

        return TFT_Point;
    }

    static AIRString GetTextureOpString(TextureOp op)
    {
        switch (op)
        {
        case TO_Disable:
            return "TOP_DISABLE";
        case TO_SelectArg1:
            return "TOP_SELECTARG1";
        case TO_SelectArg2:
            return "TOP_SELECTARG2";
        case TO_Modulate:
            return "TOP_MODULATE";
        case TO_Modulate2x:
            return "TOP_MODULATE2X";
        case TO_Modulate4x:
            return "TOP_MODULATE4X";
        case TO_Add:
            return "TOP_ADD";
        case TO_AddSigned:
            return "TOP_ADDSIGNED";
        case TO_AddSigned2x:
            return "TOP_ADDSIGNED2X";
        case TO_Subtract:
            return "TOP_SUBTRACT";
        case TO_AddSmooth:
            return "TOP_ADDSMOOTH";
        case TO_BlendDiffuseAlpha:
            return "TOP_BLENDDIFFUSEALPHA";
        case TO_BlendTextureAlpha:
            return "TOP_BLENDTEXTUREALPHA";
        case TO_BlendFactorAlpha:
            return "TOP_BLENDFACTORALPHA";
        case TO_BlendTextureAlphaPM:
            return "TOP_BLENDTEXTUREALPHAPM";
        case TO_BlendCurrentAlpha:
            return "TOP_BLENDCURRENTALPHA";
        case TO_PreModulate:
            return "TOP_PREMODULATE";
        case TO_ModulateAlpha_AddColor:
            return "TOP_MODULATEALPHA_ADDCOLOR";
        case TO_ModulateColor_AddAlpha:
            return "TOP_MODULATECOLOR_ADDALPHA";
        case TO_ModulateInvAlpha_AddColor:
            return "TOP_MODULATEINVALPHA_ADDCOLOR";
        case TO_ModulateInvColor_AddAlpha:
            return "TOP_MODULATEINVCOLOR_ADDALPHA";
        case TO_BumpenvMap:
            return "TOP_BUMPENVMAP";
        case TO_BumpenvMapLuminance:
            return "TOP_BUMPENVMAPLUMINANCE";
        case TO_DotProduct3:
            return "TOP_DOTPRODUCT3";
        case TO_MultipyAdd:
            return "TOP_MULTIPLYADD";
        case TO_Lerp:
            return "TOP_LERP";
        }

        return CStringUtil::BLANK;
    }

    static AIRString GetTextureArgString(TextureArgValue value)
    {
        switch (value)
        {
        case TAV_Constant:
            return "TA_CONSTANT";
        case TAV_Diffuse:
            return "TA_DIFFUSE";
        case TAV_SelectMask:
            return "TA_SELECTMASK";
        case TAV_Specular:
            return "TA_SPECULAR";
        case TAV_Temp:
            return "TA_TEMP";
        case TAV_Texture:
            return "TA_TEXTURE";
        case TAV_TFactor:
            return "TA_TFACTOR";
        case TAV_Current:
            return "TA_CURRENT";
        }

        return CStringUtil::BLANK;
    }

    static AIRString GetFillModeString(RenderSystemFillMode fillMode)
    {
        switch (fillMode)
        {
        case RSFM_Point:
            return "FILL_POINT";
        case RSFM_WireFrame:
            return "FILL_WIREFRAME";
        case RSFM_Solid:
            return "FILL_SOLID";
        }
        return CStringUtil::BLANK;
    }

    static AIRString GetCullModeString(CullMode cullMode)
    {
        switch (cullMode)
        {
        case CM_CCW:
            return "CULL_CCW";
        case CM_CW:
            return "CULL_CW";
        case CM_None:
            return "CULL_NONE";
        }
        return CStringUtil::BLANK;
    }

    static AIRString GetCompareFuncString(CompareFunction func)
    {
        switch (func)
        {
        case eCF_ALWAYS_FAIL:
            return "eCF_ALWAYS_FAIL";
        case eCF_ALWAYS_PASS:
            return "eCF_ALWAYS_PASS";
        case eCF_LESS:
            return "eCF_LESS";
        case eCF_LESS_EQUAL:
            return "eCF_LESS_EQUAL";
        case eCF_EQUAL:
            return "eCF_EQUAL";
        case eCF_NOT_EQUAL:
            return "eCF_NOT_EQUAL";
        case eCF_GREATER_EQUAL:
            return "eCF_GREATER_EQUAL";
        case eCF_GREATER:
            return "eCF_GREATER";
        }

        return CStringUtil::BLANK;
    }

    static u32 GetEngineParamByName(const AIRString& strName)
    {
        if (m_mapEngineParam.empty())
        {
            RegisterAllParams();
        }
        MapStringIter it = m_mapEngineParam.find(strName);
        if (it != m_mapEngineParam.end())
        {
            return it->second;
        }

        return 0;
    }

    static void RegisterAllParams();
protected:
private:
    
    static MapString m_mapEngineParam;
};
