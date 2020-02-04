#include "GUIMouse.h"
#include "Engine.h"

CGUIMouse::CGUIMouse()
{

}

CGUIMouse::~CGUIMouse()
{

}


void CGUIMouse::Update()
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    POINT mouse;
    ::GetCursorPos(&mouse);
    ScreenToClient(ENGINE_INST->GetHWnd(), &mouse);
    SetRect(mouse.x, mouse.y, GetWidth(), GetHeight());
#endif
}

