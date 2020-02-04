// ***************************************************************
//  文件: GUIProgressBar.h
//  日期: 2010-2-4
//  作者: liangairan
//  说明: 
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

    //计算出条的位置
    virtual void CaculateBarPosition();

    //virtual void UpdateBarQuad();
    //virtual bool CreateChild(TiXmlElement* pParent);
private:

    float           m_fCurValue;   // 当前值
    eGUIOrientation m_byOrient;    // 滑块控件的放置方式( 水平放置 / 垂直放置 )
    bool            m_bCanBeSet;   // 控件的当前值是否能被鼠标设置
    Rectf           m_RectBar;      // 控件条包围框
    u32           m_dwColorBar;  // 控件条颜色
    //Color4i m_ColorBar;   
    Rectf           m_RectBarUV;  // 控件条纹理UV

private:
    Rectf           m_RectTemp;   // 控件条包围框
    Rectf           m_RectTempUV; // 控件条纹理UV

    //hgeQuad       m_barQuad;    //bar绘制

    //u8         m_byOrient;    //方向
	AIRString		m_szImgSrcBar;
};
