/********************************************************************
	created:	2010/08/05
	created:	5:8:2010   18:42
	filename: 	LightingManager.h
	author:		liangairan
	
	purpose:	���չ��������������ӳ���Ⱦ
*********************************************************************/

#pragma once 

#include "Light.h"
#include "Singleton.h"

#include <set>
#include "RenderTarget.h"
#include "CustomRenderProcess.h"
#include "ScreenCompositor.h"
#include "Billboard.h"

class DepthStencil;
class FullScreenQuad;

class DeferredLightingGBuffer;
class DeferredLightingLightBuffer;

class EngineExport LightingManager : public Singleton<LightingManager>
{
    BE_SINGLETON(LightingManager);
public:
    typedef std::set<CLight*> LightList;
    typedef LightList::iterator LightListIter;
    
    ~LightingManager();

    void AddCurrentLight(CLight* pLight);

    //���£�����Ҫ�ڳ������º�����ˡ�
    void Update();

    void Clear();


    //RenderTarget* GetLightTarget()
    //{
    //    return m_pLightTarget;
    //}

    //RenderTarget* GetRenderTargetGBuffer()
    //{
    //    return m_pRenderTargetGBuffer;
    //}

    size_t GetCurrentLightsNum() const
    {
        return m_currentLights.size();
    }

    SetIterator<LightList> GetLightListIter( void )
    {
        return SetIterator<LightList>(m_currentLights);
    }

    //�����ӳ���Ⱦ
    void EnableDeferredLighting(bool bEnable);

    //debugר��
    void SaveRenderTargetToFile();

	DeferredLightingGBuffer* GetDeferredLightingGBuffer()
	{
		return m_pDeferredLightingGBuffer;
	}

	DeferredLightingLightBuffer* GetDeferredLightingLightBuffer()
	{
		return m_pDeferredLightingLightBuffer;
	}
protected:

	void CreateLightTargetAndGBuffer();
private:

    LightList  m_currentLights;   //��ǰ����ʹ�õ���light

    //RenderTarget*  m_pLightTarget;   //��Ⱦ��lightbuffer
    //RenderTarget*  m_pRenderTargetGBuffer;

	DeferredLightingGBuffer* m_pDeferredLightingGBuffer;
	DeferredLightingLightBuffer* m_pDeferredLightingLightBuffer;
};

#define LIGHTING_MANAGER LightingManager::GetInstance()



class EngineExport DeferredLightingGBuffer : public CustomRenderProcess
{
public:
	DeferredLightingGBuffer();
	virtual ~DeferredLightingGBuffer();

	virtual void Render();

	virtual void Clear();

	TextureRenderTarget* GetDepthTargetTexture()
	{
		return m_pDepthTarget;
	}

	TextureRenderTarget* GetNormalTargetTexture()
	{
		return m_pNormalTarget;
	}
protected:
private:
	TextureRenderTarget* m_pDepthTarget;
	TextureRenderTarget* m_pNormalTarget;
	
	IMaterial*   m_pMaterial;
	//CTechnique*  m_pTechnique;
	//CTechnique*  m_pTechniqueClearGBuffer;

	DepthStencil* m_pDepthStencil;

	ScreenCompositor  m_quad;
};

class EngineExport DeferredLightingLightBuffer : public CustomRenderProcess
{
public:
	DeferredLightingLightBuffer();
	virtual ~DeferredLightingLightBuffer();

	virtual void Render();

    virtual void Clear();
protected:
private:
	IMaterial* m_pMaterial;
	//CTechnique*  m_pPointLightTechnique;
	//CTechnique*  m_pDirectionalLightTechnique;

	Vector3Df  m_farPlaneCorners[4];
	CBillboard        m_billBoard;

	FullScreenQuad*  m_pFullScreenQuad;

	TextureRenderTarget* m_pLightTarget;
};
