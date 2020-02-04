#include "stdafx.h"
#include "VertexDeclarationManager.h"
#include "Engine.h"
#include "MarkupSTL.h"
#include "EngineStringConverter.h"
#include "FileSystem.h"

VertexDeclarationManager::VertexDeclarationManager()
{

}

VertexDeclarationManager::~VertexDeclarationManager()
{

}

void VertexDeclarationManager::Clear()
{
    VertexDeclarationMap_Iter itEnd = m_mapVertexDeclarations.end();
    for (VertexDeclarationMap_Iter it = m_mapVertexDeclarations.begin(); it != itEnd; ++it)
    {
        CVertexDeclaration* pVertexDecl = it->second;
        pVertexDecl->Clear();
        SafeDelete(pVertexDecl);
    }

    m_mapVertexDeclarations.clear();

    m_mapVertexDeclElements.clear();
}

/*
CVertexDeclaration* VertexDeclarationManager::CreateVertexDeclaration(EVertexBufferType type)
{
    VertexDeclarationMap_Iter it = m_mapDefaultVD.find(type);
    if (it != m_mapDefaultVD.end())
    {
        return it->second;
    }

    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    CVertexDeclaration* pVertexDecl = pRenderer->CreateVertexDeclaration();
    pVertexDecl->Build(type);
    m_mapDefaultVD.insert(std::make_pair(type, pVertexDecl));
    return pVertexDecl;
}


void VertexDeclarationManager::AddVertexDeclaration( CVertexDeclaration* pVertexDecl )
{
	VertexDeclarationMap_Iter it = m_mapDefaultVD.begin();
	for ( ; it != m_mapDefaultVD.end(); ++it )
	{
		if ( it->second == pVertexDecl )
		{
			return;
		}
	}
	m_mapDefaultVD.insert( std::make_pair(EVertexBufferType(E_VB_USER_DEF + m_mapDefaultVD.size()), pVertexDecl) );
}
*/

bool VertexDeclarationManager::LoadFromXML(const char* szFilename)
{
    CDataStream* pStream = FILESYSTEM->GetFileStream( szFilename );

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

    AIRString strElementType;
    AIRString strElementUsage;
	AIRString strInstancing;
    int nStream = 0;
    int nLastStream = 0;
    int nIndex = 0;

    while ( xml.FindElem() )
    {
        FixedString strName = xml.GetAttrib("name").c_str();

        ElementVector vecElements;

        u32 nOffset = 0;

        xml.IntoElem();

        while (xml.FindElem())
        {
            nStream = atoi(xml.GetAttrib("Stream").c_str());
            if (nStream != nLastStream)
            {
                nOffset = 0;
            }
            strElementType = xml.GetAttrib("VertexElementType").c_str();
            strElementUsage = xml.GetAttrib("VertexElementUsage").c_str();
            nIndex = atoi(xml.GetAttrib("Index").c_str());
			strInstancing = xml.GetAttrib("Instancing").c_str();
			bool bInstancing = false;
			if (!strInstancing.empty())
			{
				CStringUtil::ToLowerCase(strInstancing);
				bInstancing = (strInstancing == "true");
			}

            VertexElementType elementType = (VertexElementType)EngineStringConverter::GetEngineParamByName(strElementType);
            VertexElementUsage elementUsage = (VertexElementUsage)EngineStringConverter::GetEngineParamByName(strElementUsage);
            
            vecElements.push_back(VERTEX_ELEMENT(nStream, nOffset, elementType, elementUsage, bInstancing, nIndex));
            nOffset += CVertexDeclaration::GetTypeSize(elementType);

            nLastStream = nStream;
        }

        m_mapVertexDeclElements.insert(std::make_pair(strName, vecElements));

        xml.OutOfElem();
        
    }

    SafeDelete(pStream);

    return true;
}

VertexDeclarationManager::ElementVector& VertexDeclarationManager::GetElementVector(const FixedString& strName)
{
    AIRMap<FixedString, ElementVector>::iterator it = m_mapVertexDeclElements.find(strName);

    assert(it != m_mapVertexDeclElements.end());
    return it->second;
}

CVertexDeclaration* VertexDeclarationManager::CreateVertexDeclaration(const FixedString& strName, IGPUProgram* pVertexShader)
{
    VertexDeclarationMap_Iter it = m_mapVertexDeclarations.find(strName);

    if (it != m_mapVertexDeclarations.end())
    {
        return it->second;
    }

    ElementVector& vecElement = GetElementVector(strName);
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    CVertexDeclaration* pVertexDecl = pRenderer->CreateVertexDeclaration();

    if (pVertexDecl)
    {
        pVertexDecl->Create(vecElement, pVertexShader);

        m_mapVertexDeclarations.insert(std::make_pair(strName, pVertexDecl));
    }
    

    return pVertexDecl;
}
