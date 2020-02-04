#pragma once
#include "IFontBase.h"
#include "GUIElement.h"

const int TWINKLE_TIME = 1000;  //闪烁时间


class AIRGuiExport CGUIWord : public CGUIElement
{
public:
    CGUIWord(wchar_t wCode, IFontBase* pFont, SColor color = 0xffffffff, int nBorder = 0, SColor clrBorder = SColor(0xff000000), bool bTwinkle = false);
    virtual ~CGUIWord(void);

    void            SetScreenPos(int nX, int nY);

    //获得字体占像素的宽度，并不是字的实际宽度
    int             GetWordWidth() const;

    //获得字体占像素的高度
    int             GetWordHeight() const;

    virtual void    Move(int dx, int dy);

    virtual void    MoveTo(int nPosX, int nPosY);

    wchar_t         GetCode();

    LPWORDINFO      GetWordInfo();

    //获得字体的矩形
    void            GetFontRect(Rectf& rect);

protected:
    virtual void    RenderSelf();

    virtual void    UpdateSelf();

private:
    LPWORDINFO  m_pWord;
    IFontBase*  m_pFont;
    bool        m_bTwinkle;   //闪烁
    bool        m_bTwinkleDirect;
    u32       m_dwTwinkleStart;
    int         m_nBorder;    //字体描边
    SColor      m_clrBorder;  //描边颜色

    Rectf       m_fontRect;  //实际渲染矩形
    //Rect      m_screenRect;  //屏幕空间矩形
};
