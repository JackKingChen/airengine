/********************************************************************
	created:	2010/02/01
	created:	1:2:2010   11:55
	filename: 	d:\My Codes\client\Current\Engine\AIRGUI\GUIEditCtrl.h
	file path:	d:\My Codes\client\Current\Engine\AIRGUI
	file base:	GUIEditCtrl
	file ext:	h
	author:		liangairan
	
	purpose:	���ڱ༭�����ؼ��Ŀؼ�
*********************************************************************/

#pragma once 

#include "GUIElement.h"

class AIRGuiExport CGUIEditCtrl : public CGUIElement
{
public:
    enum eClickSide
    {
        Side_Out = 0,
        Side_Inner,
        Side_Topleft,
        Side_TopMiddle,
        Side_TopRight,
        Side_RightMiddle,
        Side_BottomRight,
        Side_BottomMiddle,
        Side_BottomLeft,
        Side_LeftMiddle,
    };
public:
    CGUIEditCtrl();
    virtual ~CGUIEditCtrl();

    virtual void Move(int dx, int dy);

    //�ı��С
    virtual void Resize(int dx, int dy);

    virtual void AttachEditElement(CGUIElement* pElement);

    eClickSide   GetClickSide(int x, int y);

    eClickSide   GetClickSide()
    {
        return m_clickType;
    }

    virtual void SetClickSide(eClickSide side);

    CGUIElement* GetAttachElement()
    {
        return m_pEditElement;
    }

    virtual void Update();

    virtual void Render();
protected:

    virtual void RenderSelf();

    virtual bool OnLButtonDownSelf(int x, int y);

    virtual bool OnLButtonUpSelf(int x, int y);

    virtual bool OnMouseMoveSelf(int x, int y);

    //����ߵ�С���ε�λ��
    virtual void CaculateSideRectPos();

    eClickSide   IsMouseInSideRect(int x, int y);

    Rectf  m_rectTopLeft;
    Rectf  m_rectTopMiddle;
    Rectf  m_rectTopRight;
    Rectf  m_rectRightMiddle;
    Rectf  m_rectBottomRight;
    Rectf  m_rectBottomMiddle;
    Rectf  m_rectBottomLeft;
    Rectf  m_rectLeftMiddle;

    CGUIElement*  m_pEditElement;

    bool          m_bMouseClickInSideRect;  //����Ƿ���ڱ߾�����
    eClickSide    m_clickType;
private:
};
