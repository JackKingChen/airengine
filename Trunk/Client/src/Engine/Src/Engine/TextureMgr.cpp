#include "stdafx.h"
//#include "stdafx.h"
#include "TextureMgr.h"
#include "Engine.h"

CTextureMgr::CTextureMgr() : m_nTexCount(0)
{
}

CTextureMgr::~CTextureMgr()
{

}

ITexture* CTextureMgr::GetTexture(const AIRString& strName)
{
    MAP_TEXTURE_ITOR it = m_mapTex.find(strName);
    if (it != m_mapTex.end())
    {
        return it->second;
    }
   
    return NULL;
}

void CTextureMgr::AddTexture(ITexture* pTexture)
{
    m_mapTex.insert( make_pair(pTexture->GetName(), pTexture) );
}

void CTextureMgr::RemoveTexture(ITexture* pTexture)
{
    MAP_TEXTURE_ITOR it = m_mapTex.find(pTexture->GetName());
	if ( it != m_mapTex.end() )
	{
		m_mapTex.erase(it);
	}    
}

void CTextureMgr::ClearTexture(ITexture*& pTex)
{
    if (pTex == NULL)
    {
        return;
    }

    MAP_TEXTURE_ITOR it = m_mapTex.find(pTex->GetName());
    if (it != m_mapTex.end())
    {
        if (pTex->Release() == 0)
        {
            SafeDelete(pTex);
            m_mapTex.erase(it);
            return;
        }
    }
}

void CTextureMgr::Clear()
{
    for (MAP_TEXTURE_ITOR it = m_mapTex.begin(); it != m_mapTex.end(); it++)
    {
        ITexture* pTexture = it->second;
        pTexture->Clear();
        SafeDelete(pTexture);
    }

    m_mapTex.clear();
}

ITexture*	CTextureMgr::CreateTextureFromFile( const char* szFileName/*, int nMipMap*/ )
{
    if (strlen(szFileName) == 0)
    {
        return NULL;
    }
    ITexture* pTexture = GetTexture(AIRString(szFileName));
    if (pTexture)
    {
        pTexture->AddRef();
    }
    else
    {
        pTexture = RENDERER->CreateTexture();
        if (pTexture->CreateFromFile(szFileName))
        {
            AddTexture(pTexture);
            //return pD3D9Texture;
        }
        else
        {
            SafeDelete(pTexture);
        }
    }
    return pTexture;
}

ITexture* CTextureMgr::CreateTextureFromFile(const char* szFileName, const TextureParam& param)
{
    if (strlen(szFileName) == 0)
    {
        return NULL;
    }
    ITexture* pTexture = GetTexture(AIRString(szFileName));
    if (pTexture)
    {
        pTexture->AddRef();
    }
    else
    {
        pTexture = RENDERER->CreateTexture();
        if (pTexture->CreateFromFile(szFileName, param))
        {
            AddTexture(pTexture);
            //return pD3D9Texture;
        }
        else
        {
            SafeDelete(pTexture);
        }
    }
    return pTexture;
}

ITexture* CTextureMgr::CreateTexture(const char* szName, const TextureParam& param)
{
    ITexture* pTexture = GetTexture(szName);
    if (pTexture)
    {
        pTexture->AddRef();
    }
    else
    {
        pTexture = RENDERER->CreateTexture();
        if (pTexture->Create(szName, param))
        {
            AddTexture(pTexture);
        }
        else
        {
            SafeDelete(pTexture);
        }
    }
    return pTexture;
}
