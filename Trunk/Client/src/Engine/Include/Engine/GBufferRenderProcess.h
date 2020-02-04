/********************************************************************
	created:	2010/08/08
	created:	8:8:2010   15:46
	filename: 	GBufferRenderProcess.h
	author:		liangairan
	
	purpose:	‰÷»æµΩGBuffer£¨”√”⁄—”≥Ÿ‰÷»æ
*********************************************************************/

#pragma once 

//#include "RenderTarget.h"
#include "ScreenCompositor.h"
#include "CustomRenderProcess.h"

class TextureRenderTarget;

class EngineExport GBufferRenderProcess : public CustomRenderProcess
{
public:
    GBufferRenderProcess();
    virtual ~GBufferRenderProcess();

    virtual void Render();

    virtual void Clear();

    virtual bool Initialize(int nWidth, int nHeight);

    //RenderTarget* GetNormalRenderTarget()
    //{
    //    return m_pRTTNormal;
    //}

    IMaterial* GetMaterial()
    {
        return m_pMaterial;
    }

    virtual CCamera* GetCamera();
protected:
    //virtual void UpdateImp();

    //virtual void RenderImpl();
	TextureRenderTarget* m_pRTTDepth;
    TextureRenderTarget* m_pRTTNormal;
    
    IMaterial*   m_pMaterial;
    //CTechnique*  m_pTechnique;
    //CTechnique*  m_pTechniqueClearGBuffer;

    ScreenCompositor  m_quad;
private:
};


