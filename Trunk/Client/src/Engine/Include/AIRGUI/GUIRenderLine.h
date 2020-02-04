#pragma once
#include "GUIElement.h"

class AIRGuiExport CGUIRenderLine : public CGUIElement
{
public:
    CGUIRenderLine(void);
    virtual ~CGUIRenderLine(void);

    void            SetIndex(int nIndex);

	int				GetIndex( void )
	{
		return m_nLineIndex;
	}

    CGUIElement*    GetLastElement();

    //重写AddChild
    virtual void    AddChild(CGUIElement* pChild);

    //是否被选中
    bool IsSelected()
    {
        return m_bSelected;
    }

    void SetSelected(bool bSelected);

    virtual void    SetWordsColor(const SColor& color);

protected:
    virtual void RenderSelf();

protected:
    //处理Align的函数
    void            SetWordPosMiddle();
    void            SetWordPosLeft();
    void            SetWordPosRight();

    //添加element后要重新计算所有子控件的位置。
    //@nBottom  所有的控件移动到下面的一条线
    void            RecaculateChildPosition(int nOffsetY);
private:
    int             m_nLineIndex;   //行号，从0开始
    bool            m_bSelected;    //是否被选中
    SColor          m_clrSelected;
    //typedef std::vector<CGUIElement*> GUIELEMENT_LIST;
    //typedef GUIELEMENT_LIST::iterator GUIELEMENT_ITER;

    //GUIELEMENT_LIST  m_vtElement;
};
