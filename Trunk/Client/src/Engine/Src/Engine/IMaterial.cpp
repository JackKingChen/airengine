#include "stdafx.h"
#include "IMaterial.h"
#include "IRenderer.h"
#include "Engine.h"
#include "FileSystem.h"
#include <assert.h>
#include "XMLFileData.h"
//#include "Technique.h"
#include "ShaderObject.h"
#include "GPUVariant.h"
#include "TextureMgr.h"
#include "EngineStringConverter.h"
#include "MaterialTemplate.h"

IMaterial::IMaterial() : m_nID(0), m_nTextureCount(0)//, m_pCurrentTechnique(NULL)
, m_fShiness(255.0f)
, m_Ka(CColorValue::White)
, m_Kd(CColorValue::White)
, m_Ks(CColorValue::Black)
, m_Ke(CColorValue::Black)
, m_bNeedCheck(false)
, m_pFileData(NULL)
, m_bReady(false)
, m_bFileLoaded(false)
, m_fVersion(0.0f)
, m_pMaterialTemplate(NULL)
{
    // : m_nID(0), m_nTextureCount(0), m_fShiness(0.0f) 
    //m_fShiness = 10.0f;
    //m_Ka = CColorValue::White;
    //m_Kd = CColorValue::White;
    //m_Ks = CColorValue::Black;
    //m_Ke = CColorValue::Black;
}

bool IMaterial::CreateFromFile(const char* szFilename, const char* szMaterialName)
{
    m_strName = AIRString(szFilename) + "/" + AIRString(szMaterialName);
    m_strMaterialName = szMaterialName;
    //m_strFilename = m_strName;
    //m_pFileData = static_cast<FileData*>(RESOURCE_MANAGER->CreateResource(Resource::Res_NormalFile, szFilename));
    m_pFileData = RESOURCE_MANAGER->CreateResource<FileData>(szFilename);
    return m_pFileData != NULL;
}

bool IMaterial::LoadFromXML(CMarkupSTL& xml, const char* szName)
{
    // 获得渲染器对象
    //IRenderer* pRenderer = ENGINE_INST->GetRenderer();

    if (xml.FindElem("Materials"))
    {
        AIRString strVersion = xml.GetAttrib("ver").c_str();
        if (!strVersion.empty())
        {
            m_fVersion = atof(strVersion.c_str());
        }
    }

    bool bRet = false;
    while (xml.FindChildElem("Material"))
    {
        xml.IntoElem();

        AIRString strName = xml.GetAttrib("Name").c_str();

        if (strcmp(strName.c_str(), szName) != 0)
        {
            xml.OutOfElem();
            continue;
        }

        AIRString strTemplate = xml.GetAttrib("template").c_str();
        if (!strTemplate.empty())
        {
            CreateFromTemplate(strTemplate.c_str());
        }
		else
		{
			CreateFromTemplate(m_strMaterialName.c_str(), m_pFileData->GetFileStream());
		}

		AIRString szMatName = xml.GetAttrib("Name").c_str();
        if (strcmp(szMatName.c_str(), szName) == 0)
        {
            m_strMaterialName = szName;
            bRet = LoadFromXML(xml);
            xml.OutOfElem();
            break;
        }

        //AddMaterial(pMaterial);
        xml.OutOfElem();
		break;
    }

    return bRet;
}

bool IMaterial::LoadFromXML(CMarkupSTL& xml)
{
    AIRString strNode;

    IRenderer* pRenderer = ENGINE_INST->GetRenderer();

	size_t nTechIndex = 0;
    while ( xml.FindChildElem() )
    {
        xml.IntoElem();
        strNode = xml.GetTagName().c_str();
        if (strNode == "Technique")
        {
			/*
			TechniqueDesc technique;

			//memset(&technique, 0, sizeof(technique));

			std::string strName = xml.GetAttrib("Name");
			if (!strName.empty())
			{
				technique.strTechniqueName = strName.c_str();
			}

			strName = xml.GetAttrib("VSVersion");
			technique.strVSVersion = strName.c_str();

			strName = xml.GetAttrib("PSVersion");
			technique.strPSVersion = strName.c_str();

			AIRVector<AIRString> vtFlows;
			CStringUtil::SplitString(xml.GetAttrib("Flow").c_str(), AIRString("|"), vtFlows);

			technique.nFlowDesc = 0;
			for (size_t i = 0; i < vtFlows.size(); ++i)
			{
				technique.nFlowDesc |= EngineParamSetting::GetRenderFlow(vtFlows[i].c_str());
			}
			//technique.nFlowDesc = EngineParamSetting::GetRenderFlow(xml.GetAttrib("Flow").c_str());

			xml.IntoElem();
			while (xml.FindElem())
			{
				if (xml.GetTagName() == "Pass")
				{
					PassDesc passDesc;
					memset(&passDesc, 0, sizeof(PassDesc));

					passDesc.strVSFunctionName = xml.GetAttrib("VertexShaderFunction").c_str();
					passDesc.strPSFunctionName = xml.GetAttrib("PixelShaderFunction").c_str();
					strNode = xml.GetAttrib("VertexDeclaration").c_str();
					if (!strNode.empty())
					{
						//passDesc.strVertexDeclaration = strNode.c_str();
					}

					xml.IntoElem();
					while (xml.FindElem())
					{
						if (xml.GetTagName() == "RenderStates")
						{
							AIRString value;
							while (xml.FindChildElem())
							{
								xml.IntoElem();
								if (xml.GetTagName() == "BlendState")
								{
									value = xml.GetAttrib("BlendEnable").c_str();
									if (!value.empty())
									{
										passDesc.blendState.blendEnable = (value == "TRUE" ? true : false);
									}

									value = xml.GetAttrib("SrcBlend").c_str();
									if (!value.empty())
									{
										//m_pBlendState->SetBlendValueSrc((RenderStateBlendValue)EngineParamSetting::GetEngineRenderParam(value));
										passDesc.blendState.nSrcColor = (RenderStateBlendValue)EngineParamSetting::GetEngineRenderParam(value);
									}

									value = xml.GetAttrib("DstBlend").c_str();
									if (!value.empty())
									{
										//m_pBlendState->SetBlendValueDest((RenderStateBlendValue)EngineParamSetting::GetEngineRenderParam(value));
										passDesc.blendState.nDestColor = (RenderStateBlendValue)EngineParamSetting::GetEngineRenderParam(value);
									}

									value = xml.GetAttrib("BlendOpt").c_str();
									if (!value.empty())
									{
										//m_pBlendState->SetBlendOp((BlendOpt)EngineParamSetting::GetEngineRenderParam(value));
										passDesc.blendState.nBlendOpColor = (BlendOpt)EngineParamSetting::GetEngineRenderParam(value);
									}

									value = xml.GetAttrib("SrcBlendAlpha").c_str();
									if (!value.empty())
									{
										//m_pBlendState->SetBlendValueSrcAlpha((RenderStateBlendValue)EngineParamSetting::GetEngineRenderParam(value));
										passDesc.blendState.nSrcAlpha = (RenderStateBlendValue)EngineParamSetting::GetEngineRenderParam(value);
									}

									value = xml.GetAttrib("DstBlendAlpha").c_str();
									if (!value.empty())
									{
										//m_pBlendState->SetBlendValueDestAlpha((RenderStateBlendValue)EngineParamSetting::GetEngineRenderParam(value));
										passDesc.blendState.nDestAlpha = (RenderStateBlendValue)EngineParamSetting::GetEngineRenderParam(value);
									}

									value = xml.GetAttrib("BlendOptAlpha").c_str();
									if (!value.empty())
									{
										//m_pBlendState->SetBlendOpAlpha((BlendOpt)EngineParamSetting::GetEngineRenderParam(value));
										passDesc.blendState.nBlendOpAlpha = (BlendOpt)EngineParamSetting::GetEngineRenderParam(value);
									}
								}
								else if (xml.GetTagName() == "RasterizerState")
								{
									value = xml.GetAttrib("FillMode").c_str();
									if (!value.empty())
									{
										//m_pRasterizerState->SetFillMode((RenderSystemFillMode)EngineParamSetting::GetEngineRenderParam(value));
										passDesc.rasterizerState.fillMode = EngineParamSetting::GetEngineRenderParam(value);
									}

									value = xml.GetAttrib("CullMode").c_str();
									if (!value.empty())
									{
										//m_pRasterizerState->SetCullMode((CullMode)EngineParamSetting::GetEngineRenderParam(value));
										passDesc.rasterizerState.cullMode = EngineParamSetting::GetEngineRenderParam(value);
									}
								}
								else if (xml.GetTagName() == "DepthStencilState")
								{
									value = xml.GetAttrib("ZEnable").c_str();
									if (!value.empty())
									{
										passDesc.depthStencilState.zEnable = (value == "TRUE" ? true : false);
									}

									value = xml.GetAttrib("StencilEnable").c_str();
									if (!value.empty())
									{
										passDesc.depthStencilState.stencilEnable = (value == "TRUE" ? true : false);
									}

									value = xml.GetAttrib("ZCompareFunc").c_str();
									if (!value.empty())
									{
										//m_pDepthStencilState->SetDepthCompareFunc((CompareFunction)EngineParamSetting::GetEngineRenderParam(value));
										passDesc.depthStencilState.DepthCompareFunc = EngineParamSetting::GetEngineRenderParam(value);
									}

									value = xml.GetAttrib("ZWrite").c_str();
									if (!value.empty())
									{
										//m_pDepthStencilState->SetDepthWriteEnable(value == "TRUE");
										passDesc.depthStencilState.zWriteEnable = (value == "TRUE");
									}
								}
								else if (xml.GetTagName() == "FixedPipeState")
								{
									value = xml.GetAttrib("AlphaTestEnable").c_str();
									if (!value.empty())
									{
										passDesc.fixedPipeState.alphaTestEnable = (value == "TRUE" ? true : false);
									}

									value = xml.GetAttrib("AlphaRef").c_str();
									if (!value.empty())
									{
										if (CStringUtil::GetStringType(value) == eST_STRING)
										{
											//m_pFixedPipeState->m_nAlphaRef = ATOL(value.c_str());
											passDesc.fixedPipeState.alphaRef = ATOL(value.c_str());
										}
										else 
										{
											passDesc.fixedPipeState.alphaRef = atoi(value.c_str());
										}
									}

									value = xml.GetAttrib("FogEnable").c_str();
									if (!value.empty())
									{
										//m_pFixedPipeState->m_bFogEnable = value == "TRUE" ? true : false;
										passDesc.fixedPipeState.fogEnable = (value == "TRUE" ? true : false);
									}

									value = xml.GetAttrib("LightEnable").c_str();
									if (!value.empty())
									{
										//m_pFixedPipeState->m_bLightEnable = value == "TRUE" ? true : false;
										passDesc.fixedPipeState.lightEnable = (value == "TRUE" ? true : false);
									}

									value = xml.GetAttrib("AlphaCompareFunc").c_str();
									if (!value.empty())
									{
										//m_pFixedPipeState->m_AlphaCompareFunc = (CompareFunction)EngineParamSetting::GetEngineRenderParam(value);
										passDesc.fixedPipeState.alphaCompareFunc = EngineParamSetting::GetEngineRenderParam(value);
									}
								}
								xml.OutOfElem();
							}
						}
					}
					xml.OutOfElem();
					technique.m_vtPassDesces.push_back(passDesc);
				}
				else if (xml.GetTagName() == "Macro")
				{                
					strName = xml.GetAttrib("Name");
					ShaderMarcoDef def;
					strcpy(def.szName, strName.c_str());

					strName = xml.GetAttrib("Definition");
					if (!strName.empty())
					{
						strcpy(def.szDefination, strName.c_str());
					}

					technique.m_vtShaderMarcos.push_back(def);
				}
			}
			xml.OutOfElem();

			RenderTechnique RenderTech;
			RenderTech.bIsReady = false;
			RenderTech.shaderMarco.Reset();
			RenderTech.nFlowDesc = technique.nFlowDesc;

			RenderTech.m_vtRenderPass.resize(technique.m_vtPassDesces.size());

			for (size_t j = 0; j < technique.m_vtPassDesces.size(); ++j)
			{
				RenderPass& renderPass = RenderTech.m_vtRenderPass[j];
				PassDesc& passDesc = technique.m_vtPassDesces[j];
				renderPass.blendState = passDesc.blendState;
				renderPass.rasterizerState = passDesc.rasterizerState;
				renderPass.depthStencilState = passDesc.depthStencilState;
				renderPass.fixedPipeState = passDesc.fixedPipeState;
			}

			if (nTechIndex < m_vtCurrentRenderTechs.size())
			{
				m_vtCurrentRenderTechs[nTechIndex] = RenderTech;
			}
			else
				m_vtCurrentRenderTechs.push_back(RenderTech);
			
			nTechIndex++;
			*/
        }
        else if (strNode == "LightParam")
        {
            std::string value = xml.GetAttrib("DIFFUSE");
            sscanf(value.c_str(), "(%f,%f,%f,%f)", &m_Kd.r, &m_Kd.g, &m_Kd.b, &m_Kd.a);

            value = xml.GetAttrib("SPECULAR");
            sscanf(value.c_str(), "(%f,%f,%f,%f)", &m_Ks.r, &m_Ks.g, &m_Ks.b, &m_Ks.a);

            value = xml.GetAttrib("EMISSIVE");
            sscanf(value.c_str(), "(%f,%f,%f,%f)", &m_Ke.r, &m_Ke.g, &m_Ke.b, &m_Ke.a);

            value = xml.GetAttrib("AMBIENT");
            sscanf(value.c_str(), "(%f,%f,%f,%f)", &m_Ka.r, &m_Ka.g, &m_Ka.b, &m_Ka.a);

            value = xml.GetAttrib("SHINESS");
            m_fShiness = atof(value.c_str());
        }
        else if (strNode == "MaterialParams")
        {
            xml.IntoElem();
            while (xml.FindElem())
            {
                strNode = xml.GetTagName().c_str();
                if (strNode == "var")
                {
                    stMtlParam param;
                    param.pVar = NULL;
                    param.strName = xml.GetAttrib("name").c_str();
                    param.csType = eMCT_FLOAT;

                    size_t index = 0;
                    //if (!GetMaterialIndexByName(param.strName, index))
                    {
                        VARIANT_T varType = EngineParamSetting::GetVariantType(xml.GetAttrib("VariantType").c_str());

                        std::string strValue = xml.GetAttrib("value");

                        const char* szValue = NULL;

                        if (!strValue.empty())
                        {
                            szValue = strValue.c_str();

                        }

                        switch (varType)
                        {
                        case VAR_FLOAT:
                            {
                                f32 fValue = atof(strValue.c_str());
                                param.pVar = new GPUVariant(fValue);
                            }
                            break;
                        case VAR_FLOAT2:
                            {
                                Vector2Df vec2;
                                if (szValue)
                                {
                                    sscanf(szValue, "(%f,%f)", &vec2.x, &vec2.y);
                                }

                                param.pVar = new GPUVariant(vec2);
                            }
                            break;
                        case VAR_FLOAT3:
                            {
                                Vector3Df vec3;
                                if (szValue)
                                {
                                    sscanf(szValue, "(%f,%f,%f)", &vec3.x, &vec3.y, &vec3.z);
                                }

                                param.pVar = new GPUVariant(vec3);
                            }
                            break;
                        case VAR_FLOAT4:
                            {
                                Vector4f vec4;
                                if (szValue)
                                {
                                    sscanf(szValue, "(%f,%f,%f,%f)", &vec4.x, &vec4.y, &vec4.z, &vec4.w);
                                }

                                param.pVar = new GPUVariant(vec4);
                            }
                            break;
                        case VAR_FLOAT4X4:
                            break;
                        case VAR_BOOL:
                            break;
                        case VAR_TEXTURE:
                            {
                                ITexture* pTexture = strValue.empty() ? NULL : TEXTURE_MANAGER->CreateTextureFromFile(strValue.c_str());

                                param.pVar = new GPUVariant(pTexture);
                                param.csType = eMCT_Sampler;
                            }
                            break;
                        case VAR_INT:
                            {
                                s32 nValue = atoi(strValue.c_str());
                                param.pVar = new GPUVariant(nValue);
                                param.csType = eMCT_INT;
                            }
                            break;
                        }

                        //if (varType == VAR_TEXTURE)
                        //{
                        //    stSampler sam;
                        //    sam.strName = xml.GetAttrib("name").c_str();
                        //    sam.pSamplerStateObject = pRenderer->CreateSamplerStateObject();
                        //    m_vtSamplers.push_back(sam);
                        //    
                        //}
						if (!GetMaterialIndexByName(param.strName, index))
							m_vtMaterialParams.push_back(param);
						else
						{
							m_vtMaterialParams[index] = param;
						}
                    }

                }
                
            }
            xml.OutOfElem();
        }
        else if (strNode == "SamplerStateObject")
        {
            std::string value = xml.GetAttrib("name");
            FixedString strSamplerName = value.c_str();
            m_vtSamplerNames.push_back(strSamplerName);

            /*
            stSampler sampler;
            sampler.strName = value.c_str();
            sampler.pSamplerStateObject = pRenderer->CreateSamplerStateObject();

            value = xml.GetAttrib("Filter");
            FixedString strValue = value.c_str();
            sampler.m_texFilter = EngineStringConverter::GetTextureFilterOption(strValue);

            value = xml.GetAttrib("AddressU");
            strValue = value.c_str();
            sampler.m_texAddr = EngineStringConverter::GetTextureAddress(strValue);
            */

            SamplerState sampler;
            memset(&sampler, 0, sizeof(SamplerState));



            value = xml.GetAttrib("Filter");
            FixedString strValue = value.c_str();
            sampler.texFilter = EngineStringConverter::GetTextureFilterOption(strValue);

            value = xml.GetAttrib("AddressU");
            strValue = value.c_str();
            sampler.texAddr = EngineStringConverter::GetTextureAddress(strValue);

            m_vtSamplers.push_back(sampler);
        }

        xml.OutOfElem();
    }

    //m_nTextureCount = m_vtTextureUnit.size();

    //if (m_pCurrentTechnique == NULL && m_vtTechniques.size() > 0)
    //{
    //    m_pCurrentTechnique = m_vtTechniques[0];
    //}

    return true;
}

/*
void IMaterial::SetTextureStageState(u32 nStage, u32 nStageState, u32 nValue)
{
    CTextureUnit* pTextureUnit = GetTextureUnit(nStage);

    if (pTextureUnit)
    {
        pTextureUnit->SetTextureStageState(nStageState, nValue);
    }
}

void IMaterial::SetSamplerState(u32 nSampler, u32 nSamplerState, u32 nValue)
{
    CTextureUnit* pTextureUnit = GetTextureUnit(nSampler);

    if (pTextureUnit)
    {
        pTextureUnit->SetSamplerState(nSamplerState, nValue);
    }
}
*/
/*
void IMaterial::ApplyMaterial(IRenderer* pRenderer)
{
    //设置纹理单元

    
    int nIndex = 0;
    for (TEXTUREUNIT_ITER it = m_vtTextureUnit.begin(); it != m_vtTextureUnit.end(); ++it, ++nIndex)
    {
        (*it)->ApplyTextureUnit(this);
    }
    
    //把不需要用到的texture全部disable
    int nCurrentTextures = pRenderer->GetCurrentTexturesNum();
    for (; nIndex < nCurrentTextures; ++nIndex)
    {
        pRenderer->SetTexture(nIndex, NULL);
    }
    pRenderer->SetCurrentTexturesNum(m_vtTextureUnit.size());

    if (IsLightingEnable())
    {
        if (HasVertexShader())
        {
            
        }
        else
        {
            pRenderer->SetSurfaceMaterialParam(m_Ke, m_Ks, m_Kd, m_Ka, m_fShiness);
        }
        
    }

    IShader* pShader = GetShader();
    if (pShader)
    {
        pShader->Begin();
    }
    
}
*/
void IMaterial::Clear()
{
    /*
    for (size_t i = 0; i < m_vtTechniques.size(); ++i)
    {
        if (m_vtTechniques[i])
        {
            m_vtTechniques[i]->Clear();
            SafeDelete(m_vtTechniques[i]);
        }
        
    }
    m_vtTechniques.clear();
    m_pCurrentTechnique = NULL;
    */

    if (m_pFileData)
    {
        RESOURCE_MANAGER->ClearResource(m_pFileData);
        m_pFileData = NULL;
    }
    
    for (size_t i = 0; i < m_vtMaterialParams.size(); ++i)
    {
        SafeDelete(m_vtMaterialParams[i].pVar);
    }
    m_vtMaterialParams.clear();
    m_vtSamplers.clear();
}

bool IMaterial::IsTransparent()
{
    if (m_pMaterialTemplate)
    {
        //这里重复读入rendertechnique，需要优化，优化方案：创建rendercontext这个类，这个类有一切的渲染信息
        //RenderTechnique* pTechnqiue = GetRenderTechnique(m_curShaderMarco, 0);
        //return pTechnqiue->m_vtRenderPass[0].blendState.blendEnable;
        return false;
    }
    else
    {
        /*
        if (m_pCurrentTechnique)
        {
            return m_pCurrentTechnique->IsTransparent();
        }

        for (size_t i = 0; i < m_vtTechniques.size(); ++i)
        {
            if (m_vtTechniques[i]->IsTransparent())
            {
                return true;
            }
        }
        */
    }
    
    return false;
}

void IMaterial::DeepCopy(IMaterial* pCopy)
{
    Clear();
    m_Ke = pCopy->m_Ke;
    m_Ks = pCopy->m_Ks;
    m_Ka = pCopy->m_Ka;
    m_Kd = pCopy->m_Kd;
    m_fShiness = pCopy->m_fShiness;
    /*
    for (size_t i = 0; i < pCopy->m_vtTechniques.size(); ++i)
    {
        CTechnique* pTechnique = new CTechnique(this);
        *pTechnique = *pCopy->m_vtTechniques[i];

        m_vtTechniques.push_back(pTechnique);

        if (pTechnique->GetName() == "Default")
        {
            m_pCurrentTechnique = pTechnique;
        }
    }

    if (m_pCurrentTechnique == NULL && m_vtTechniques.size() > 0)
    {
        m_pCurrentTechnique = m_vtTechniques[0];
    }
    */

    m_strName = pCopy->m_strName;
    m_strMaterialName = pCopy->m_strMaterialName;
    //m_bLoaded = pCopy->m_bLoaded;
    m_bReady = pCopy->m_bReady;
    //m_bShare = pCopy->m_bShare;
    m_pFileData = pCopy->m_pFileData;

    if (m_pFileData)
    {
        m_pFileData->AddRef();
    }
    m_bFileLoaded = pCopy->m_bFileLoaded;

    
    for (size_t i = 0; i < pCopy->m_vtMaterialParams.size(); ++i)
    {
        stMtlParam param;
        param.pVar = NULL;
        if (pCopy->m_vtMaterialParams[i].pVar != NULL)
        {
            param.pVar = new GPUVariant(*(pCopy->m_vtMaterialParams[i].pVar));
        }
        param.csType = pCopy->m_vtMaterialParams[i].csType;
        param.strName = pCopy->m_vtMaterialParams[i].strName;
        m_vtMaterialParams.push_back(param);
    }

    m_vtSamplers = pCopy->m_vtSamplers;
    m_vtSamplerNames = pCopy->m_vtSamplerNames;
    m_pMaterialTemplate = pCopy->m_pMaterialTemplate;
    m_curShaderMarco = pCopy->m_curShaderMarco;
    m_vtCurrentRenderTechs = pCopy->m_vtCurrentRenderTechs;
}

bool IMaterial::IsLightingEnable()
{
    /*
    if (m_pCurrentTechnique)
    {
        return m_pCurrentTechnique->IsLightingEnable();
    }

    for (size_t i = 0; i < m_vtTechniques.size(); ++i)
    {
        if (m_vtTechniques[i]->IsLightingEnable())
        {
            return true;
        }
    }
    */
    return false;
}

bool IMaterial::IsFogEnable()
{
    /*
	if (m_pCurrentTechnique)
	{
		return m_pCurrentTechnique->IsFogEnable();
	}

	for (size_t i = 0; i < m_vtTechniques.size(); ++i)
	{
		if (m_vtTechniques[i]->IsFogEnable())
		{
			return true;
		}
	}
    */
	return false;
}

bool IMaterial::IsWireFrame() const
{
    /*
    if (m_pCurrentTechnique)
    {
        return m_pCurrentTechnique->IsWireFrame();
    }

    for (size_t i = 0; i < m_vtTechniques.size(); ++i)
    {
        if (m_vtTechniques[i]->IsWireFrame())
        {
            return true;
        }
    }
    */
    return false;
}

void IMaterial::SetTransparent(bool bAlpha)
{
    /*for (size_t i = 0; i < m_vtTechniques.size(); ++i)
    {
        m_vtTechniques[i]->SetTransparent(bAlpha);
    }*/
}

void IMaterial::SetFillMode(RenderSystemFillMode fillMode)
{
    /*for (size_t i = 0; i < m_vtTechniques.size(); ++i)
    {
        m_vtTechniques[i]->SetFillMode(fillMode);
    }*/
}

void IMaterial::SetZWriteEnable(bool bEnable)
{
    /*for (size_t i = 0; i < m_vtTechniques.size(); ++i)
    {
        m_vtTechniques[i]->SetZWriteEnable(bEnable);
    }*/
}

void IMaterial::SetZBufferEnable(bool bEnable)
{
    /*for (size_t i = 0; i < m_vtTechniques.size(); ++i)
    {
        m_vtTechniques[i]->SetZBufferEnable(bEnable);
    }*/
}

/*

void IMaterial::SetTextureUnitTex(int nStage, ITexture* pTexture)
{
    for (size_t i = 0; i < m_vtTechniques.size(); ++i)
    {
        m_vtTechniques[i]->SetTextureUnitTex(nStage, pTexture);
    }
}

void IMaterial::SetTextureUnitTex(int nStage, const char* szTextureFile, int nMipmap)
{ 
    for (size_t i = 0; i < m_vtTechniques.size(); ++i)
    {
        m_vtTechniques[i]->SetTextureUnitTex(nStage, szTextureFile, nMipmap);
    }
}
*/

void IMaterial::SetAmbient(const CColorValue& ambient)
{
    m_Ka = ambient;
}

void IMaterial::SetDifusse(const CColorValue& difusse)
{
    m_Kd = difusse;
}

void IMaterial::SetEmissive(const CColorValue& emissive)
{
    m_Ke = emissive;
}

void IMaterial::SetSpecular(const CColorValue& specular)
{
    m_Ks = specular;
}

void IMaterial::SetShiness(float fShiness)
{
    m_fShiness = fShiness;
}

void IMaterial::SaveToFile(const char* szFilename)
{
    CMarkupSTL xml;


    CDataStream* pStream = FILESYSTEM->GetFileStream(szFilename);

    if (pStream != NULL)
    {
        AIRString strDoc = pStream->GetAsString();
        xml.SetDoc(strDoc.c_str());
        xml.IntoElem();
    }
    else
    {
        xml.SetDoc("<?xml version=\"1.0\" encoding=\"gb2312\"?>\r\n");
        xml.AddElem("Materials");

        xml.IntoElem();
        xml.AddNode(CMarkupSTL::MNT_TEXT, "\r\n    ");
    }
    SafeDelete(pStream);  //释放了原文件先

    while (xml.FindChildElem("Material"))
    {
        xml.IntoElem();
        const AIRString& strMaterialName = xml.GetAttrib("Name").c_str();
        if (strMaterialName == m_strMaterialName) 
        {
            xml.RemoveElem();
            xml.OutOfElem();
            break;
        }
        xml.OutOfElem();
    }

    xml.IntoElem();
    xml.AddElem("Material");

    xml.SetAttrib("Name", m_strMaterialName.c_str());
    //xml.SetAttrib("TextureCount", m_vtTextureUnit.size());

    //进入材质结点Material
    xml.IntoElem();

    //保存technique
    /*
    for (size_t i = 0; i < m_vtTechniques.size(); ++i)
    {
        if (m_vtTechniques[i])
        {
            xml.AddElem("Technique");
            xml.AddAttrib("Name", m_vtTechniques[i]->GetName().c_str());
            //xml.AddAttrib("VertexShaderVersion", m_vtTechniques[i]->GetVSVersion().c_str());
            //xml.AddAttrib("PixelShaderVersion", m_vtTechniques[i]->GetPSVersion().c_str());

            //获得pass
            for (size_t nPass = 0; nPass < m_vtTechniques[i]->GetPassesNum(); ++nPass)
            {
                xml.IntoElem();
                CPass* pPass = m_vtTechniques[i]->GetPass(nPass);
                if (pPass)
                {
                    pPass->SaveToXML(xml);
                }
                xml.OutOfElem();
            }
        }
        
    }
    */

    xml.AddElem("LightParam");
    //xml.AddAttrib("Type", "DIFFUSE");
    //顺序是RGBA
    char szTmp[256] = { 0 };
    sprintf(szTmp, "(%f,%f,%f,%f)", m_Kd.r, m_Kd.g, m_Kd.b, m_Kd.a);
    xml.AddAttrib("DIFFUSE", szTmp);

    sprintf(szTmp, "(%f,%f,%f,%f)", m_Ks.r, m_Ks.g, m_Ks.b, m_Ks.a);
    xml.AddAttrib("SPECULAR", szTmp);

    sprintf(szTmp, "(%f,%f,%f,%f)", m_Ke.r, m_Ke.g, m_Ke.b, m_Ke.a);
    xml.AddAttrib("EMISSIVE", szTmp);

    sprintf(szTmp, "(%f,%f,%f,%f)", m_Ka.r, m_Ka.g, m_Ka.b, m_Ka.a);
    xml.AddAttrib("AMBIENT", szTmp);

    sprintf(szTmp, "%.4f", m_fShiness);
    xml.AddAttrib("SHINESS", szTmp);
    xml.OutOfElem();

    xml.OutOfElem();

    if (xml.Save(szFilename))
    {
        //m_strFilename = szFilename;
    }
}

void IMaterial::SaveToFile(const char* szPath, const char* szFilename)
{
	CMarkupSTL xml;


	CDataStream* pStream = FILESYSTEM->GetFileStream(szPath, szFilename);

	if (pStream != NULL)
	{
		AIRString strDoc = pStream->GetAsString();
		xml.SetDoc(strDoc.c_str());
		xml.IntoElem();
	}
	else
	{
		xml.SetDoc("<?xml version=\"1.0\" encoding=\"gb2312\"?>\r\n");
		xml.AddElem("Materials");

		xml.IntoElem();
		xml.AddNode(CMarkupSTL::MNT_TEXT, "\r\n    ");
	}
	SafeDelete(pStream);  //释放了原文件先

	while (xml.FindChildElem("Material"))
	{
		xml.IntoElem();
		const AIRString& strMaterialName = xml.GetAttrib("Name").c_str();
		if (strMaterialName == m_strMaterialName) 
		{
			xml.RemoveElem();
			xml.OutOfElem();
			break;
		}
		xml.OutOfElem();
	}

	xml.IntoElem();
	xml.AddElem("Material");

	xml.SetAttrib("Name", m_strMaterialName.c_str());
	//xml.SetAttrib("TextureCount", m_vtTextureUnit.size());

	//进入材质结点Material
	xml.IntoElem();

	//保存technique
    /*
	for (size_t i = 0; i < m_vtTechniques.size(); ++i)
	{
		if (m_vtTechniques[i])
		{
			xml.AddElem("Technique");
			xml.AddAttrib("Name", m_vtTechniques[i]->GetName().c_str());
			//xml.AddAttrib("VertexShaderVersion", m_vtTechniques[i]->GetVSVersion().c_str());
			//xml.AddAttrib("PixelShaderVersion", m_vtTechniques[i]->GetPSVersion().c_str());

			//获得pass
			for (size_t nPass = 0; nPass < m_vtTechniques[i]->GetPassesNum(); ++nPass)
			{
				xml.IntoElem();
				CPass* pPass = m_vtTechniques[i]->GetPass(nPass);
				if (pPass)
				{
					pPass->SaveToXML(xml);
				}
				xml.OutOfElem();
			}
		}

	}
    */

	xml.AddElem("LightParam");
	//xml.AddAttrib("Type", "DIFFUSE");
	//顺序是RGBA
	char szTmp[256] = { 0 };
	sprintf(szTmp, "(%f,%f,%f,%f)", m_Kd.r, m_Kd.g, m_Kd.b, m_Kd.a);
	xml.AddAttrib("DIFFUSE", szTmp);

	sprintf(szTmp, "(%f,%f,%f,%f)", m_Ks.r, m_Ks.g, m_Ks.b, m_Ks.a);
	xml.AddAttrib("SPECULAR", szTmp);

	sprintf(szTmp, "(%f,%f,%f,%f)", m_Ke.r, m_Ke.g, m_Ke.b, m_Ke.a);
	xml.AddAttrib("EMISSIVE", szTmp);

	sprintf(szTmp, "(%f,%f,%f,%f)", m_Ka.r, m_Ka.g, m_Ka.b, m_Ka.a);
	xml.AddAttrib("AMBIENT", szTmp);

	sprintf(szTmp, "%.4f", m_fShiness);
	xml.AddAttrib("SHINESS", szTmp);
	xml.OutOfElem();

	xml.OutOfElem();

	AIRString strFullName = AIRString(szPath) + AIRString("/") + szFilename;
	if (xml.Save(szFilename))
	{
		//m_strFilename = szFilename;
	}
}


void IMaterial::SetLightEnable(bool bEnable)
{
    //for (size_t i = 0; i < m_vtTechniques.size(); ++i)
    //{
    //    m_vtTechniques[i]->SetLightEnable(bEnable);
    //}
}

void IMaterial::SetFogEnable( bool bEnable )
{
	//for (size_t i = 0; i < m_vtTechniques.size(); ++i)
	//{
	//	m_vtTechniques[i]->SetFogEnable(bEnable);
	//}
}
/*
void IMaterial::SetDepthBias(float fBias)
{
    for (size_t i = 0; i < m_vtTechniques.size(); ++i)
    {
        m_vtTechniques[i]->SetDepthBias(fBias);
    }
}

void IMaterial::SetSlopeScaleDepthBias(float fBias)
{
    for (size_t i = 0; i < m_vtTechniques.size(); ++i)
    {
        m_vtTechniques[i]->SetSlopeScaleDepthBias(fBias);
    }
}
*/


/*
CTechnique* IMaterial::GetTechnique(const AIRString& strName)
{
    for (size_t i = 0; i < m_vtTechniques.size(); ++i)
    {
        if (m_vtTechniques[i]->GetName() == strName)
        {
            return m_vtTechniques[i];
        }
    }
    return NULL;
}

void IMaterial::AddTechnique(CTechnique* pTechnique)
{
    for (size_t i = 0; i < m_vtTechniques.size(); ++i)
    {
        if(m_vtTechniques[i]->GetName() == pTechnique->GetName())
        {
            return;
        }
    }
    //pTechnique->Cache();
    m_vtTechniques.push_back(pTechnique);
}
*/

void IMaterial::SetBlendSrc(RenderStateBlendValue src)
{
    //for (size_t i = 0; i < m_vtTechniques.size(); ++i)
    //{
    //    m_vtTechniques[i]->SetBlendSrc(src);
    //}
}

void IMaterial::SetBlendDest(RenderStateBlendValue dest)
{
    //for (size_t i = 0; i < m_vtTechniques.size(); ++i)
    //{
    //    m_vtTechniques[i]->SetBlendDest(dest);
    //}
} 

void IMaterial::SetBlendOpt(BlendOpt opt)
{
    //for (size_t i = 0; i < m_vtTechniques.size(); ++i)
    //{
    //    m_vtTechniques[i]->SetBlendOpt(opt);
    //}
}

void IMaterial::SetAlphaTest(bool bAlphaTest)
{
    //for (size_t i = 0; i < m_vtTechniques.size(); ++i)
    //{
    //    m_vtTechniques[i]->SetAlphaTest(bAlphaTest);
    //}
}

void IMaterial::SetAlphaRef(u32 nRef)
{
    //for (size_t i = 0; i < m_vtTechniques.size(); ++i)
    //{
    //    m_vtTechniques[i]->SetAlphaRef(nRef);
    //}
}

void IMaterial::SetName(const char* szName)
{
    m_strName = szName;
}

void IMaterial::SetCullMode(CullMode cull)
{
    //for (size_t i = 0; i < m_vtTechniques.size(); ++i)
    //{
    //    m_vtTechniques[i]->SetCullMode(cull);
    //}
}

void IMaterial::SetStencilEnable(bool bEnable)
{
    //for (size_t i = 0; i < m_vtTechniques.size(); ++i)
    //{
    //    m_vtTechniques[i]->SetStencilEnable(bEnable);
    //}
}
/*
void IMaterial::RemoveTechique(int nIndex)
{
    if (m_vtTechniques.size() > nIndex)
    {
        if (m_vtTechniques[nIndex])
        {
            m_vtTechniques[nIndex]->Clear();
            SafeDelete(m_vtTechniques[nIndex]);
        }
        
    }
}
*/
bool IMaterial::Cache()
{
    if (!m_bFileLoaded)
    {
        if (m_pFileData && m_pFileData->IsLoaded())
        {
            m_pFileData->GetFileStream()->Seek(0, CDataStream::AIR_SEEK_SET);
            AIRString strDoc = m_pFileData->GetFileStream()->GetAsString();

            CMarkupSTL xml;
            xml.SetDoc(strDoc.c_str());
            
            m_bFileLoaded = LoadFromXML(xml, m_strMaterialName.c_str());
            //RESOURCE_MANAGER->ClearResource(m_pFileData);
            //m_pFileData = NULL;

            /*
            bool bReady = true;
            for (size_t i = 0; i < m_vtTechniques.size(); ++i)
            {
                if (!m_vtTechniques[i]->Cache())
                {
                    bReady = false;
                }
            }
            m_bReady = bReady;
            */
        }
        else
        {
            return false;
        }
    }
    
    if (m_bFileLoaded)
    {
        /*
        if (!m_bReady && !m_vtTechniques.empty())
        {
            bool bReady = true;
            for (size_t i = 0; i < m_vtTechniques.size(); ++i)
            {
                if (!m_vtTechniques[i]->Cache())
                {
                    bReady = false;
                }
            }

            for (size_t i = 0; i < m_vtMaterialParams.size(); ++i)
            {
                GPUVariant* pVar = m_vtMaterialParams[i].pVar;
                if (pVar && pVar->GetType() == VAR_TEXTURE)
                {
                    if (pVar->GetTexture() && !pVar->GetTexture()->IsReady())
                    {
                        bReady = false;
                        bReady = pVar->GetTexture()->Cache();
                    }
                }
            }
            m_bReady = bReady;
        }
        */

        if (!m_bReady && m_pMaterialTemplate)
        {
            bool bReady = true;

            for (size_t i = 0; i < m_vtMaterialParams.size(); ++i)
            {
                GPUVariant* pVar = m_vtMaterialParams[i].pVar;
                if (pVar && pVar->GetType() == VAR_TEXTURE)
                {
                    if (pVar->GetTexture() && !pVar->GetTexture()->IsReady())
                    {
                        bReady = false;
                        bReady = pVar->GetTexture()->Cache();
                    }
                }
            }
            if (bReady)
            {
                bReady = m_pMaterialTemplate->IsReady();
            }
            
            m_bReady = bReady;
        }
		else
		{
			m_bReady = (m_pMaterialTemplate == NULL);
		}
    }
    

    return m_bReady;
}

/*
bool IMaterial::Load()
{
    XMLFileData* pFileData = static_cast<XMLFileData*>(FILEDATA_MANAGER->CreateFileData(m_strFilename.c_str()));

    if (pFileData == NULL)
    {
        return false;
    }

    CMarkupSTL& xml = pFileData->GetXML();

    if (!xml.FindElem())
    {
        return false;
    }

    m_strName = m_strFilename + "/" + m_strMaterialName;
    //m_strFilename = m_strName;

    //CStringUtil::ToLowerCase(m_strName);
    //SafeDelete(pStream);

    LoadFromXML(xml, m_strMaterialName.c_str());

    if (m_bLoaded)
    {
        //NotifyAllListeners();
    }

    return m_bLoaded;
}
*/

void IMaterial::CheckResourceStatus()
{
    /*
    bool bLoaded = false;
    for (size_t i = 0; i < m_vtTechniques.size(); ++i)
    {
        bLoaded = m_vtTechniques[i]->CheckLoadStatus();
        if (!bLoaded)
        {
            break;
        }
    }
    */

    //m_bLoaded = bLoaded;
    if (m_pFileData->IsLoaded())
    {
        m_bReady = (m_pMaterialTemplate != NULL ? m_pMaterialTemplate->IsReady() : true);
    }
    else
    {
        m_bReady = false;
    }

    m_bNeedCheck = false;
}

void IMaterial::SetMaterialParam(const FixedString& strName, const GPUVariant& var)
{
    size_t index = 0;
    
    if (GetMaterialIndexByName(strName, index))
    {
        if (m_vtMaterialParams[index].pVar == NULL)
        {
            m_vtMaterialParams[index].pVar = new GPUVariant;
        }

        *m_vtMaterialParams[index].pVar = var;
    }

    /*
    if (m_pCurrentTechnique)
    {
        for (size_t i = 0; i < m_pCurrentTechnique->GetPassesNum(); ++i)
        {
            CPass* pPass = m_pCurrentTechnique->GetPass(i);
            if (pPass)
            {
                ShaderObject* pShaderObject = pPass->GetShaderObject();
                if (pShaderObject)
                {
                    pShaderObject->SetGPUVariant(strName, var);
                }
            }
        }
    }
    */
}

bool IMaterial::GetMaterialIndexByName(const FixedString& strName, size_t& index)
{
    for (size_t i = 0; i < m_vtMaterialParams.size(); ++i)
    {
        if (m_vtMaterialParams[i].strName == strName)
        {
            index = i;
            return true;
        }
    }

    return false;
}
/*
void IMaterial::UpdateMaterialParams(CPass* pPass)
{
    ShaderObject* pShaderObject = pPass->GetShaderObject();

    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    CGPUDataParamSource* pGPUDataParam = pRenderer->GetGPUDataParam();
    pGPUDataParam->SetMaterialShiness(GetShininess());
    pGPUDataParam->SetMaterialAmbient(GetAmbient());
    pGPUDataParam->SetMaterialDiffuse(GetDiffuse());
    pGPUDataParam->SetMaterialSpecular(GetSpecular());

    pShaderObject->BeginUse();
    
    stMtlParam* pParam = NULL;
    for (size_t i = 0; i < m_vtMaterialParams.size(); ++i)
    {
        pParam = &m_vtMaterialParams[i];
        if (pParam && pParam->pVar)
        {
            pShaderObject->UpdateMaterialParam(pParam->strName, pParam->pVar, pParam->csType);
        }
        
    }

    pPass->ApplyPass(pRenderer);

    //下面是更新sampler，主要是提交samplerstate
    for (size_t i = 0; i < m_vtSamplers.size(); ++i)
    {
        u32 nIndex = 0;
        pShaderObject->GetSamplerIndex(m_vtSamplerNames[i], nIndex);

        pRenderer->SetSamplerState(nIndex, m_vtSamplers[i]);
    }
}
*/

void IMaterial::UpdateMaterialParams(RenderPass* pPass)
{
    ShaderObject* pShaderObject = pPass->pShaderObject;

    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    CGPUDataParamSource* pGPUDataParam = pRenderer->GetGPUDataParam();
    pGPUDataParam->SetMaterialShiness(GetShininess());
    pGPUDataParam->SetMaterialAmbient(GetAmbient());
    pGPUDataParam->SetMaterialDiffuse(GetDiffuse());
    pGPUDataParam->SetMaterialSpecular(GetSpecular());

    pShaderObject->BeginUse();

    stMtlParam* pParam = NULL;
    for (size_t i = 0; i < m_vtMaterialParams.size(); ++i)
    {
        pParam = &m_vtMaterialParams[i];
        if (pParam && pParam->pVar)
        {
            pShaderObject->UpdateMaterialParam(pParam->strName, pParam->pVar, pParam->csType);
            if (pParam->csType == eMCT_Sampler)
            {
                if (pParam->nSamplerIndex == -1)
                {
                    for (size_t s = 0; s < m_vtSamplerNames.size(); ++s)
                    {
                        if (pParam->strName == m_vtSamplerNames[s])
                        {
                            pParam->nSamplerIndex = s;
                            break;
                        }
                    }
                }

                u32 nIndex = 0;
                pShaderObject->GetSamplerIndex(m_vtSamplerNames[pParam->nSamplerIndex], nIndex);

                pRenderer->SetSamplerState(nIndex, m_vtSamplers[pParam->nSamplerIndex]);
            }
        }

    }

    pGPUDataParam->m_fAlphaRef = (f32)pPass->fixedPipeState.alphaRef / 255.0f;

    pShaderObject->Apply(pRenderer);

    //pPass->ApplyPass(pRenderer);
    //设置渲染状态
    pRenderer->SetRenderState(pPass->blendState, pPass->rasterizerState, pPass->depthStencilState, pPass->fixedPipeState);

    //下面是更新sampler，主要是提交samplerstate
    
    for (size_t i = 0; i < m_vtSamplers.size(); ++i)
    {
        u32 nIndex = 0;
        pShaderObject->GetSamplerIndex(m_vtSamplerNames[i], nIndex);

        //pRenderer->SetSamplerState(nIndex, m_vtSamplers[i]);
    }
    
}

void IMaterial::AddMaterialParam(const FixedString& strName, MaterialConstantType type, const GPUVariant& var)
{
    stMtlParam mtlPara;
    mtlPara.strName = strName;
    mtlPara.csType = type;
    mtlPara.pVar = new GPUVariant;
    *mtlPara.pVar = var;
    m_vtMaterialParams.push_back(mtlPara);
}

bool IMaterial::CreateFromTemplate(const FixedString& strFile)
{
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    MaterialTemplate* pTemplate = pRenderer->CreateMaterialTemplate(strFile);
    if (pTemplate == 0)
    {
        return false;
    }

    m_pMaterialTemplate = pTemplate;

    for (size_t i = 0; i < pTemplate->GetTechniquesNum(); ++i)
    {
        const TechniqueDesc& techDesc = pTemplate->GetTechniqueDesc(i);

        RenderTechnique RenderTech;
        RenderTech.bIsReady = false;
        RenderTech.shaderMarco.Reset();
        RenderTech.nFlowDesc = techDesc.nFlowDesc;

        RenderTech.m_vtRenderPass.resize(techDesc.m_vtPassDesces.size());

        for (size_t j = 0; j < techDesc.m_vtPassDesces.size(); ++j)
        {
            RenderPass& renderPass = RenderTech.m_vtRenderPass[j];
            renderPass.vertexArrayObj = 0;
            m_pMaterialTemplate->GetRenderState(i, j, renderPass.blendState, renderPass.rasterizerState, renderPass.depthStencilState, renderPass.fixedPipeState);
        }
        

        m_vtCurrentRenderTechs.push_back(RenderTech);
        m_vtTechFlowIndices.push_back(i);
    }

	size_t nMtlParamNum = m_pMaterialTemplate->GetMaterialParamsNum();
	for (size_t i = 0; i < nMtlParamNum; ++i)
	{
		stMtlParam param;
		m_pMaterialTemplate->GetMaterialParam(i, param);
		m_vtMaterialParams.push_back(param);
	}

	size_t nSamplersNum = m_pMaterialTemplate->GetSamplersNum();
	for (size_t i = 0; i < nSamplersNum; ++i)
	{
		FixedString strSamplerName;
		SamplerState samState;
		if (m_pMaterialTemplate->GetSamplerObject(i, strSamplerName, samState))
		{
			m_vtSamplerNames.push_back(strSamplerName);
			m_vtSamplers.push_back(samState);
		}
	}
	m_bFileLoaded = true;
	return true;
}

bool IMaterial::CreateFromTemplate(const FixedString& strName, CDataStream* pStream)
{
	IRenderer* pRenderer = ENGINE_INST->GetRenderer();
	MaterialTemplate* pTemplate = pRenderer->CreateMaterialTemplate(strName, pStream);
	if (pTemplate == 0)
	{
		return false;
	}

	m_pMaterialTemplate = pTemplate;

	for (size_t i = 0; i < pTemplate->GetTechniquesNum(); ++i)
	{
		const TechniqueDesc& techDesc = pTemplate->GetTechniqueDesc(i);

		RenderTechnique RenderTech;
		RenderTech.bIsReady = false;
		RenderTech.shaderMarco.Reset();
		RenderTech.nFlowDesc = techDesc.nFlowDesc;

		RenderTech.m_vtRenderPass.resize(techDesc.m_vtPassDesces.size());

		for (size_t j = 0; j < techDesc.m_vtPassDesces.size(); ++j)
		{
			RenderPass& renderPass = RenderTech.m_vtRenderPass[j];
			m_pMaterialTemplate->GetRenderState(i, j, renderPass.blendState, renderPass.rasterizerState, renderPass.depthStencilState, renderPass.fixedPipeState);
		}


		m_vtCurrentRenderTechs.push_back(RenderTech);
		m_vtTechFlowIndices.push_back(i);
	}

	size_t nMtlParamNum = m_pMaterialTemplate->GetMaterialParamsNum();
	for (size_t i = 0; i < nMtlParamNum; ++i)
	{
		stMtlParam param;
		m_pMaterialTemplate->GetMaterialParam(i, param);
		m_vtMaterialParams.push_back(param);
	}

	size_t nSamplersNum = m_pMaterialTemplate->GetSamplersNum();
	for (size_t i = 0; i < nSamplersNum; ++i)
	{
		FixedString strSamplerName;
		SamplerState samState;
		if (m_pMaterialTemplate->GetSamplerObject(i, strSamplerName, samState))
		{
			m_vtSamplerNames.push_back(strSamplerName);
			m_vtSamplers.push_back(samState);
		}
	}
	m_bFileLoaded = true;
	return true;
}

RenderTechnique* IMaterial::GetRenderTechnique(const ShaderMarco& marco, size_t nIndex)
{
    RenderTechnique& renderTech = m_vtCurrentRenderTechs[nIndex];
    if (renderTech.shaderMarco != marco || !renderTech.bIsReady)
    {
        renderTech.shaderMarco = marco;
        
        for (size_t i = 0; i < renderTech.m_vtRenderPass.size(); ++i)
        {
            GetRenderPass(marco, nIndex, i, renderTech.m_vtRenderPass[i]);
        }
        
        renderTech.bIsReady = true;
    }

    return &m_vtCurrentRenderTechs[nIndex];
}

bool IMaterial::GetRenderPass(const ShaderMarco& shaderMarco, size_t nTechIndex, size_t nPassIndex, RenderPass& pass)
{
    IGPUProgram* pVertexShader = NULL;
    IGPUProgram* pPixelShader = NULL;

    m_pMaterialTemplate->GetShader(shaderMarco, nTechIndex, nPassIndex, &pVertexShader, &pPixelShader);

    //IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    //IGPUProgram* pGpuPrograms[4] = { 0 };
    //pGpuPrograms[0] = pVertexShader;
    //pGpuPrograms[1] = pPixelShader;
    if (pass.pShaderObject)
    {
        if (pass.pShaderObject->GetGPUProgram(eGST_Vertex) != pVertexShader || pass.pShaderObject->GetGPUProgram(eGST_Pixel) != pPixelShader)
        {
            ShaderObject* pShaderObject = m_pMaterialTemplate->CreateShaderObject(pVertexShader, pPixelShader);//pRenderer->CreateShaderObject(pGpuPrograms, 2);
            pass.pShaderObject = pShaderObject;
        }
    }
    else
    {
        ShaderObject* pShaderObject = m_pMaterialTemplate->CreateShaderObject(pVertexShader, pPixelShader);//pRenderer->CreateShaderObject(pGpuPrograms, 2);
        pass.pShaderObject = pShaderObject;
    }

    m_pMaterialTemplate->GetRenderState(nTechIndex, nPassIndex, pass.blendState, pass.rasterizerState, pass.depthStencilState, pass.fixedPipeState);
    return true;
}

size_t IMaterial::GetTechniqueIndexByFlow(eRenderFlow flow)
{
    for (size_t i = 0; i < m_vtCurrentRenderTechs.size(); ++i)
    {
        if (m_vtCurrentRenderTechs[i].nFlowDesc & flow)
        {
            return i;
        }
    }
    return -1;
}

void IMaterial::AddSamplerState(const FixedString& strName, const SamplerState& state)
{
    if (GetSamplerStateIndex(strName) == -1)
    {
        m_vtSamplerNames.push_back(strName);
        m_vtSamplers.push_back(state);
    }
    
}

size_t IMaterial::GetSamplerStateIndex(const FixedString& strName)
{
    for (size_t i = 0; i < m_vtSamplerNames.size(); ++i)
    {
        if (m_vtSamplerNames[i] == strName)
        {
            return i;
        }
    }

    return -1;
}
