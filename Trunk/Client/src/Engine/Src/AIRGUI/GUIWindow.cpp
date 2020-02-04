//#include "stdafx.h"
#include "GUIWindow.h"
//#include "TextureManager.h"
//#include "UILuaScriptMgr.h"
//#include "InputSystem.h"

CGUIWindow::CGUIWindow() : m_bDrag(false)
{
    //m_ctrlType = eCtrl_Window;
    m_byCurKeyState = UI_KEY_UP;
    m_byLastKeyState = UI_KEY_UP;
    m_strType = "CGUIWindow";
}

CGUIWindow::~CGUIWindow()
{

}

CGUIElement* CGUIWindow::Clone()
{
    return NULL;
}

void CGUIWindow::RegisterToScript()
{
    //UILUASCRIPTMGR->Declare(this->m_strName.c_str(), this);
}


bool CGUIWindow::CreateSelf(CGUIElement* pParent, CMarkupSTL& xml)
{
    CGUIAutoSizeFrame::CreateSelf(pParent, xml);

    return true;
}

/*
void CGUIWindow::Render()
{
    CGUIElement::Render();
}


void CGUIWindow::UpdateSelf()
{
    //CGUIElement::Update();

    if (INPUTSYSTEM->KeyDown(DIK_0))
    {
        m_byCurKeyState = UI_KEY_DOWN;
        
    }
    else 
        m_byCurKeyState = UI_KEY_UP;

    if (m_byCurKeyState != m_byLastKeyState && m_byCurKeyState == UI_KEY_UP)
    {
        OnStatusChange();
    }

    m_byLastKeyState = m_byCurKeyState;
}
*/

/*
bool CGUIWindow::OnMouseMoveSelf(int x, int y)
{
    
    int dx, dy;
    INPUTSYSTEM->GetMovement(dx, dy);
    if (INPUTSYSTEM->LButtonDown())
    {
        Move(dx, dy);
    }
    
}*/

/*
void CGUIWindow::RenderSelf()
{
    
}
*/
