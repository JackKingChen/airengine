//#include "StdAfx.h"
#include "GUIWord.h"
#include "Timer.h"
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
#include <MMSystem.h>
#endif

CGUIWord::CGUIWord(wchar_t wCode, IFontBase* pFont, SColor color, int nBorder, SColor clrBorder, bool bTwinkle) :// : CGUIElement::m_renderQuad.m_clrDiffuse(color),
m_pFont(pFont), 
m_bTwinkleDirect(true), 
m_dwTwinkleStart(Timer::GetTick()),
m_nBorder(nBorder),
m_clrBorder(clrBorder),
m_bTwinkle(bTwinkle)
{
    m_pWord = pFont->GetWordInfo(wCode);
	m_strType = "CGUIWord";
    m_renderQuad.m_clrDiffuse = color;

    if (m_pWord)
    {
        m_realRect.right = m_realRect.left + m_pWord->nWidth;
        m_realRect.bottom = m_realRect.top + m_pWord->nHeight;
        m_renderQuad.m_rectScreen = m_realRect;
    }
}

CGUIWord::~CGUIWord(void)
{
}

void CGUIWord::SetScreenPos(int nX, int nY)
{
    m_realRect.left = nX;
    m_realRect.top = nY;
    m_realRect.right = m_realRect.left + m_pWord->nWidth;
    m_realRect.bottom = m_realRect.top + m_pWord->nHeight;
    m_renderQuad.m_rectScreen = m_realRect;
    if (m_pWord)
    {
        m_fontRect.left = nX + m_pWord->nOffsetX;
        m_fontRect.top = nY + m_pWord->nOffsetY;
        m_fontRect.right = m_fontRect.left + m_pWord->nFontWidth;
        m_fontRect.bottom = m_fontRect.top + m_pWord->nFontHeight;
    }
    
}

int CGUIWord::GetWordWidth() const 
{
    return m_pWord->nWidth;
}

int CGUIWord::GetWordHeight() const
{
    return m_pWord->nWidth;
}

LPWORDINFO CGUIWord::GetWordInfo()
{
    return m_pWord;
}


void CGUIWord::RenderSelf()
{
    if (m_nBorder > 0)    //有描边的话
    {
        if (m_pFont->GetFontTypePtr()->m_nBorderSize == 0)
        {
            //画8次边先
            Rectf borderRect = m_fontRect;
            borderRect.Move(-1, -1);  //左上角
            m_pFont->Render(borderRect, m_pWord, m_pFrameLayer->m_fZ, m_clrBorder);
            borderRect.Move(1, 0);   //上方
            m_pFont->Render(borderRect, m_pWord, m_pFrameLayer->m_fZ, m_clrBorder);
            borderRect.Move(1, 0);   //右上方
            m_pFont->Render(borderRect, m_pWord, m_pFrameLayer->m_fZ, m_clrBorder);
            borderRect.Move(0, 1);   //右方
            m_pFont->Render(borderRect, m_pWord, m_pFrameLayer->m_fZ, m_clrBorder);
            borderRect.Move(0, 1);   //右下方
            m_pFont->Render(borderRect, m_pWord, m_pFrameLayer->m_fZ, m_clrBorder);
            borderRect.Move(-1, 0);   //正下方
            m_pFont->Render(borderRect, m_pWord, m_pFrameLayer->m_fZ, m_clrBorder);
            borderRect.Move(-1, 0);   //左下方
            m_pFont->Render(borderRect, m_pWord, m_pFrameLayer->m_fZ, m_clrBorder);
            borderRect.Move(0, -1);   //左方
            m_pFont->Render(borderRect, m_pWord, m_pFrameLayer->m_fZ, m_clrBorder);
        }
    }
    m_pFont->Render(m_fontRect, m_pWord, m_pFrameLayer->m_fZ, m_renderQuad.m_clrDiffuse);
    //wchar_t buf[1000];
    //wsprintfW( buf, L"%c, %x\n", m_pWord->wCode, m_renderQuad.m_clrDiffuse.color );
    //OutputDebugStringW( buf );
}

void CGUIWord::UpdateSelf()
{
    if (m_bTwinkle)
    {
        u32 dwCurrent = Timer::GetTick();
        u32 dwTwinkleDelta = Timer::GetTick() - m_dwTwinkleStart;
        dwTwinkleDelta = dwTwinkleDelta > TWINKLE_TIME ? TWINKLE_TIME : dwTwinkleDelta;


        if (m_bTwinkleDirect)   //
        {
            u32 alpha = (float)dwTwinkleDelta / TWINKLE_TIME * 255;
            m_renderQuad.m_clrDiffuse.SetAlpha(alpha);
        }
        else
        {
            u32 alpha = 255 - (float)dwTwinkleDelta / TWINKLE_TIME * 255;
            m_renderQuad.m_clrDiffuse.SetAlpha(alpha);
        }

        if (dwTwinkleDelta >= TWINKLE_TIME)
        {
            m_bTwinkleDirect = !m_bTwinkleDirect;
            m_dwTwinkleStart = dwCurrent;
        }
    }

}

void CGUIWord::Move(int dx, int dy)
{
    CGUIElement::Move(dx, dy);
    m_fontRect.Move(dx, dy);
}

void CGUIWord::MoveTo(int nPosX, int nPosY)
{
    CGUIElement::MoveTo(nPosX, nPosY);
    m_fontRect.MoveTo(nPosX, nPosY);
}

wchar_t CGUIWord::GetCode()
{
    if (m_pWord)
    {
        return m_pWord->wCode;
    }
    return 0;
}

void CGUIWord::GetFontRect(Rectf& rect)
{
    rect = m_fontRect;
}
