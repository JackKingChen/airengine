// ***************************************************************
//  File: GUIButton.h
//  Date: 2010-2-12
//  Author: liangairan
//  Description: ��ť�ؼ�
// ***************************************************************

#pragma once

#include "GUIElement.h"

class AIRGuiExport CGUIButton : public CGUIElement
{
public:
    CGUIButton();
    ~CGUIButton();

    // ��¡����
    virtual CGUIElement*    Clone();

    // ���
    virtual void            CleanUp();

    virtual SColor          GetColorUp()
    {
        return m_ColorUp;
    }

    virtual SColor          GetColorDown()
    {
        return m_ColorDown;
    }

    virtual SColor          GetColorHover()
    {
        return m_ColorHover;
    }

    virtual SColor          GetColorGray()
    {
        return m_ColorInactive;
    }

    const AIRString& GetImgSrcUpStr() const
    {
        return m_szImgSrcUp;
    }

    const AIRString& GetImgSrcDownStr() const
    {
        return m_szImgSrcDown;
    }

    const AIRString& GetImgSrcHoverStr() const
    {
        return m_szImgSrcHover;
    }

    const AIRString& GetImgSrcGrayStr() const
    {
        return m_szImgSrcInactive;
    }

	virtual void SetImgSrcUp( const AIRString& strImgSrcUp );

    virtual void SetImgSrcDown(const AIRString& strImgSrcDown);

    virtual void SetImgSrcHover(const AIRString& strImgSrcHover);

    virtual void SetImgSrcGray(const AIRString& strImgSrcGray);

    virtual void SetColorDown(const SColor color);

    virtual void SetColorHover(const SColor color);

    virtual void SetColorGray(const SColor color);

    virtual void AutoSize();

protected:

    // ע�ᵽ�ű�
    virtual void            RegisterToScript();

    // ��������
    virtual bool            CreateSelf(CGUIElement* pParent, CMarkupSTL& xml);

    // ��Ⱦ����
    virtual void            RenderSelf();
    
    // ״̬�ı�
    virtual void            OnStatusChange();

	virtual bool			SaveSelfToXML(CMarkupSTL& xml );

    virtual void            NotifyTextureDestroySelf(ITexture* pTex)
    {
        if ( m_renderQuad.m_pTexture == pTex )
        {
            m_renderQuad.m_pTexture = NULL;
            m_szImgSrc.clear();
            m_szImgSrcDown.clear();
            m_szImgSrcHover.clear();
            m_szImgSrcInactive.clear();
            m_szImgSrcUp.clear();
        }
    }


protected:
    string                  m_strScript;            // �ű�
    Rectf                   m_UVUp;                 // ��ť����ʱ������UV
    Rectf                   m_UVDown;               // ��ť����ʱ������UV
    Rectf                   m_UVHover;              // ��������ڰ�ť��ʱ������UV
    Rectf                   m_UVInactive;           // ��ť������ʱ������UV
    SColor                  m_ColorUp;              // ��ť����ʱ����ɫ
    SColor                  m_ColorDown;            // ��ť����ʱ����ɫ
    SColor                  m_ColorHover;           // ��������ڰ�ť��ʱ����ɫ
    SColor                  m_ColorInactive;        // ��ť������ʱ����ɫ

private:
	AIRString				m_szImgSrcUp;
	AIRString				m_szImgSrcDown;
	AIRString				m_szImgSrcHover;
	AIRString				m_szImgSrcInactive;
};

