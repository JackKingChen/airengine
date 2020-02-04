#pragma once 

//#include "Renderable.h"
#include "Rect.h"
//#include "ITexture.h"
//#include "hge.h"
//#include "hgecolor.h"
#include "Common.h"

class CWordRenderable
{
public:
    CWordRenderable();

    CWordRenderable(wchar_t code);
    ~CWordRenderable();
    virtual void    Render();

    //virtual void  Render(ITexture hTex, const hgeColorRGB& color = hgeColorRGB(1.0f, 1.0f, 1.0f, 1.0f));

    virtual void    CleanUp();

    virtual void    Update();

    wchar_t         GetCode() { return m_wCode; }

    void            SetUVRect(const Rectf& rectUV);
    void            SetRectScreen(const Rect& rectScreen);

    u32             GetWidth() { return m_nWidth; }
    u32             GetHeight() { return m_nHeight; }

    void            SetSize(u32 nWidth, u32 nHeight);
    void            SetScreenPos(s32 nX, s32 nY);
protected:
private:
    void            UpdateRenderQuad();

private:
    Rectf           m_rectUV;
    Rect           m_rectScreen;   //屏幕绘制区域
    u32             m_nWidth;             //字体的宽度
    u32             m_nHeight;            //字体的高度
    wchar_t         m_wCode;
};
