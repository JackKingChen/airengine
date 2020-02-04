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

    //��дAddChild
    virtual void    AddChild(CGUIElement* pChild);

    //�Ƿ�ѡ��
    bool IsSelected()
    {
        return m_bSelected;
    }

    void SetSelected(bool bSelected);

    virtual void    SetWordsColor(const SColor& color);

protected:
    virtual void RenderSelf();

protected:
    //����Align�ĺ���
    void            SetWordPosMiddle();
    void            SetWordPosLeft();
    void            SetWordPosRight();

    //���element��Ҫ���¼��������ӿؼ���λ�á�
    //@nBottom  ���еĿؼ��ƶ��������һ����
    void            RecaculateChildPosition(int nOffsetY);
private:
    int             m_nLineIndex;   //�кţ���0��ʼ
    bool            m_bSelected;    //�Ƿ�ѡ��
    SColor          m_clrSelected;
    //typedef std::vector<CGUIElement*> GUIELEMENT_LIST;
    //typedef GUIELEMENT_LIST::iterator GUIELEMENT_ITER;

    //GUIELEMENT_LIST  m_vtElement;
};
