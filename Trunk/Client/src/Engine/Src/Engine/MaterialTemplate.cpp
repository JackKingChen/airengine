#include "MaterialTemplate.h"
#include "GPUVariant.h"
#include "Engine.h"
#include "IGPUProgram.h"
#include "FileData.h"
#include "StringUtil.h"
#include "ShaderObject.h"
#include "TextureMgr.h"
#include "EngineStringConverter.h"

MaterialTemplate::MaterialTemplate() : m_bReady(false), m_fVersion(0.0f)
, m_pVertexShaderFile(NULL)
, m_pPixelShaderFile(NULL)
{

}

MaterialTemplate::~MaterialTemplate()
{
}

bool MaterialTemplate::Create(CDataStream* pStream)
{
    AIRString strDoc = pStream->GetAsString();

    CMarkupSTL xml;
    xml.SetDoc(strDoc.c_str());

    return LoadFromXML(xml);
}

bool MaterialTemplate::GetShader(ShaderMarco marco, u32 nTechniqueIndex, u32 nPassIndex, IGPUProgram** pVertexShader, IGPUProgram** pPixelShader)
{
    if (nTechniqueIndex >= m_vtTechniqueDesces.size())
    {
        return false;
    }

    if (nPassIndex >= m_vtTechniqueDesces[nTechniqueIndex].m_vtPassDesces.size())
    {
        return false;
    }

    TechniqueDesc& techDesc = m_vtTechniqueDesces[nTechniqueIndex];

    PassDesc& passDesc = techDesc.m_vtPassDesces[nPassIndex];

    //先处理vs
    GPUPROGRAM_MAP& mapVS = m_vtFunctionVSPrograms[passDesc.nVSIndex];

    GPUPROGRAM_MAP::iterator it = mapVS.find(marco.m_uMarcoValue);

    
    if (it == mapVS.end())
    {
        //编译
        IRenderer* pRenderer = ENGINE_INST->GetRenderer();
        IGPUProgram* pGPUProgram = pRenderer->CreateVertexShader("vs", techDesc.strVSVersion.AsCharPtr(), passDesc.strVSFunctionName.AsCharPtr());
        AIRVector<ShaderMarcoDef> vtMarcos;
        marco.GetMarcoStrings(vtMarcos);
        for (size_t i = 0; i < vtMarcos.size(); ++i)
        {
            pGPUProgram->AddMarco(vtMarcos[i].szName, vtMarcos[i].szDefination);
        }

        if (pGPUProgram->Create(m_pVertexShaderFile->GetFileStream()))
        {
            //为了适应gles2
            if (!ENGINE_INST->IsHardwareSkeleton() && m_strVertexShaderFile == "VSMesh.glsl")
            {
            //    passDesc.strVertexDeclaration = "VS_MeshInput";
            }
            //pGPUProgram->CreateVertexDeclaration(passDesc.strVertexDeclaration);
            mapVS.insert(std::make_pair(marco.m_uMarcoValue, pGPUProgram));

            *pVertexShader = pGPUProgram;
        }
    }
    else
    {
        *pVertexShader = it->second;
    }

    //处理ps
    GPUPROGRAM_MAP& mapPS = m_vtFunctionPSPrograms[passDesc.nPSIndex];

    it = mapPS.find(marco.m_uMarcoValue);

    if (it == mapPS.end())
    {
        //编译
        IRenderer* pRenderer = ENGINE_INST->GetRenderer();
        IGPUProgram* pGPUProgram = pRenderer->CreatePixelShader("ps", techDesc.strPSVersion.AsCharPtr(), passDesc.strPSFunctionName.AsCharPtr());
        AIRVector<ShaderMarcoDef> vtMarcos;
        marco.GetMarcoStrings(vtMarcos);
        for (size_t i = 0; i < vtMarcos.size(); ++i)
        {
            pGPUProgram->AddMarco(vtMarcos[i].szName, vtMarcos[i].szDefination);
        }

        if (pGPUProgram->Create(m_pPixelShaderFile->GetFileStream()))
        {
            mapPS.insert(std::make_pair(marco.m_uMarcoValue, pGPUProgram));

            //仅用于gles2
            for (AIRMap<FixedString, s32>::iterator it = m_mapSamplerIndex.begin(); it != m_mapSamplerIndex.end(); ++it)
            {
                pGPUProgram->AddSamplerIndex((it->first).AsCharPtr(), it->second);
            }

            *pPixelShader = pGPUProgram;
        }
    }
    else
    {
        *pPixelShader = it->second;
    }

	return true;
}

bool MaterialTemplate::LoadFromStream(CDataStream* pStream, const char* szName)
{
	pStream->Seek(0, CDataStream::AIR_SEEK_SET);
	AIRString strDoc = pStream->GetAsString();

	CMarkupSTL xml;
	xml.SetDoc(strDoc.c_str());

	return LoadFromXML(xml, szName);
}

bool MaterialTemplate::LoadFromXML(CMarkupSTL& xml, const char* szName)
{
	if (xml.FindElem("Materials"))
	{
		AIRString strVersion = xml.GetAttrib("ver").c_str();
		if (!strVersion.empty())
		{
			m_fVersion = atof(strVersion.c_str());
		}
	}

	bool bRet = false;
	xml.IntoElem();
	while (xml.FindElem("Material"))
	{
		AIRString szMatName = xml.GetAttrib("Name").c_str();

		if (strcmp(szMatName.c_str(), szName) != 0)
		{
			continue;
		}
		xml.IntoElem();

		AIRString strNode;

		while (xml.FindElem())
		{
			strNode = xml.GetTagName().c_str();

			if (strNode == "Technique")
			{

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

						//把入口函数作为marco写进technique，这里是为了适应glsl只有一个void main()函数的问题
						AIRString strMarcoVS = AIRString("USE_") + passDesc.strVSFunctionName.AsCharPtr(); 
						CStringUtil::ToUpperCase(strMarcoVS);
						AIRString strMarcoPS = AIRString("USE_") + passDesc.strPSFunctionName.AsCharPtr();
						CStringUtil::ToUpperCase(strMarcoPS);
						ShaderMarcoDef def;
						strcpy(def.szName, strMarcoVS.c_str());
						technique.AddShaderMarco(def);
						strcpy(def.szName, strMarcoPS.c_str());
						technique.AddShaderMarco(def);
						//----

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

						//technique.m_vtShaderMarcos.push_back(def);
						technique.AddShaderMarco(def);
					}
				}
				xml.OutOfElem();

				m_vtTechniqueDesces.push_back(technique);
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


						VARIANT_T varType = EngineParamSetting::GetVariantType(xml.GetAttrib("VariantType").c_str());

						param.csType = EngineParamSetting::VariantTypeToMtlType(varType);

						std::string strValue = xml.GetAttrib("value");

						if (!strValue.empty())
						{
							const char* szValue = strValue.c_str();
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
									sscanf(szValue, "(%f,%f)", &vec2.x, &vec2.y);
									param.pVar = new GPUVariant(vec2);
								}
								break;
							case VAR_FLOAT3:
								{
									Vector3Df vec3;
									sscanf(szValue, "(%f,%f,%f)", &vec3.x, &vec3.y, &vec3.z);
									param.pVar = new GPUVariant(vec3);
								}
								break;
							case VAR_FLOAT4:
								{
									Vector4f vec4;
									sscanf(szValue, "(%f,%f,%f,%f)", &vec4.x, &vec4.y, &vec4.z, &vec4.w);
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
						}
						m_vtMaterialParams.push_back(param);
					}
				}
				xml.OutOfElem();
			}
			else if (strNode == "VertexShader")
			{
				m_strVertexShaderFile = xml.GetAttrib("file").c_str();
				if (m_pVertexShaderFile == NULL && !m_strVertexShaderFile.empty())
				{
					IRenderer* pRenderer = ENGINE_INST->GetRenderer();
					pRenderer->RenameShaderFile(m_strVertexShaderFile.c_str(), m_strVertexShaderFile, true);
					if (!ENGINE_INST->IsHardwareSkeleton() && m_strVertexShaderFile == "VSSkinAnim.glsl")
					{
						m_strVertexShaderFile = "VSMesh.glsl";
					}
					m_pVertexShaderFile = RESOURCE_MANAGER->CreateResource<FileData>(m_strVertexShaderFile.c_str());
					m_pVertexShaderFile->AddListener(this);
					m_pVertexShaderFile->LoadFromFile(m_strVertexShaderFile.c_str(), false);
				}
			}
			else if (strNode == "PixelShader")
			{
				m_strPixelShaderFile = xml.GetAttrib("file").c_str();
				if (m_pPixelShaderFile == NULL && !m_strPixelShaderFile.empty())
				{
					IRenderer* pRenderer = ENGINE_INST->GetRenderer();
					pRenderer->RenameShaderFile(m_strPixelShaderFile.c_str(), m_strPixelShaderFile, false);
					m_pPixelShaderFile = RESOURCE_MANAGER->CreateResource<FileData>(m_strPixelShaderFile.c_str());
					m_pPixelShaderFile->AddListener(this);
					m_pPixelShaderFile->LoadFromFile(m_strPixelShaderFile.c_str(), false);
				}
				xml.IntoElem();
				while (xml.FindElem("TextureUnit"))
				{
					AddSamplerIndex(xml.GetAttrib("name").c_str(), atoi(xml.GetAttrib("unit").c_str()));
				}
				xml.OutOfElem();
			}
			else if (strNode == "SamplerStateObject")
			{
				std::string value = xml.GetAttrib("name");
				FixedString strSamplerName = value.c_str();
				m_vtSamplerNames.push_back(strSamplerName);

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
		}

		xml.OutOfElem();
		break;
	}
	xml.OutOfElem();

	//加载shader
	for (size_t i = 0; i < m_vtTechniqueDesces.size(); ++i)
	{
		TechniqueDesc& techDesc = m_vtTechniqueDesces[i];
		for (size_t j = 0; j < techDesc.m_vtPassDesces.size(); ++j)
		{
			PassDesc& desc = techDesc.m_vtPassDesces[j];
			IGPUProgram* pVertexShader = RENDERER->CreateVertexShader("VertexShader", techDesc.strVSVersion.AsCharPtr(), desc.strVSFunctionName.AsCharPtr());

			ShaderMarco marco;
			for (size_t k = 0; k < techDesc.m_vtShaderMarcos.size(); ++k)
			{
				pVertexShader->AddMarco(techDesc.m_vtShaderMarcos[k].szName, techDesc.m_vtShaderMarcos[k].szDefination);
				marco.SetShaderMarco(techDesc.m_vtShaderMarcos[k].szName, 1);
			}

			pVertexShader->Create(m_pVertexShaderFile->GetFileStream());

			//为了适应gles2
			if (!ENGINE_INST->IsHardwareSkeleton() && m_strVertexShaderFile == "VSMesh.glsl")
			{
				//    desc.strVertexDeclaration = "VS_MeshInput";
			}
			//pVertexShader->CreateVertexDeclaration(desc.strVertexDeclaration.AsCharPtr());

			//Sh
			size_t vsIndex = GetVSFunctionIndex(desc.strVSFunctionName);
			GPUPROGRAM_MAP& mapProgram = m_vtFunctionVSPrograms[vsIndex];

			mapProgram.insert(std::make_pair(marco.m_uMarcoValue, pVertexShader));

			IGPUProgram* pPixelShader = RENDERER->CreatePixelShader("PixelShader", techDesc.strPSVersion.AsCharPtr(), desc.strPSFunctionName.AsCharPtr());

			for (size_t k = 0; k < techDesc.m_vtShaderMarcos.size(); ++k)
			{
				pPixelShader->AddMarco(techDesc.m_vtShaderMarcos[k].szName, techDesc.m_vtShaderMarcos[k].szDefination);
				marco.SetShaderMarco(techDesc.m_vtShaderMarcos[k].szName, 1);
			}

			pPixelShader->Create(m_pPixelShaderFile->GetFileStream());

			//仅用于gles2
			for (AIRMap<FixedString, s32>::iterator it = m_mapSamplerIndex.begin(); it != m_mapSamplerIndex.end(); ++it)
			{
				pPixelShader->AddSamplerIndex((it->first).AsCharPtr(), it->second);
			}

			size_t psIndex = GetPSFunctionIndex(desc.strPSFunctionName);
			GPUPROGRAM_MAP& mapProgramPS = m_vtFunctionPSPrograms[psIndex];
			mapProgramPS.insert(std::make_pair(marco.m_uMarcoValue, pPixelShader));
		}
	}

	m_bReady = true;
	return true;
}

bool MaterialTemplate::LoadFromXML(CMarkupSTL& xml)
{
    bool bRet = false;
    while (xml.FindElem("Material"))
    {
		AIRString szMatName = xml.GetAttrib("Name").c_str();
        xml.IntoElem();
        
        AIRString strNode;

        while (xml.FindElem())
        {
            strNode = xml.GetTagName().c_str();

            if (strNode == "Technique")
            {
                
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

						//把入口函数作为marco写进technique，这里是为了适应glsl只有一个void main()函数的问题
						AIRString strMarcoVS = AIRString("USE_") + passDesc.strVSFunctionName.AsCharPtr(); 
						CStringUtil::ToUpperCase(strMarcoVS);
						AIRString strMarcoPS = AIRString("USE_") + passDesc.strPSFunctionName.AsCharPtr();
						CStringUtil::ToUpperCase(strMarcoPS);
						ShaderMarcoDef def;
						strcpy(def.szName, strMarcoVS.c_str());
						technique.AddShaderMarco(def);
						strcpy(def.szName, strMarcoPS.c_str());
						technique.AddShaderMarco(def);
						//----
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

                m_vtTechniqueDesces.push_back(technique);
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
                        

                        VARIANT_T varType = EngineParamSetting::GetVariantType(xml.GetAttrib("VariantType").c_str());

						param.csType = EngineParamSetting::VariantTypeToMtlType(varType);

                        std::string strValue = xml.GetAttrib("value");

                        if (!strValue.empty())
                        {
                            const char* szValue = strValue.c_str();
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
                                    sscanf(szValue, "(%f,%f)", &vec2.x, &vec2.y);
                                    param.pVar = new GPUVariant(vec2);
                                }
                                break;
                            case VAR_FLOAT3:
                                {
                                    Vector3Df vec3;
                                    sscanf(szValue, "(%f,%f,%f)", &vec3.x, &vec3.y, &vec3.z);
                                    param.pVar = new GPUVariant(vec3);
                                }
                                break;
                            case VAR_FLOAT4:
                                {
                                    Vector4f vec4;
                                    sscanf(szValue, "(%f,%f,%f,%f)", &vec4.x, &vec4.y, &vec4.z, &vec4.w);
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
                        }
                        m_vtMaterialParams.push_back(param);
                    }
                }
				xml.OutOfElem();
            }
            else if (strNode == "VertexShader")
            {
                m_strVertexShaderFile = xml.GetAttrib("file").c_str();
                if (m_pVertexShaderFile == NULL && !m_strVertexShaderFile.empty())
                {
                    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
                    pRenderer->RenameShaderFile(m_strVertexShaderFile.c_str(), m_strVertexShaderFile, true);
                    if (!ENGINE_INST->IsHardwareSkeleton() && m_strVertexShaderFile == "VSSkinAnim.glsl")
                    {
                        m_strVertexShaderFile = "VSMesh.glsl";
                    }
                    m_pVertexShaderFile = RESOURCE_MANAGER->CreateResource<FileData>(m_strVertexShaderFile.c_str());
                    m_pVertexShaderFile->AddListener(this);
                    m_pVertexShaderFile->LoadFromFile(m_strVertexShaderFile.c_str(), false);
                }
            }
            else if (strNode == "PixelShader")
            {
                m_strPixelShaderFile = xml.GetAttrib("file").c_str();
                if (m_pPixelShaderFile == NULL && !m_strPixelShaderFile.empty())
                {
                    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
                    pRenderer->RenameShaderFile(m_strPixelShaderFile.c_str(), m_strPixelShaderFile, false);
                    m_pPixelShaderFile = RESOURCE_MANAGER->CreateResource<FileData>(m_strPixelShaderFile.c_str());
                    m_pPixelShaderFile->AddListener(this);
                    m_pPixelShaderFile->LoadFromFile(m_strPixelShaderFile.c_str(), false);
                }
                xml.IntoElem();
                while (xml.FindElem("TextureUnit"))
                {
                    AddSamplerIndex(xml.GetAttrib("name").c_str(), atoi(xml.GetAttrib("unit").c_str()));
                }
                xml.OutOfElem();
            }
			else if (strNode == "SamplerStateObject")
			{
				std::string value = xml.GetAttrib("name");
				FixedString strSamplerName = value.c_str();
				m_vtSamplerNames.push_back(strSamplerName);

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
        }

        xml.OutOfElem();
    }

    //加载shader
    for (size_t i = 0; i < m_vtTechniqueDesces.size(); ++i)
    {
        TechniqueDesc& techDesc = m_vtTechniqueDesces[i];
        for (size_t j = 0; j < techDesc.m_vtPassDesces.size(); ++j)
        {
            PassDesc& desc = techDesc.m_vtPassDesces[j];
            IGPUProgram* pVertexShader = RENDERER->CreateVertexShader("VertexShader", techDesc.strVSVersion.AsCharPtr(), desc.strVSFunctionName.AsCharPtr());

            ShaderMarco marco;
            for (size_t k = 0; k < techDesc.m_vtShaderMarcos.size(); ++k)
            {
                pVertexShader->AddMarco(techDesc.m_vtShaderMarcos[k].szName, techDesc.m_vtShaderMarcos[k].szDefination);
                marco.SetShaderMarco(techDesc.m_vtShaderMarcos[k].szName, 1);
            }

            pVertexShader->Create(m_pVertexShaderFile->GetFileStream());

            //为了适应gles2
            if (!ENGINE_INST->IsHardwareSkeleton() && m_strVertexShaderFile == "VSMesh.glsl")
            {
            //    desc.strVertexDeclaration = "VS_MeshInput";
            }
            //pVertexShader->CreateVertexDeclaration(desc.strVertexDeclaration.AsCharPtr());

            //Sh
            size_t vsIndex = GetVSFunctionIndex(desc.strVSFunctionName);
            GPUPROGRAM_MAP& mapProgram = m_vtFunctionVSPrograms[vsIndex];
            
            mapProgram.insert(std::make_pair(marco.m_uMarcoValue, pVertexShader));

            IGPUProgram* pPixelShader = RENDERER->CreatePixelShader("PixelShader", techDesc.strPSVersion.AsCharPtr(), desc.strPSFunctionName.AsCharPtr());

            for (size_t k = 0; k < techDesc.m_vtShaderMarcos.size(); ++k)
            {
                pPixelShader->AddMarco(techDesc.m_vtShaderMarcos[k].szName, techDesc.m_vtShaderMarcos[k].szDefination);
                marco.SetShaderMarco(techDesc.m_vtShaderMarcos[k].szName, 1);
            }

            pPixelShader->Create(m_pPixelShaderFile->GetFileStream());

            //仅用于gles2
            for (AIRMap<FixedString, s32>::iterator it = m_mapSamplerIndex.begin(); it != m_mapSamplerIndex.end(); ++it)
            {
                pPixelShader->AddSamplerIndex((it->first).AsCharPtr(), it->second);
            }

            size_t psIndex = GetPSFunctionIndex(desc.strPSFunctionName);
            GPUPROGRAM_MAP& mapProgramPS = m_vtFunctionPSPrograms[psIndex];
            mapProgramPS.insert(std::make_pair(marco.m_uMarcoValue, pPixelShader));
        }
    }

    m_bReady = true;
    return true;
}

void MaterialTemplate::OnResourceLoaded(Resource* pResource)
{
    if (pResource == m_pVertexShaderFile)
    {
    }
    else if (pResource == m_pPixelShaderFile)
    {
    }
}

size_t MaterialTemplate::GetVSFunctionIndex(const FixedString& strFunction)
{
    for (size_t i = 0; i < m_vtVSFunctions.size(); ++i)
    {
        if (m_vtVSFunctions[i] == strFunction)
        {
            return i;
        }
    }

    m_vtVSFunctions.push_back(strFunction);
    GPUPROGRAM_MAP mapProgram;
    m_vtFunctionVSPrograms.push_back(mapProgram);

    return m_vtVSFunctions.size() - 1;
}

size_t MaterialTemplate::GetPSFunctionIndex(const FixedString& strFunction)
{
    for (size_t i = 0; i < m_vtPSFunctions.size(); ++i)
    {
        if (m_vtPSFunctions[i] == strFunction)
        {
            return i;
        }
    }

    m_vtPSFunctions.push_back(strFunction);
    GPUPROGRAM_MAP mapProgram;
    m_vtFunctionPSPrograms.push_back(mapProgram);

    return m_vtPSFunctions.size() - 1;
}

void MaterialTemplate::Clear()
{
    for (size_t i = 0; i < m_vtFunctionVSPrograms.size(); ++i)
    {
        GPUPROGRAM_MAP& mapShaders = m_vtFunctionVSPrograms[i];

        for (GPUPROGRAM_MAP::iterator it = mapShaders.begin(); it != mapShaders.end(); ++it)
        {
            IGPUProgram* pShader = it->second;
            pShader->Clear();
            SafeDelete(pShader);
        }

        mapShaders.clear();
    }

    m_vtFunctionVSPrograms.clear();

    for (size_t i = 0; i < m_vtFunctionPSPrograms.size(); ++i)
    {
        GPUPROGRAM_MAP& mapShaders = m_vtFunctionPSPrograms[i];

        for (GPUPROGRAM_MAP::iterator it = mapShaders.begin(); it != mapShaders.end(); ++it)
        {
            IGPUProgram* pShader = it->second;
            pShader->Clear();
            SafeDelete(pShader);
        }

        mapShaders.clear();
    }

    m_vtFunctionPSPrograms.clear();

    for (AIRMap<u64, ShaderObject*>::iterator it = m_mapShaderObjects.begin(); it != m_mapShaderObjects.end(); ++it)
    {
        SafeDelete(it->second);
    }
    m_mapShaderObjects.clear();

	for (size_t i = 0; i < m_vtMaterialParams.size(); ++i)
	{
		if (m_vtMaterialParams[i].pVar)
		{
			SafeDelete(m_vtMaterialParams[i].pVar);
		}
	}
	m_vtMaterialParams.clear();
}

void MaterialTemplate::GetRenderState(size_t nTechIndex, size_t nPassIndex, BlendState& blend, RasterizerState& rasterizerState, DepthStencilState& depthStencil, FixedPipeState& fixedPipe)
{
    PassDesc& passDesc = m_vtTechniqueDesces[nTechIndex].m_vtPassDesces[nPassIndex];
    blend = passDesc.blendState;
    rasterizerState = passDesc.rasterizerState;
    depthStencil = passDesc.depthStencilState;
    fixedPipe = passDesc.fixedPipeState;
}

ShaderObject* MaterialTemplate::CreateShaderObject(IGPUProgram* pVertexShader, IGPUProgram* pPixelShader)
{
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();

    u64 uVSHigh = (u64)(pVertexShader);
    u64 uPSLow = (u64)(pPixelShader);

    u64 key = (uVSHigh << 32 | uPSLow);

    ShaderObject_Map::iterator it = m_mapShaderObjects.find(key);
    if (it == m_mapShaderObjects.end())
    {
        ShaderObject* pShaderObject = pRenderer->CreateShaderObject();

        IGPUProgram* pGpuPrograms[4] = { 0 };
        pGpuPrograms[0] = pVertexShader;
        pGpuPrograms[1] = pPixelShader;
        if (pShaderObject->Create(pGpuPrograms, 2))
        {
            m_mapShaderObjects.insert(std::make_pair(key, pShaderObject));
            return pShaderObject;
        }
        else
        {
            SafeDelete(pShaderObject);
        }
    }
    else
    {
        return it->second;
    }
    return NULL;
}

void MaterialTemplate::GetMaterialParam(size_t nIndex, stMtlParam& mtlParam)
{
	if (nIndex < m_vtMaterialParams.size())
	{
		if (mtlParam.pVar)
		{
			SafeDelete(mtlParam.pVar);
		}

		mtlParam = m_vtMaterialParams[nIndex];
	}
}

bool MaterialTemplate::GetSamplerObject(size_t index, FixedString& name, SamplerState& state)
{
	if (index < m_vtSamplers.size())
	{
		name = m_vtSamplerNames[index];
		state = m_vtSamplers[index];

		return true;
	}

	return false;
}
