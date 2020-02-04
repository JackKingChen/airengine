// ***************************************************************
//  File: GUISlider.h
//  Date: 2010-2-12
//  Author: liangairan
//  Description:  滑条控件
// ***************************************************************


#pragma once
#include "guibutton.h"

enum eBar_State
{
    eSlider_Normal = 0x00,
    eSlider_LButtonDown,
    eSlider_Hover,
};

//value为0-1之间
typedef bool (*OnBarDragCallBack)(float fValue);

class AIRGuiExport CGUISlider :
    public CGUIElement
{
public:
    friend class CGUIScrollBar;
public:
    CGUISlider(void);
    virtual ~CGUISlider(void);

    virtual void        Move(int dx, int dy);

    //set the current value of the slider
    virtual void        SetCurValue(double dwValue);

    //set the delta value of the slider
    virtual void        SetDeltaValue(double dwValue);

    virtual bool        OnMouseWheel(int dz);

    double              GetDeltaValue() const;

    double              GetCurValue() const;

    virtual void        SetRect(const Rectf& rect);

    //---------------------------------------------------------
    // 功能:  数值减少时的回调
    //---------------------------------------------------------
    virtual void        MinusCallBack();

    //---------------------------------------------------------
    // 功能:  数值增加时的回调
    //---------------------------------------------------------
    virtual void        PlusCallBack();

    virtual eGUIOrientation    GetOrient() const;

    virtual const AIRString& GetBarImageSrcStr() const
    {
        return m_szImgSrcBar;
    }

    virtual const AIRString& GetSliderBtnImageSrcStr() const
    {
        return m_szImgSrcSliderBtn;
    }

    virtual void SetOrientation(eGUIOrientation orient);

    virtual void SetBarImageSrc(const AIRString& strImageSrc);

    virtual void SetSliderBtnImageSrc(const AIRString& strImageSrc);

    virtual void SetAfterLoad();

    virtual CGUIButton*  GetSliderButton()
    {
        return m_pSliderBtn;
    }

    virtual void SetBarWidth(int nWidth);

    virtual void SetBarHeight(int nHeight);

    virtual int  GetBarWidth()
    {
        return m_nBarWidth;
    }

    virtual int  GetBarHeight()
    {
        return m_nBarHeight;
    }

    virtual int  GetSliderWidth()
    {
        return m_nSliderWidth;
    }

    virtual int  GetSliderHeight()
    {
        return m_nSliderHeight;
    }

    virtual void SetSliderWidth(int nWidth);

    virtual void SetSliderHeight(int nHeight);

    virtual void CreateDefault();

    virtual bool OnMouseMove(int x, int y);

    //设置拖动的控件
    //virtual void        SetRelatedCtrl(CGUIElement* pCtrl);

protected:
    virtual bool        CreateSelf(CGUIElement* pParent, CMarkupSTL& xml);

	virtual bool		SaveSelfToXML(CMarkupSTL& xml );

    //virtual void      OnStatusChange();

    virtual void        RenderSelf();

    //virtual bool        OnMouseMoveSelf(int x, int y);    

    //virtual bool        OnMouseMoveChild(int x, int y);

    virtual bool        OnLButtonDownSelf(int x, int y);

    virtual void        UpdateSelf();

    virtual void        CaculateSliderPosition();

    //计算相对值
    double              CaculateCurValue();

    void                AdjustSliderPosition();

    void                AdjustSliderBtnPosition();

    void                CaculateSliderRects();

    virtual void        NotifyTextureDestroySelf(ITexture* pTex)
    {
        if ( m_renderQuad.m_pTexture == pTex )
        {
            m_renderQuad.m_pTexture = NULL;
            m_szImgSrc.clear();
            m_szImgSrcBar.clear();
            m_szImgSrcSliderBtn.clear();
            m_strSliderBtn.clear();
        }
    }

    

private:
    OnBarDragCallBack   m_pDragBarFun;
    Rectf               m_rectBar;          //滑条区域
    Rectf               m_rectSlider;       //滑块，暂时不用
    Rectf               m_rectCenter;       //能拖动的区域
    Rectf               m_barUV;
    Rectf               m_sliderUV;
    SColor              m_colorBar;
    SColor              m_colorSlider;
    eGUIOrientation     m_byOrient;         // 滑块控件的放置方式( 水平放置 / 垂直放置 )
    u8                m_bySliderState;
    double              m_dCurValue;        // 当前值的相对量( 0.0 - 1.0 )
    double              m_dDeltaValue;      // 鼠标滚轮滚动时滑块移动的数值( 0.0 - 1.0 )
    //double              m_dPercent;         //移动百分比
    int                 m_nSliderWidth;     //slider的宽
    int                 m_nSliderHeight;    //slider的高
    int                 m_nBarWidth;
    int                 m_nBarHeight;
protected:
    //CGUIElement*        m_pRelatedCtrl;     //要拖动的控件

    //ITexture*     m_pBarTexture;      //滑块的纹理
	AIRString			m_szImgSrcBar;
	AIRString			m_szImgSrcSliderBtn;
    AIRString         m_strSliderBtn;    //控件名
    CGUIButton*         m_pSliderBtn;      //按钮
};
