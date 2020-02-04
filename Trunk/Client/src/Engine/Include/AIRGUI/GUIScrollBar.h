/********************************************************************
	created:	2009/11/16
	created:	16:11:2009   12:05
	filename: 	d:\My Codes\Engine\AIRGUI\GUIScrollBar.h
	file path:	d:\My Codes\Engine\AIRGUI
	file base:	GUIScrollBar
	file ext:	h
	author:		liangairan
	
	purpose:	ScrollBar的控件类，这里要注意一点，它是一个组合组件，
                组合组件相当于CGUIElement那样，必须要重写非Self的函数，
*********************************************************************/

#pragma once
#include "guielement.h"
#include "GUISlider.h"
#include "GUIButton.h"

class CGUIScrollBar :
    public CGUIElement
{
public:
    CGUIScrollBar(void);
    virtual ~CGUIScrollBar(void);

    virtual bool Create(CGUIElement* pParent, CMarkupSTL& xml);

	virtual bool SaveSelfToXML(CMarkupSTL& xml );

    //virtual bool IsInRect(int x, int y);

    //设置关联的控件
    virtual void SetRelatedCtrl(CGUIElement* pElement);

    virtual void SetRelatedCtrl(const char* szCtrlName);

	const AIRString& GetRelatedCtrlName( void )
	{
		return m_strRelatedCtrl;
	}

    //这些函数必须要重写，因为它是复合组件
    virtual bool OnMouseWheel(int dz);

    virtual bool OnMouseMove(int x, int y);

    virtual bool OnLButtonUp(int x, int y);

    virtual bool OnLButtonDown(int x, int y);

    virtual CGUIButton* GetButtonStart() 
    {
        return m_pBtnStart;
    }

    virtual CGUIButton* GetButtonEnd()
    {
        return m_pBtnEnd;
    }

    virtual CGUISlider* GetSlider()
    {
        return m_pSlider;
    }

    //
    virtual void CreateDefault();
    
    //设置上下button的宽
    virtual void SetButtonWidth(int nWidth);

    //设置上下button的高
    virtual void SetButtonHeight(int nHeight);

    //设置slider的Button大小
    virtual void SetSliderBtnWidth(int nWidth);

    virtual void SetSliderBtnHeight(int nHeight);

    virtual void SetRect(const Rectf& rect);

    

protected:
    //virtual void ClearChild();
    CGUIButton* m_pBtnStart;
    CGUIButton* m_pBtnEnd;
    CGUISlider* m_pSlider;

    CGUIElement* m_pRelatedElement;   //关联的GUI元素

    //修正子控件的位置
    virtual void ModifyChildrenPosition();
protected:
    virtual bool CreateSelf(CGUIElement* pParent, CMarkupSTL& xml);
    
    virtual void SetAfterLoadSelf();

    //计算位置
    virtual void CalculateRelatedCtrlPos();

    virtual void NotifyTextureDestroyChild(ITexture* pTex)
    {
        m_pBtnStart->NotifyTextureDestroy( pTex );
        m_pBtnEnd->NotifyTextureDestroy( pTex );
        m_pSlider->NotifyTextureDestroy( pTex );
        CGUIElement::NotifyTextureDestroyChild( pTex );
    }

    //更新自己，主要是根据内容调整slider的位置
    virtual void UpdateSelf();

    //virtual bool OnMouseMoveSelf(int x, int y);

    //virtual bool OnMouseWheelSelf();
private:
    //子控件的名字
    AIRString m_strBtnUp;    //向上的button
    AIRString m_strBtnDown;  //向下的button
    AIRString m_strSlider;   //滑条的名字
    AIRString m_strRelatedCtrl;   //相关的控件名
    int         m_nRelatedCtrlOffset;    //可移动长度，实际上是关联控件真正高度与scrollbar高度的差值
};
