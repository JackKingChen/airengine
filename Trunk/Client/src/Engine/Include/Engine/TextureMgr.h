#pragma once

#include "ITexture.h"
#include "Common.h"
#include "AppConfig.h"
#include "Singleton.h"

typedef std::map<AIRString, ITexture*>	MAP_TEXTURE;
typedef MAP_TEXTURE::iterator				MAP_TEXTURE_ITOR;


class EngineExport CTextureMgr : public Singleton<CTextureMgr>
{
    BE_SINGLETON(CTextureMgr);
public:
    
    ~CTextureMgr();

	//--------------------------------------------------
	//! add by hjj 2010-6-29
	virtual ITexture*	CreateTextureFromFile( const char* szFileName);

    virtual ITexture*   CreateTextureFromFile(const char* szFileName, const TextureParam& param);

    virtual ITexture*   CreateTexture(const char* szName, const TextureParam& param);

    virtual ITexture*	GetTexture(const AIRString& strName);

    //virtual void		ReleaseTexture(s32 nID) = 0;
    
    //清空所有纹理
    virtual void		Clear();

    virtual void		ClearTexture(ITexture*& pTex);


protected:
    virtual void		AddTexture(ITexture* pTexture);

    virtual void		RemoveTexture(ITexture* pTexture);

    //virtual void		RemoveTexture(s32 nID);
    //virtual void		DeleteTexture(ITexture* pTexture);


protected:
    MAP_TEXTURE			m_mapTex;
    u32					m_nTexCount;

};

#define TEXTURE_MANAGER CTextureMgr::GetInstance()
