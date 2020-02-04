#include "stdafx.h"
#include "EngineStringConverter.h"
#include "VertexDeclaration.h"

EngineStringConverter::MapString EngineStringConverter::m_mapEngineParam;

EngineStringConverter::EngineStringConverter()
{

}

EngineStringConverter::~EngineStringConverter()
{

}

void EngineStringConverter::RegisterAllParams()
{
    //pixel format
    m_mapEngineParam.insert(std::make_pair("FMT_A8R8G8B8", RPF_A8R8G8B8));
    m_mapEngineParam.insert(std::make_pair("FMT_R8G8B8", RPF_R8G8B8));
    m_mapEngineParam.insert(std::make_pair("FMT_X8R8G8B8", RPF_X8R8G8B8));
    m_mapEngineParam.insert(std::make_pair("FMT_D24S8", RPF_D24S8));
    m_mapEngineParam.insert(std::make_pair("FMT_L8", RPF_L8));
    m_mapEngineParam.insert(std::make_pair("FMT_L16", RPF_L16));
    m_mapEngineParam.insert(std::make_pair("FMT_R16F", RPF_R16F));
    m_mapEngineParam.insert(std::make_pair("FMT_A16B16G16R16F", RPF_A16B16G16R16F));
    m_mapEngineParam.insert(std::make_pair("FMT_R32F", RPF_R32F));
    m_mapEngineParam.insert(std::make_pair("FMT_G32R32F", RPF_G32R32F));
    m_mapEngineParam.insert(std::make_pair("FMT_A32B32G32R32F", RPF_A32B32G32R32F));
    m_mapEngineParam.insert(std::make_pair("FMT_A8L8", RPF_A8L8));
	m_mapEngineParam.insert(std::make_pair("FMT_ALPHA", RPF_ALPHA));

    //cull mode
    m_mapEngineParam.insert(std::make_pair("CULL_NONE", CM_None));
    m_mapEngineParam.insert(std::make_pair("CULL_CW", CM_CW));
    m_mapEngineParam.insert(std::make_pair("CULL_CCW", CM_CCW));

    //texture filter option
    m_mapEngineParam.insert(std::make_pair("TEXF_NONE", TFT_None));
    m_mapEngineParam.insert(std::make_pair("TEXF_POINT", TFT_Point));
    m_mapEngineParam.insert(std::make_pair("TEXF_LINEAR", TFT_Linear));
    m_mapEngineParam.insert(std::make_pair("TEXF_ANISOTROPIC", TFT_Anisoropic));
    m_mapEngineParam.insert(std::make_pair("TEXF_PYRAMIDALQUAD", TFT_Pyramidaquad));
    m_mapEngineParam.insert(std::make_pair("TEXF_GAUSSIANQUAD", TFT_Gaussianquad));
    m_mapEngineParam.insert(std::make_pair("TEXF_CONVOLUTIONMONO", TFT_Convolutionmono));

    //texture address
    m_mapEngineParam.insert(std::make_pair("TADDRESS_WRAP", TAV_Warp));
    m_mapEngineParam.insert(std::make_pair("TADDRESS_MIRROR", TAV_Mirror));
    m_mapEngineParam.insert(std::make_pair("TADDRESS_CLAMP", TAV_Clamp));
    m_mapEngineParam.insert(std::make_pair("TADDRESS_BORDER", TAV_Border));
    m_mapEngineParam.insert(std::make_pair("TADDRESS_MIRRORONCE", TAV_Mirroronce));

    //texture stage state type
    m_mapEngineParam.insert(std::make_pair("TSS_COLOROP", TSST_ColorOp));
    m_mapEngineParam.insert(std::make_pair("TSS_COLORARG1", TSST_ColorArg1));
    m_mapEngineParam.insert(std::make_pair("TSS_COLORARG2", TSST_ColorArg2));
    m_mapEngineParam.insert(std::make_pair("TSS_ALPHAOP", TSST_AlphaOp));
    m_mapEngineParam.insert(std::make_pair("TSS_ALPHAARG1", TSST_AlphaArg1));
    m_mapEngineParam.insert(std::make_pair("TSS_ALPHAARG2", TSST_AlphaArg2));
    m_mapEngineParam.insert(std::make_pair("TSS_BUMPENVMAT00", TSST_BumpenvMat00));
    m_mapEngineParam.insert(std::make_pair("TSS_BUMPENVMAT01", TSST_BumpenvMat01));
    m_mapEngineParam.insert(std::make_pair("TSS_BUMPENVMAT10", TSST_BumpenvMat10));
    m_mapEngineParam.insert(std::make_pair("TSS_BUMPENVMAT11", TSST_BumpenvMat11));
    m_mapEngineParam.insert(std::make_pair("TSS_TEXCOORDINDEX", TSST_TexcoordIndex));
    m_mapEngineParam.insert(std::make_pair("TSS_BUMPENVLSCALE", TSST_BumpenvlScale));
    m_mapEngineParam.insert(std::make_pair("TSS_BUMPENVLOFFSET", TSST_BumpenvlOffset));
    m_mapEngineParam.insert(std::make_pair("TSS_TEXTURETRANSFORMFLAGS", TSST_TextureTransformFlags));
    m_mapEngineParam.insert(std::make_pair("TSS_COLORARG0", TSST_ColorArg0));
    m_mapEngineParam.insert(std::make_pair("TSS_ALPHAARG0", TSST_AlphaArg0));
    m_mapEngineParam.insert(std::make_pair("TSS_RESULTARG", TSST_ResultArg));
    m_mapEngineParam.insert(std::make_pair("TSS_CONSTANT", TSST_Constant));


    //texture op value
    m_mapEngineParam.insert(std::make_pair("TOP_DISABLE", TO_Disable));
    m_mapEngineParam.insert(std::make_pair("TOP_SELECTARG1", TO_SelectArg1));
    m_mapEngineParam.insert(std::make_pair("TOP_SELECTARG2", TO_SelectArg2));
    m_mapEngineParam.insert(std::make_pair("TOP_MODULATE", TO_Modulate));
    m_mapEngineParam.insert(std::make_pair("TOP_MODULATE2X", TO_Modulate2x));
    m_mapEngineParam.insert(std::make_pair("TOP_MODULATE4X", TO_Modulate4x));
    m_mapEngineParam.insert(std::make_pair("TOP_ADD", TO_Add));
    m_mapEngineParam.insert(std::make_pair("TOP_ADDSIGNED", TO_AddSigned));
    m_mapEngineParam.insert(std::make_pair("TOP_ADDSIGNED2X", TO_AddSigned2x));
    m_mapEngineParam.insert(std::make_pair("TOP_SUBTRACT", TO_Subtract));
    m_mapEngineParam.insert(std::make_pair("TOP_ADDSMOOTH", TO_AddSmooth));
    m_mapEngineParam.insert(std::make_pair("TOP_BLENDDIFFUSEALPHA", TO_BlendDiffuseAlpha));
    m_mapEngineParam.insert(std::make_pair("TOP_BLENDTEXTUREALPHA", TO_BlendTextureAlpha));
    m_mapEngineParam.insert(std::make_pair("TOP_BLENDFACTORALPHA", TO_BlendFactorAlpha));
    m_mapEngineParam.insert(std::make_pair("TOP_BLENDTEXTUREALPHAPM", TO_BlendTextureAlphaPM));
    m_mapEngineParam.insert(std::make_pair("TOP_BLENDCURRENTALPHA", TO_BlendCurrentAlpha));
    m_mapEngineParam.insert(std::make_pair("TOP_PREMODULATE", TO_PreModulate));
    m_mapEngineParam.insert(std::make_pair("TOP_MODULATEALPHA_ADDCOLOR", TO_ModulateAlpha_AddColor));
    m_mapEngineParam.insert(std::make_pair("TOP_MODULATECOLOR_ADDALPHA", TO_ModulateColor_AddAlpha));
    m_mapEngineParam.insert(std::make_pair("TOP_MODULATEINVCOLOR_ADDALPHA", TO_ModulateInvAlpha_AddColor));
    m_mapEngineParam.insert(std::make_pair("TOP_MODULATEINVCOLOR_ADDALPHA", TO_ModulateInvColor_AddAlpha));
    m_mapEngineParam.insert(std::make_pair("TOP_BUMPENVMAP", TO_BumpenvMap));
    m_mapEngineParam.insert(std::make_pair("TOP_BUMPENVMAPLUMINANCE", TO_BumpenvMapLuminance));
    m_mapEngineParam.insert(std::make_pair("TOP_DOTPRODUCT3", TO_DotProduct3));
    m_mapEngineParam.insert(std::make_pair("TOP_MULTIPLYADD", TO_MultipyAdd));
    m_mapEngineParam.insert(std::make_pair("TOP_LERP", TO_Lerp));

    //texture arg value
    m_mapEngineParam.insert(std::make_pair("TA_CONSTANT", TAV_Constant));
    m_mapEngineParam.insert(std::make_pair("TA_DIFFUSE", TAV_Diffuse));
    m_mapEngineParam.insert(std::make_pair("TA_SELECTMASK", TAV_SelectMask));
    m_mapEngineParam.insert(std::make_pair("TA_SPECULAR", TAV_Specular));
    m_mapEngineParam.insert(std::make_pair("TA_TEMP", TAV_Temp));
    m_mapEngineParam.insert(std::make_pair("TA_TEXTURE", TAV_Texture));
    m_mapEngineParam.insert(std::make_pair("TA_TFACTOR", TAV_TFactor));
    m_mapEngineParam.insert(std::make_pair("TA_CURRENT", TAV_Current));
    m_mapEngineParam.insert(std::make_pair("TA_COMPLEMENT", TAV_Complement));
    m_mapEngineParam.insert(std::make_pair("TA_ALPHAREPLICATE", TAV_AlphaReplicate));

    //blend option
    m_mapEngineParam.insert(std::make_pair("BLENDOP_ADD", BO_Add));
    m_mapEngineParam.insert(std::make_pair("BLENDOP_SUBTRACT", BO_Subtract));
    m_mapEngineParam.insert(std::make_pair("BLENDOP_REVSUBTRACT", BO_RevSubtract));
    m_mapEngineParam.insert(std::make_pair("BLENDOP_MIN", BO_Min));
    m_mapEngineParam.insert(std::make_pair("BLENDOP_MAX", BO_Max));

    //blend op value
    m_mapEngineParam.insert(std::make_pair("BLEND_ZERO", RSRV_Zero));
    m_mapEngineParam.insert(std::make_pair("BLEND_ONE", RSRV_One));
    m_mapEngineParam.insert(std::make_pair("BLEND_SRCCOLOR", RSRV_SrcColor));
    m_mapEngineParam.insert(std::make_pair("BLEND_INVSRCCOLOR", RSRV_InvSrcColor));
    m_mapEngineParam.insert(std::make_pair("BLEND_SRCALPHA", RSRV_SrcAlpha));
    m_mapEngineParam.insert(std::make_pair("BLEND_INVSRCALPHA", RSRV_InvSrcAlpha));
    m_mapEngineParam.insert(std::make_pair("BLEND_DESTCOLOR", RSRV_DestColor));
    m_mapEngineParam.insert(std::make_pair("BLEND_INVDESTALPHA", RSRV_InvDestColor));
    m_mapEngineParam.insert(std::make_pair("BLEND_SRCALPHASAT", RSRV_SrcAlphaSat));
    m_mapEngineParam.insert(std::make_pair("BLEND_BOTHSRCALPHA", RSRV_BothSrcAlpha));
    m_mapEngineParam.insert(std::make_pair("BLEND_BOTHINVSRCALPHA", RSRV_BothInvSrcAlpha));
    m_mapEngineParam.insert(std::make_pair("BLEND_BLENDFACTOR", RSRV_BlendFactor));
    m_mapEngineParam.insert(std::make_pair("BLEND_INVBLENDFACTOR", RSRV_InvBlendFactor));
    m_mapEngineParam.insert(std::make_pair("BLEND_SRCCOLOR2", RSRV_SrcColor2));
    m_mapEngineParam.insert(std::make_pair("BLEND_INVSRCCOLOR2", RSRV_InvSrcColor2));
    m_mapEngineParam.insert(std::make_pair("BLEND_DESTALPHA", RSRV_DestAlpha));
    m_mapEngineParam.insert(std::make_pair("BLEND_INVDESTALPHA", RSRV_InvDestAlpha));

    //vertexelementtype
    m_mapEngineParam.insert(std::make_pair("eVET_FLOAT1", eVET_FLOAT1));
    m_mapEngineParam.insert(std::make_pair("eVET_FLOAT2", eVET_FLOAT2));
    m_mapEngineParam.insert(std::make_pair("eVET_FLOAT3", eVET_FLOAT3));
    m_mapEngineParam.insert(std::make_pair("eVET_FLOAT4", eVET_FLOAT4));
    m_mapEngineParam.insert(std::make_pair("eVET_COLOUR", eVET_COLOUR));
    m_mapEngineParam.insert(std::make_pair("eVET_SHORT1", eVET_SHORT1));
    m_mapEngineParam.insert(std::make_pair("eVET_SHORT2", eVET_SHORT2));
    m_mapEngineParam.insert(std::make_pair("eVET_SHORT3", eVET_SHORT3));
    m_mapEngineParam.insert(std::make_pair("eVET_SHORT4", eVET_SHORT4));
    m_mapEngineParam.insert(std::make_pair("eVET_UBYTE4", eVET_UBYTE4));
    m_mapEngineParam.insert(std::make_pair("eVET_COLOUR_ARGB", eVET_COLOUR_ARGB));
    m_mapEngineParam.insert(std::make_pair("eVET_COLOUR_ABGR", eVET_COLOUR_ABGR));

    //VertexElementUsage
    m_mapEngineParam.insert(std::make_pair("eVEU_POSITION", eVEU_POSITION));
    m_mapEngineParam.insert(std::make_pair("eVEU_BLEND_WEIGHTS", eVEU_BLEND_WEIGHTS));
    m_mapEngineParam.insert(std::make_pair("eVEU_BLEND_INDICES", eVEU_BLEND_INDICES));
    m_mapEngineParam.insert(std::make_pair("eVEU_NORMAL", eVEU_NORMAL));
    m_mapEngineParam.insert(std::make_pair("eVEU_DIFFUSE", eVEU_DIFFUSE));
    m_mapEngineParam.insert(std::make_pair("eVEU_SPECULAR", eVEU_SPECULAR));
    m_mapEngineParam.insert(std::make_pair("eVEU_TEXTURE_COORDINATES", eVEU_TEXTURE_COORDINATES));
    m_mapEngineParam.insert(std::make_pair("eVEU_BINORMAL", eVEU_BINORMAL));
    m_mapEngineParam.insert(std::make_pair("eVEU_TANGENT", eVEU_TANGENT));
    m_mapEngineParam.insert(std::make_pair("eVEU_POSITIONT", eVEU_POSITIONT));
}

