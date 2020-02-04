// ***************************************************************
//  �ļ�: GUIProgressBar.h
//  ����: 2010-2-4
//  ����: liangairan
//  ˵��: 
// ***************************************************************

#pragma once
#include "guielement.h"

class AIRGuiExport CGUIProgressBar :  public CGUIElement
{
    //enum PROGRESSBAR_ORIENT
    //{
    //    ORIENT_HORZ,
    //    ORIENT_VERT,
    //};

public:
    CGUIProgressBar(void);
    virtual ~CGUIProgressBar(void);

    virtual CGUIElement*    Clone();
    //virtual bool   Create(TiXmlElement* pElement);

    //virtual void Render();
    virtual void            Move(int dx, int dy);

    virtual void            SetValue(float fValue);

	virtual float			GetValue( void )
	{
		return m_fCurValue;
	}

    virtual eGUIOrientation GetOrient()
    {
        return m_byOrient;
    }

    virtual const AIRString& GetImageSrcBar() const
    {
        return m_szImgSrcBar;
    }

    virtual void SetBarImageSrc(const AIRString& strImageSrc);

    virtual void SetOrientation(eGUIOrientation orient);

    virtual void SetRect(const Rectf& rect);

    virtual void SetWidth(int nWidth);

    virtual void SetHeight(int nHeight);
protected:
    virtual void            RenderSelf();

    virtual void            UpdateSelf();
    //virtual void RenderChild();
    virtual bool            CreateSelf(CGUIElement* pParent, CMarkupSTL& xml);

	virtual bool			SaveSelfToXML(CMarkupSTL& xml );

    virtual void            NotifyTextureDestroySelf(ITexture* pTex)
    {
        if ( m_renderQuad.m_pTexture == pTex )
        {
            m_renderQuad.m_pTexture = NULL;
            m_szImgSrc.clear();
            m_szImgSrcBar.clear();
        }
    }

    //���������λ��
    virtual void CaculateBarPosition();

    //virtual void UpdateBarQuad();
    //virtual bool CreateChild(TiXmlElement* pParent);
private:

    float           m_fCurValue;   // ��ǰֵ
    eGUIOrientation m_byOrient;    // ����ؼ��ķ��÷�ʽ( ˮƽ���� / ��ֱ���� )
    bool            m_bCanBeSet;   // �ؼ��ĵ�ǰֵ�Ƿ��ܱ��������
    Rectf           m_RectBar;      // �ؼ�����Χ��
    u32           m_dwColorBar;  // �ؼ�����ɫ
    //Color4i m_ColorBar;   
    Rectf           m_RectBarUV;  // �ؼ�������UV

private:
    Rectf           m_RectTemp;   // �ؼ�����Χ��
    Rectf           m_RectTempUV; // �ؼ�������UV

    //hgeQuad       m_barQuad;    //bar����

    //u8         m_byOrient;    //����
	AIRString		m_szImgSrcBar;
};
