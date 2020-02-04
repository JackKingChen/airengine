#include "stdafx.h"
#include "MaterialManager.h"
//#include "SystemMgr.h"
#include "FileSystem.h"
#include "StringUtil.h"
#include "Engine.h"

//CMaterialManager* CMaterialManager::m_pInstance = NULL;

CMaterialManager::CMaterialManager() : m_nMaterialCount(0)
{

}

CMaterialManager::~CMaterialManager()
{

}

IMaterial* CMaterialManager::GetMaterial(const char* szName)
{
    IMaterial* pMaterial = NULL;
    for (MATERIAL_LIST_ITER it = m_lstMaterials.begin(); it != m_lstMaterials.end(); ++it)
    {
        pMaterial = *it;/*m_vtMaterialSets[i]->GetMaterial(szName)*/;
        if (strcmp(pMaterial->GetName().c_str(), szName) == 0)
        {
            return pMaterial;
        }
    }

    return NULL;
}

IMaterial* CMaterialManager::GetMaterial(const AIRString& strName)
{
    IMaterial* pMaterial = NULL;
    for (MATERIAL_LIST_ITER it = m_lstMaterials.begin(); it != m_lstMaterials.end(); ++it)
    {
        pMaterial = *it;/*m_vtMaterialSets[i]->GetMaterial(szName)*/;
        if (pMaterial->GetName() == strName)
        {
            return pMaterial;
        }
    }

    return pMaterial;
}

void CMaterialManager::Clear()
{
    IMaterial* pMaterial = NULL;
    for (MATERIAL_LIST_ITER it = m_lstMaterials.begin(); it != m_lstMaterials.end(); ++it)
    {
        pMaterial = *it;
        if (pMaterial)
        {
            pMaterial->Clear();
        }
        
        SafeDelete(pMaterial);
    }
    m_lstMaterials.clear();
}

void CMaterialManager::ClearMaterial(IMaterial*& pMaterial)
{
    IMaterial* pMaterialExist = NULL;
    for (MATERIAL_LIST_ITER it = m_lstMaterials.begin(); it != m_lstMaterials.end(); ++it)
    {
        pMaterialExist = *it;
        if (pMaterial == pMaterialExist)
        {
            pMaterial->Clear();
            SafeDelete(pMaterial);
            m_lstMaterials.erase(it);
            break;
        }
        
    }
}

IMaterial* CMaterialManager::CreateMaterial(const char* szName)
{
    IMaterial* pMaterialExist = GetMaterial(szName);
    
    
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    IMaterial* pMaterial = NULL;

    if (pMaterialExist)
    {
        pMaterial = new IMaterial; //pRenderer->CreateMaterial();
        pMaterial->DeepCopy(pMaterialExist);
    }
    else
    {
        pMaterial = new IMaterial;//pRenderer->CreateMaterial(szName);
        AIRString strFile, strMaterial;
        CStringUtil::SplitFilename(szName, strFile, strMaterial);
        if (!pMaterial->CreateFromFile(strFile.c_str(), strMaterial.c_str()))
        {
            pMaterial->Clear();
            SafeDelete(pMaterial);
        }
    }

    if (pMaterial)
    {
        m_lstMaterials.push_back(pMaterial);
    }
    
    return pMaterial;
}

void CMaterialManager::AddMaterial(IMaterial* pMaterial)
{
	for (MATERIAL_LIST_ITER it = m_lstMaterials.begin(); it != m_lstMaterials.end(); ++it)
	{
		if (pMaterial == *it)
		{
			return;
		}

	}

	m_lstMaterials.push_back(pMaterial);
}

int CMaterialManager::LoadResource(const char* szFilename, bool bLoaded /* = true */)
{
    /*
    CMarkupSTL xml;

    if (!xml.Load(szFilename))
    {
        return false;
    }

    if (!xml.FindElem())
    {
        return false;
    }

    IRenderer* pRenderer = ENGINE_INST->GetRenderer();

    xml.IntoElem();

    while (xml.FindElem())
    {
        IMaterial* pMaterial = AURO_NEW_VERBOSE CD3D9Material;
        if (pMaterial->LoadFromXML(xml))
        {
            AddMaterial(pMaterial);
        }
    }

    xml.OutOfElem();
    */
    //return true;
    return 0;
}

IMaterial* CMaterialManager::CreateMaterial()
{
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    IMaterial* pMaterial = new IMaterial;//pRenderer->CreateMaterial();
    m_lstMaterials.push_back(pMaterial);
    return pMaterial;
}

IMaterial* CMaterialManager::CreateFromTemplate(const char* szTemplate)
{
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();

    AIRString strTemplate = AIRString("TemplateMaterial.xml/") + szTemplate;

    IMaterial* pMaterial = new IMaterial; //pRenderer->CreateMaterial(strTemplate.c_str());
    AIRString strFile, strMaterial;
    CStringUtil::SplitFilename(strTemplate, strFile, strMaterial);
    if (!pMaterial->CreateFromFile(strFile.c_str(), strMaterial.c_str()))
    {
        pMaterial->Clear();
        SafeDelete(pMaterial);
    }

    m_lstMaterials.push_back(pMaterial);
    return pMaterial;
}


