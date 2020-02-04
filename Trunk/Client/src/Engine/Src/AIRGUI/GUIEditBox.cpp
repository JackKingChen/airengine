#include "GUIEditBox.h"
#include "GUISystem.h"
//#include "SEFps.h"
#include "Engine.h"
#include "GUIRenderer.h"
//#include "InputSystem.h"
//#include <dinput.h>


CGUIEditBox::CGUIEditBox() : 
m_nCurState(eEDITBOX_NOTINPUT), 
m_dwCurRenderTime(0),
m_dwLastRenderTime(Timer::GetTick()), 
m_bRenderCursor(false), 
m_bPassword(false),
m_bMultiLine(false), 
m_nCursorOffset(0),
m_nCursorPosX(0),
m_nCursorPosY(0),
m_nLastState(eEDITBOX_NOTINPUT)
{
    memset(m_wcText, 0, sizeof(m_wcText));
    memset(m_wcTemp, 0, sizeof(m_wcTemp));
    memset(m_wcPassword, 0, sizeof(m_wcPassword));
    memset(m_szText, 0, sizeof(m_szText));
    //m_ctrlType = eCtrl_Edit;
    m_strType = "CGUIEditBox";
    m_dwMaxWords = 65535;
    m_lstWords.clear();
    m_curIt = m_lstWords.begin();
	m_bMoveable = false;
}

CGUIEditBox::~CGUIEditBox()
{

}

void CGUIEditBox::ClearChild()
{
    CGUIElement::ClearChild();
    for (GUIWORD_ITER it = m_lstWords.begin(); it != m_lstWords.end(); ++it)
    {
        (*it)->CleanUp();
        SafeDelete(*it);
    }

    m_strText.clear();

    m_lstWords.clear();

    m_nCursorOffset = 0;

    CacluateNoWordsCursorPos();
}

void CGUIEditBox::Update()
{
    UpdateSelf();
    for (GUIWORD_ITER it = m_lstWords.begin(); it != m_lstWords.end(); ++it)
    {
        (*it)->Update();
    }
}

void CGUIEditBox::RenderSelf()
{
    //先画一个矩形
    //GUISYSTEM->GetHGE()->Gfx_RenderRect(m_renderQuad.m_rectScreen.left, m_renderQuad.m_rectScreen.top, m_renderQuad.m_rectScreen.right, m_renderQuad.m_rectScreen.bottom, m_renderQuad.m_clrDiffuse);
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    DrawRenderQuad();
    //pRenderer->SetScissorEnable(true);
    m_rectScissor.left = (int)m_renderQuad.m_rectScreen.left;
    m_rectScissor.right = (int)m_renderQuad.m_rectScreen.right;
    m_rectScissor.top = (int)m_renderQuad.m_rectScreen.top;
    m_rectScissor.bottom = (int)m_renderQuad.m_rectScreen.bottom;
    GUIRENDERER->EnableScissor(m_rectScissor);
    //pRenderer->SetScissorRect(&m_rectScissor);
    if (m_bFocus/*m_nCurState == eEDITBOX_INPUT*/)
    {
        RenderCursor();
    }

    for (GUIWORD_ITER it = m_lstWords.begin(); it != m_lstWords.end(); ++it)
    {
        (*it)->Render();
    }
    GUIRENDERER->DisableScissor();
    //pRenderer->SetScissorEnable(false);

}

bool CGUIEditBox::CreateSelf(CGUIElement* pParent, CMarkupSTL& xml)
{
    CGUIElement::CreateSelf(pParent, xml);

    AIRString strValue = xml.GetAttrib("Password").c_str();
    if (strValue == "TRUE")
    {
        m_bPassword = true;
    }

    strValue = xml.GetAttrib("Align").c_str();
    if (strValue == "Align_Left")
    {
        m_align = eAlign_Left;
    }
    else if (strValue == "Align_Middle")
    {
        m_align = eAlign_Middle;
    }
    else if (strValue == "Align_MiddleTop")
    {
        m_align = eAlign_MiddleTop;
    }
    else 
    {
        m_align = eAlign_LeftTop;
    }

    strValue = xml.GetAttrib("MaxWord").c_str();
    if (strValue.length() > 0)
    {
        m_dwMaxWords = atoi(strValue.c_str());
    }

    if (xml.FindChildElem("GUIFont"))
    {
        xml.IntoElem();
        m_pFont = CreateGUIFont(xml);
        xml.OutOfElem();
    }

    CacluateNoWordsCursorPos();

    m_fontRect.left = m_nCursorPosX;
    m_fontRect.top = m_nCursorPosY;
    m_fontRect.right = m_renderQuad.m_rectScreen.right;
    m_fontRect.bottom = m_renderQuad.m_rectScreen.bottom;
    return true;
}


bool CGUIEditBox::SaveSelfToXML(CMarkupSTL& xml )
{
	CGUIStaticText::SaveSelfBaseToXML(xml);

	if ( m_bPassword )
	{
		xml.AddAttrib( "Password", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "Password", "FALSE" );
	}

	xml.AddAttrib( "MaxWord", GetString("%d",m_dwMaxWords) );

    if (m_pFont)
    {
        SaveFontToXML(xml, m_pFont);
    }
    
    /*
    xml.IntoElem();
	CGUIStaticText::SaveTextBlockToXML(xml);
    xml.OutOfElem();
    */

	return true;

}


void CGUIEditBox::OnStatusChange()
{
    
}

void CGUIEditBox::RenderCursor()
{
    //HGE* pHGE = GUISYSTEM->GetHGE();
    //FPS->GetTime();//pHGE->Timer_GetTime();
	u32 dwCurrentTime = Timer::GetTick();
    if ((int)dwCurrentTime - (int)m_dwLastRenderTime <= 500)
    {
        
    }
    else
    {
        m_dwLastRenderTime = dwCurrentTime;
        m_bRenderCursor = !m_bRenderCursor;
    }

    if (m_bRenderCursor)
    {
        //IRenderer* pRenderer = ENGINE_INST->GetRenderer();
        //pRenderer->Draw2DLine(m_nCursorPosX, m_nCursorPosY, m_nCursorPosX, m_nCursorPosY + m_pFont->GetWordHeight());
        m_rcCursor.left = m_nCursorPosX;
        m_rcCursor.right = m_rcCursor.left + 1.0f;
        m_rcCursor.top = m_nCursorPosY;
		if ( m_pFont )
		{
			m_rcCursor.bottom = m_rcCursor.top + m_pFont->GetWordHeight();
		}
		else
		{
			m_rcCursor.bottom = m_rcCursor.top + 12;
		}
        
        GUIRENDERER->Render(m_rcCursor, Rectf::ZERO, m_pFrameLayer->m_fZ, NULL );
    }
    
}

void CGUIEditBox::SetCursorPos(int x, int y)
{
    m_nCursorPosX = x;
    m_nCursorPosY = y;
}

void CGUIEditBox::UpdateString()
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    if (!m_bFocus)
    {
        return;
    }
    CIME* pIME = GUISYSTEM->GetIME();
    wchar_t* pIMEText = pIME->GetTextMessageW();

    if (pIMEText[0] == 0)
    {
        return;
    }

    if (m_nCursorOffset == wcslen(m_wcText))
    {
        wcscat(m_wcText, pIMEText);
        MoveCursorRight();
        //m_strText += pIMEText;
    }
    else
    {
        wchar_t* pStr = m_wcText + m_nCursorOffset;
        wcscpy(m_wcTemp, pStr);
        m_wcText[m_nCursorOffset] = 0;
        wcscat(m_wcText, pIMEText);
        wcscat(m_wcText, m_wcTemp);
        MoveCursorRight();
    }

    if (m_bPassword)
    {
        wcsncpy(m_wcPassword, L"********************************", wcslen(m_wcText));
    }
    
    //m_nCursorOffset += wcslen(pIMEText);
    
    pIME->ClearBuffer();
#endif
}

void CGUIEditBox::MoveCursorLeft()
{
    if (m_nCursorOffset == 0)
    {
        return;
    }
    //CWordRenderable* pWord = m_pFont->FindWord(m_wcText[--m_nCursorOffset]);
    //if (pWord)
    //{
    //    m_nCursorPosX -= pWord->GetWidth();
    //}
    
}

void CGUIEditBox::MoveCursorRight()
{
    if (m_nCursorOffset < (int)wcslen(m_wcText))
    {
        wchar_t wCH = m_wcText[m_nCursorOffset++];
        //CWordRenderable* pWord = m_pFont->AddWord(wCH);
        LPWORDINFO pWord = m_pFont->AddWord(wCH);
        
        if (pWord)
        {
            m_nCursorPosX += pWord->nWidth;//pWord->GetWidth();
        }
    }
}

void CGUIEditBox::ForwardDelete()
{
    if (m_nCursorOffset == wcslen(m_wcText))
    {
        return;
    }
    int nCurPos = m_nCursorOffset;
    //MoveCursorRight();
    wcscpy(m_wcTemp, m_wcText + nCurPos + 1);
    m_wcText[nCurPos] = 0;
    wcscat(m_wcText, m_wcTemp);
}

void CGUIEditBox::BackwardDelete()
{
    int nCurPos = m_nCursorOffset;
    MoveCursorLeft();
    wcscpy(m_wcTemp, m_wcText + nCurPos);
    m_wcText[nCurPos - 1] = 0;
    wcscat(m_wcText, m_wcTemp);

    if (m_bPassword)
    {
        m_wcPassword[nCurPos - 1] = 0;
    }
}

void CGUIEditBox::Move(int dx, int dy)
{
    CGUIElement::Move(dx, dy);
    m_nCursorPosX += dx;
    m_nCursorPosY += dy;

    m_fontRect.Move(dx, dy);

    MoveWords(dx, dy);
}

void CGUIEditBox::MoveWords(int dx, int dy)
{
    //for (u32 i = 0; i < m_lstWords.size(); i++)
    //{
    //    m_lstWords[i]->Move(dx, dy);
    //}
    for (GUIWORD_ITER it = m_lstWords.begin(); it != m_lstWords.end(); ++it)
    {
        (*it)->Move(dx, dy);
    }
}

/*
const char* CGUIEditBox::GetText()
{
    WCharToMByte(m_wcText, m_szText, sizeof(m_wcText));
    return m_szText;
}
*/

void CGUIEditBox::SetWords(const wchar_t* wszText)
{
    /*
    if (m_bFormated)
    {
        IGUIFormater* pFormater = GUISYSTEM->GetGUIFormater();
        if (pFormater)
        {
            GUIELEMENT_LIST lstElement;
            pFormater->FormatText(wszText, lstElement);

            for (GUIELEMENT_ITER it = lstElement.begin(); it != lstElement.end(); ++it)
            {
            }
        }
       
    }
    else
    */
    {
        //m_strText.insert(m_nCursorOffset, wszText);
        for (u32 i = 0; i < wcslen(wszText); i++)
        {
            if (m_lstWords.size() + i + 1 > m_dwMaxWords)   //达到最大数量
            {
                return;
            }
            if (wszText[i] == L'\r' && m_bMultiLine)   //回车符
            { 
                m_nCursorPosX = m_renderQuad.m_rectScreen.left;
                m_nCursorPosY += m_pFont->GetWordHeight();
                continue;
            }
            CGUIWord* pWord = new CGUIWord(wszText[i], m_pFont);
            pWord->SetScreenPos(m_nCursorPosX, m_nCursorPosY);
            if (m_nCursorOffset == m_lstWords.size())
            {
                m_lstWords.push_back(pWord);
            }
            else
            {
                //插入到字符串中去
                GUIWORD_ITER it = m_lstWords.begin();
                std::advance(it, m_nCursorOffset);//m_lstWords.begin() + m_nCursorOffset;
                //后面的全部向后移动
                for (GUIWORD_ITER itnext = it; itnext != m_lstWords.end(); itnext++)
                {
                    (*itnext)->Move(pWord->GetWordWidth(), 0);
                }
                m_lstWords.insert(it, pWord);
                
            }
            m_nCursorOffset++;

            m_nCursorPosX += pWord->GetWordWidth();
        }

        //m_strText += wszText;
    }
    

    ResetCursorPos();
}

void CGUIEditBox::ResetCursorPos()
{
    int nOffsetX = 0;
    int nOffsetY = 0;

    if (m_lstWords.size() == 0)
    {
        CacluateNoWordsCursorPos();
        return;
    }
    else if (m_nCursorOffset > 0)
    {
        GUIWORD_ITER lastIt = m_lstWords.begin();
        std::advance(lastIt, m_nCursorOffset - 1);
        CGUIWord* pPreWord = *lastIt; //m_lstWords[m_nCursorOffset - 1];
        Rectf rect;
        pPreWord->GetRealRect(rect);
        m_nCursorPosX = rect.right;
        m_nCursorPosY = rect.top;
    }
    while (m_nCursorPosX > m_renderQuad.m_rectScreen.right)
    {
        if (m_lstWords.size() > 0)
        {
            GUIWORD_ITER lastIt = m_lstWords.begin();
            std::advance(lastIt, m_nCursorOffset - 1);
            CGUIWord* pLastWord = *lastIt; //m_lstWords[m_nCursorOffset - 1];
            nOffsetX = -pLastWord->GetWordWidth();
            m_nCursorPosX += nOffsetX;
            for (GUIWORD_ITER it = m_lstWords.begin(); it != m_lstWords.end(); ++it)
            {
                (*it)->Move(nOffsetX, nOffsetY);
            }
            //for (size_t i = 0; i < m_lstWords.size(); i++)
            //{
            //    m_lstWords[i]->Move(nOffsetX, nOffsetY);
            //}
        }
    }

    while (m_nCursorPosX <= m_renderQuad.m_rectScreen.left && m_nCursorOffset > 0)
    {
        GUIWORD_ITER itNext = m_lstWords.begin();
        std::advance(itNext, m_nCursorOffset - 1);
        CGUIWord* pNextWord = *itNext;//m_lstWords[m_nCursorOffset - 1];
        nOffsetX = pNextWord->GetWordWidth() + m_renderQuad.m_rectScreen.left - m_nCursorPosX;
        m_nCursorPosX += nOffsetX;
        for (GUIWORD_ITER it = m_lstWords.begin(); it != m_lstWords.end(); ++it)
        {
            (*it)->Move(nOffsetX, nOffsetY);
        }
        //for (size_t i = 0; i < m_lstWords.size(); i++)
        //{
        //    m_lstWords[i]->Move(nOffsetX, nOffsetY);
        //}
        
    }
}

bool CGUIEditBox::OnKeyPressSelf(int nKey)
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    //char 
    if (m_lstWords.size() == 0)
    {
        if (m_pKeyPressFun)
        {
            return m_pKeyPressFun(this, nKey);
        }
    }
    if (nKey == VK_LEFT)
    {
        if (m_nCursorOffset > 0)
        {
            GUIWORD_ITER it = m_lstWords.begin();
            std::advance(it, --m_nCursorOffset);
            CGUIWord* pWord = *it; //m_lstWords.begin() + (--m_nCursorOffset);
            m_nCursorPosX -= pWord->GetWordWidth();
        }
        
    }
    else if (nKey == VK_RIGHT)
    {
        if (m_nCursorOffset < (int)m_lstWords.size())
        {
            GUIWORD_ITER it = m_lstWords.begin();
            std::advance(it, m_nCursorOffset++);
            CGUIWord* pWord = *it; //m_lstWords.begin() + m_nCursorOffset++;
            m_nCursorPosX += pWord->GetWordWidth();
        }
    }
    else if (nKey == VK_DELETE)
    {
        if (m_lstWords.size() > 0 && m_nCursorOffset < (int)m_lstWords.size()/* - 1*/)
        {
            GUIWORD_ITER it = m_lstWords.begin();
            std::advance(it, m_nCursorOffset);//m_lstWords.begin() + m_nCursorOffset;
            CGUIWord* pWord = *it;
            int dx = pWord->GetWordWidth();
            SafeDelete(pWord);
            m_lstWords.erase(it);
            m_strText.erase(m_nCursorOffset, 1);
            MoveWordsAfterChange(-dx);
        }
    }
    else if (nKey == VK_BACK)
    {
        if (m_nCursorOffset > 0)
        {
            GUIWORD_ITER it = m_lstWords.begin();
            std::advance(it, m_nCursorOffset - 1);//m_lstWords.begin() + m_nCursorOffset - 1;
            CGUIWord* pWord = *it;
            int dx = pWord->GetWordWidth();
            SafeDelete(pWord);
            m_lstWords.erase(it);
            m_strText.erase(m_nCursorOffset - 1, 1);
            --m_nCursorOffset;
            MoveWordsAfterChange(-dx);
            m_nCursorPosX -= dx;
        }
    }
#endif

    ResetCursorPos();
    if (m_pKeyPressFun)
    {
        return m_pKeyPressFun(this, nKey);
    }
    return true;
}

bool CGUIEditBox::OnChar(char ch)
{
    char szTmp[2] = {ch, 0};
    wchar_t wszTmp[4] = { 0 };
    MByteToWChar(szTmp, wszTmp, 2);
    m_strText.insert(m_nCursorOffset, wszTmp);
    //m_strText += wszTmp;
    if (m_bPassword)
    {
        AddWords(L"*");
    }
    else
        AddWords(wszTmp);
    
    return true;
}

void CGUIEditBox::AddText(const wchar_t* wszText)
{
    m_strText.insert(m_nCursorOffset, wszText);
    if (m_bPassword)
    {
        AddWords(L"*");
    }
    else
        AddWords(wszText);
}

void CGUIEditBox::AddText(const char* szText)
{
    CGUIStaticText::AddText(szText);
}

bool CGUIEditBox::IsPasswordMode() const
{
    return m_bPassword;
}

void CGUIEditBox::CacluateNoWordsCursorPos()
{
    switch (m_align)
    {
    case eAlign_Left:
        m_nCursorPosX = m_renderQuad.m_rectScreen.left;
        m_nCursorPosY = m_renderQuad.m_rectScreen.top + (m_renderQuad.m_rectScreen.GetHeight() - m_pFont->GetWordHeight()) / 2; 
        break;
    case eAlign_LeftTop:
        m_nCursorPosX = m_renderQuad.m_rectScreen.left;
        m_nCursorPosY = m_renderQuad.m_rectScreen.top;
        break;
    case eAlign_Middle:
        m_nCursorPosX = m_renderQuad.m_rectScreen.left + m_renderQuad.m_rectScreen.GetWidth() / 2;
        m_nCursorPosY = m_renderQuad.m_rectScreen.top + (m_renderQuad.m_rectScreen.GetHeight() - m_pFont->GetWordHeight()) / 2; 
        break;
    case eAlign_MiddleTop:
        m_nCursorPosX = m_renderQuad.m_rectScreen.left + m_renderQuad.m_rectScreen.GetWidth() / 2;
        m_nCursorPosY = m_renderQuad.m_rectScreen.top;
        break;
    }
}

void CGUIEditBox::SetFont(const FONTTYPE& ft)
{
    if (m_pFont)
    {
        if (m_pFont->GetFontType() == ft)
        {
            return;
        }
        //FONTMGR->ReleaseFont(m_pFont);
    }

    for (GUIWORD_ITER it = m_lstWords.begin(); it != m_lstWords.end(); ++it)
    {
        (*it)->CleanUp();
        SafeDelete(*it);
    }
    m_lstWords.clear();

    m_nCursorOffset = 0;
    ResetCursorPos();

    if (m_strText.length() > 0)
    {
        m_pFont = FONTMGR->CreateFontW(ft, m_strText.c_str());
        SetWords(m_strText.c_str());
    }
    else
    {
        m_pFont = FONTMGR->CreateFontW(ft, L"a");
    }
}

void CGUIEditBox::SetMaxWordsNum(int nMaxWords)
{
    m_dwMaxWords = nMaxWords;
}

void CGUIEditBox::SetWordsColor(const SColor color)
{
    for (GUIWORD_ITER it = m_lstWords.begin(); it != m_lstWords.end(); ++it)
    {
        (*it)->SetColor(color);
    }
}

void CGUIEditBox::MoveWordsAfterChange(int dx)
{
    GUIWORD_ITER it = m_lstWords.begin();
    std::advance(it, m_nCursorOffset);
    for (; it != m_lstWords.end(); ++it)
    {
        (*it)->Move(dx, 0);
    }
}

//void CGUIEditBox::RegisterToScript()
//{
//    UILUASCRIPTMGR->Declare(this->m_strName.c_str(), this);
//}
