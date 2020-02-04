// ***************************************************************
//  File: GUIAnimation.h
//  Date: 2010-2-4
//  Author: liangairan
//  Description: 动画控件
// ***************************************************************

#pragma once
#include "Common.h"
#include "GUIElement.h"
#include "ImageSet.h"

class AIRGuiExport CGUIAnimation : public CGUIElement
{
public:
    CGUIAnimation();
    virtual ~CGUIAnimation();

    // 更新
    virtual void Update();

    bool IsLoop()
    {
        return m_bLoop;
    }

    int  GetFrameRate()
    {
        return m_nFrameRate;
    }

    void SetLoop(bool bLoop);

    void SetFrameRate(int nFrameRate);

	// for editor 
	void SetFrames( AIRList<AIRString>& frameInfo );

	void GetFrames( AIRList<AIRString>& frameInfo );

    CGUIAnimation& operator = (const CGUIAnimation& other)
    {
        m_renderQuad.m_rectScreen = other.m_renderQuad.m_rectScreen;
        m_realRect = other.m_realRect;
        m_renderQuad.m_uv = other.m_renderQuad.m_uv;
        m_bLoop = other.m_bLoop;
        m_fFrameChangeTime = other.m_fFrameChangeTime;
        m_fLastRenderTime = other.m_fLastRenderTime;
        m_nFrameRate = other.m_nFrameRate;
        m_nCurFrame = other.m_nCurFrame;
        m_renderQuad.m_pTexture = other.m_renderQuad.m_pTexture;
        m_vtFrames = other.m_vtFrames;
        return *this;
    }

    virtual void CopyFrom(const CGUIElement* pOther);

protected:
    // 渲染本身
    virtual void RenderSelf();

    virtual void NotifyTextureDestroySelf(ITexture* pTex)
    {
        if ( m_renderQuad.m_pTexture == pTex )
        {
            m_renderQuad.m_pTexture = NULL;
            m_szImgSrc.clear();
            m_vtFrames.clear();
        }
    }

    // 创建自身
    virtual bool CreateSelf( CGUIElement* pParent, CMarkupSTL& xml );

	virtual bool CreateChild(CMarkupSTL& parent);

	virtual bool SaveSelfToXML(CMarkupSTL& xml );

private:
    bool						m_bLoop;                //循环播放
    int							m_nFrameRate;           //帧率
    float						m_fFrameChangeTime;     //每帧时间间隔
    float						m_fLastRenderTime;      //上次渲染时间
    int							m_nCurFrame;            //当前帧
	AIRVector<CImageUnit*>    m_vtFrames;             //每一帧的子图
};
