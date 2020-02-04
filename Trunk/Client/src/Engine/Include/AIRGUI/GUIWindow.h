#pragma once

#include "GUIAutoSizeFrame.h"

class CGUIWindow : public CGUIAutoSizeFrame
{
public:
    CGUIWindow();
    ~CGUIWindow();

    virtual CGUIElement* Clone();

    //virtual bool  Create(TiXmlElement* pElement);

    //virtual void  Render();

    //virtual void    CleanUp();

    //virtual void  UpdateSelf();

    
protected:
    virtual bool    CreateSelf(CGUIElement* pParent, CMarkupSTL& xml/*TiXmlElement* pElement*/);

    virtual void    RegisterToScript();

    //virtual void  OnStatusChange();

    //virtual void  OnLButtonDownSelf(int x, int y);

    //virtual bool  OnMouseMoveSelf(int x, int y);
    //virtual void  RenderSelf();

protected:
    bool  m_bDrag;   //�Ƿ�����϶�
    bool  m_bActive; //�Ƿ񱻻���
private:

    u8  m_byCurKeyState;
    u8  m_byLastKeyState;

    
};
