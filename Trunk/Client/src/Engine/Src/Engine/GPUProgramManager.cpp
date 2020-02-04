#include "stdafx.h"
#include "IGPUProgram.h"
#include "GPUProgramManager.h"
#include "Engine.h"
#include "FileSystem.h"
#include "StringUtil.h"
#include "VertexDeclarationManager.h"


GPUProgramManager::GPUProgramManager()
{

}

GPUProgramManager::~GPUProgramManager()
{

}

void GPUProgramManager::AddGPUProgram(IGPUProgram* pGPUProgram)
{
    GPUProgram_Iter it = m_mapGPUProgram.find(pGPUProgram->GetName());
    if (it != m_mapGPUProgram.end())
    {
        assert(false);
    }
    m_mapGPUProgram.insert(std::make_pair(pGPUProgram->GetName(), pGPUProgram));
}

/*
IGPUProgram* GPUProgramManager::CreateVertexShader(const char* szFilename, const char* szVersion, const char* szFunction)
{
    IGPUProgram* pVertexShader = NULL;

    GPUProgram_Iter it = m_mapGPUProgram.find(szFilename);
    if (it != m_mapGPUProgram.end())
    {
        pVertexShader = it->second;
        //pVertexShader->AddRef();
        return pVertexShader;
    }

    pVertexShader = RENDERER->CreateVertexShader();
    if (!pVertexShader->Create(szFilename, szVersion, szFunction))
    {
        SafeDelete(pVertexShader);
        return NULL;
    }

    m_mapGPUProgram.insert(make_pair(pVertexShader->GetFilename(), pVertexShader));

    return pVertexShader;
}

IGPUProgram* GPUProgramManager::CreatePixelShader(const char* szFilename, const char* szVersion, const char* szFunction)
{
    IGPUProgram* pPixelShader = NULL;
    GPUProgram_Iter it = m_mapGPUProgram.find(szFilename);
    if (it != m_mapGPUProgram.end())
    {
        pPixelShader = it->second;
        //pVertexShader->AddRef();
        return pPixelShader;
    }

    pPixelShader = RENDERER->CreatePixelShader();
    if (!pPixelShader->Create(szFilename, szVersion, szFunction))
    {
        SafeDelete(pPixelShader);
        return NULL;
    }

    m_mapGPUProgram.insert(make_pair(pPixelShader->GetName(), pPixelShader));

    //pPixelShader->AddRef();

    return pPixelShader;
}
*/

IGPUProgram* GPUProgramManager::GetGPUProgram(const AIRString& strName)
{
	if ( strName.length() == 0 )
	{
		return NULL;
	}
    GPUProgram_Iter it = m_mapGPUProgram.find(strName);
    if (it != m_mapGPUProgram.end())
    {
        return it->second;
    }
    assert(false);
    return NULL;
}

void GPUProgramManager::Clear()
{
    GPUProgram_Iter itEnd = m_mapGPUProgram.end();
    for (GPUProgram_Iter it = m_mapGPUProgram.begin(); it != itEnd; ++it)
    {
        it->second->Clear();
        SafeDelete(it->second);
    }
    m_mapGPUProgram.clear();
}

void GPUProgramManager::CreateDefaultShaders(const char* szShaderPatch)
{
    VertexDeclarationManager::GetInstance()->LoadFromXML("VertexDeclaration.xml");
    //CreateDefaultShadersThroughPatchFile(szShaderPatch);
}

void GPUProgramManager::AddShaderFile(const char* szFilename, GPUProgramCreateCallback pCallback)
{
    m_mapShaderFiles.insert(std::make_pair(szFilename, pCallback));
}

bool GPUProgramManager::CreateDefaultShadersThroughPatchFile( const char* szShaderPatchFile /*= "ShaderPatch.xml"*/ )
{
	CDataStream* pStream = FILESYSTEM->GetFileStream( szShaderPatchFile );

	if ( !pStream )
	{
		return false;
	}

	CMarkupSTL xml( pStream->GetAsString().c_str() );
	if ( !xml.FindElem() )
	{
		SafeDelete(pStream);
		return false;
	}

	xml.IntoElem();
	AIRString szFile("");
	AIRString szShaderType("");
	AIRString szShaderName("");
	AIRString szShaderVersion("");
	AIRString szShaderEntryFunc("");
    

	while ( xml.FindElem() )
	{
		if( xml.GetTagName() == "PatchUnion" )
		{
			szFile = xml.GetAttrib( "SrcFile" ).c_str();

			CDataStream* pShaderFileStream = FILESYSTEM->GetFileStream( szFile.c_str() );
			if (!pShaderFileStream)
			{
				DT_TO_MSGBOX_FMT( LT_ERROR, GetString("加载shader文件[%s]失败!", szFile.c_str()) );
				return false;
			}

			xml.IntoElem();

			while ( xml.FindElem("ShaderUnion") )
			{
				szShaderType = xml.GetAttrib( "ShaderType" ).c_str();
				CStringUtil::ToUpperCase( szShaderType );
				szShaderName = xml.GetAttrib( "ShaderName" ).c_str();
				szShaderVersion = xml.GetAttrib( "ShaderVersion" ).c_str();
				szShaderEntryFunc = xml.GetAttrib( "ShaderEntryFunc" ).c_str();
				IGPUProgram* pGPUProgram = NULL;

				if ( szShaderType == "VS" )
				{
                    std::string vsDecl = xml.GetAttrib("VertexDeclaration");
                    FixedString strVertexDeclaration = vsDecl.c_str();
					if ( szShaderEntryFunc.empty() )
					{
						szShaderEntryFunc = "VSMain";
					}
					pGPUProgram = RENDERER->CreateVertexShader( szShaderName.c_str(), szShaderVersion.c_str(), szShaderEntryFunc.c_str() );	
					if ( !pGPUProgram )
					{
						DT_TO_MSGBOX_FMT( LT_ERROR, GetString("创建shader失败!\n[ShaderFile]:%s\n[ShaderType]:%s\n[ShaderName]:%s\n[ShaderVersion]:%s\n[EntryFunc]:%s", 
							szFile.c_str(), szShaderType.c_str(), szShaderName.c_str(), szShaderVersion.c_str(), szShaderEntryFunc.c_str()) );
						return false;
					}
					while( xml.FindChildElem("Macro") )
					{
						pGPUProgram->AddMarco( xml.GetChildAttrib("Name").c_str(), xml.GetChildAttrib("Definition").c_str() );
					}

					while (xml.FindChildElem("Class") )
					{
						pGPUProgram->AddClassName(xml.GetChildAttrib("Name").c_str());
					}

					if( !pGPUProgram->Create(pShaderFileStream) )
					{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
						AIRString msg = AIRString("Create Shader [") + szFile + AIRString("] failed!");
						MessageBoxA( NULL, msg.c_str(), "ERROR", MB_OK );
#endif
					}
					//屏蔽，因为顶点声明和pass无关
                    //if (!strVertexDeclaration.empty())
                    //{
                    //    pGPUProgram->CreateVertexDeclaration(strVertexDeclaration);
                    //}
					GPUPROGRAM_MANAGER->AddGPUProgram(pGPUProgram);
				}
				else if ( szShaderType == "PS" )
				{
					if ( szShaderEntryFunc.length() == 0 )
					{
						szShaderEntryFunc = "PSMain";
					}
					pGPUProgram = RENDERER->CreatePixelShader( szShaderName.c_str(), szShaderVersion.c_str(), szShaderEntryFunc.c_str() );	
					if ( !pGPUProgram )
					{
						DT_TO_MSGBOX_FMT( LT_ERROR, GetString("创建shader失败!\n[ShaderFile]:%s\n[ShaderType]:%s\n[ShaderName]:%s\n[ShaderVersion]:%s\n[EntryFunc]:%s", 
							szFile.c_str(), szShaderType.c_str(), szShaderName.c_str(), szShaderVersion.c_str(), szShaderEntryFunc.c_str()) );
						return false;
					}
                    xml.IntoElem();
                    while (xml.FindElem())
                    {
					    if( xml.GetTagName() == "Macro" )
					    {
						    pGPUProgram->AddMarco( xml.GetAttrib("Name").c_str(), xml.GetAttrib("Definition").c_str() );
					    }

					    if ( xml.GetTagName() == "Class" )
					    {
						    pGPUProgram->AddClassName(xml.GetAttrib("Name").c_str());
					    }

                        if (xml.GetTagName() == "TextureUnit")
                        {
                            pGPUProgram->AddSamplerIndex(xml.GetAttrib("name").c_str(), atoi(xml.GetAttrib("unit").c_str()));
                        }
                    }
                    xml.OutOfElem();
					pGPUProgram->Create(pShaderFileStream);
					GPUPROGRAM_MANAGER->AddGPUProgram(pGPUProgram);
				}
			}

			DT_TO_DBGSTR( LT_NONE, GetString("创建shader文件[%s]成功!", szFile.c_str()) );

			SafeDelete( pShaderFileStream );
			xml.OutOfElem();
		}
	}

	xml.OutOfElem();

	SafeDelete( pStream );
	return true;
}