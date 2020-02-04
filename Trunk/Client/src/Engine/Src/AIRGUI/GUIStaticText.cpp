#include "GUIStaticText.h"
#include "GUISystem.h"
#include "Util.h"
#include "Engine.h"
#include "GUIWord.h"
#include "GUIRenderLine.h"
#include "StringUtil.h"
#include "GUIRenderer.h"
//#include "UILuaScriptMgr.h"

CGUIStaticText::CGUIStaticText() : m_pFont(NULL), m_align(eAlign_LeftTop), m_nRowSpace(0), m_bAutoScroll(false), m_bSaveLine(false), m_nLeftOffset(0),
m_nRightOffset(0),
m_nWordFrame(1),
m_clrWordFrame(255, 0, 0, 0)
{
    //m_ctrlType = eCtrl_StaticText;
    m_strType = "CGUIStaticText";
    m_bEnableScissor = false;
    m_bFormated = false;
    memset( &m_rectScissor, 0, sizeof(RECT) );
	m_bMoveable = false;
    
}

CGUIStaticText::~CGUIStaticText()
{
    for (u32 i = 0; i < m_vtRenderLines.size(); i++)
    {
        CGUIRenderLine* pLine = m_vtRenderLines[i];
        pLine->CleanUp();
        SafeDelete(pLine);
    }

    m_vtRenderLines.clear();
}

void CGUIStaticText::Move(int dx, int dy)
{
    CGUIElement::Move(dx, dy);

    //MoveWords(dx, dy);
    for (u32 i = 0; i < m_vtRenderLines.size(); i++)
    {
        CGUIRenderLine* pLine = m_vtRenderLines[i];
        pLine->Move(dx, dy);
    }
    m_rectScissor.left += dx;
    m_rectScissor.top += dy;
    m_rectScissor.right += dx;
    m_rectScissor.bottom += dy;
}

void CGUIStaticText::MoveRealRectTo(int nPosX, int nPosY)
{
    int dx = nPosX - m_realRect.left;
    int dy = nPosY - m_realRect.top;
    MoveRealRect(dx, dy);
}

void CGUIStaticText::ClearChild()
{
    for (u32 i = 0; i < m_vtRenderLines.size(); i++)
    {
        CGUIRenderLine* pLine = m_vtRenderLines[i];
        pLine->CleanUp();
        SafeDelete(pLine);
    }

    m_vtRenderLines.clear();

    m_strText.clear();
}

void CGUIStaticText::RenderSelf()
{
    //m_bDrawFrame = true;
    CGUIElement::RenderSelf();
    if (m_vtRenderLines.size() == 0)
    {
        return;
    }
    GUIRenderer* pRenderer = GUIRENDERER;
    //IRenderer* pRenderer = ENGINE_INST->GetRenderer();

    if ( m_bEnableScissor )
    {
        pRenderer->EnableScissor(m_rectScissor);
        //pRenderer->SetScissorRect(&m_rectScissor);
    }

    for (u32 i = 0; i < m_vtRenderLines.size(); i++)
    {
		if ( m_vtRenderLines[i]->GetIndex() >= 0 )
		{
			m_vtRenderLines[i]->Render();
		}
		else
		{
			//OutputDebugString( GetString("invalid string : %d\n",i) );
		}
    }

    if (m_bEnableScissor)
    {
        pRenderer->DisableScissor();
    }
    
}

const wchar_t*  CGUIStaticText::GetTextW() 
{ 
    return m_strText.c_str(); 
}

void CGUIStaticText::Update()
{
    CGUIElement::Update();

    for (size_t i = 0; i < m_vtRenderLines.size(); ++i)
    {
        m_vtRenderLines[i]->Update();
    }
}

bool CGUIStaticText::CreateSelf(CGUIElement* pParent, CMarkupSTL& xml)
{
    CGUIElement::CreateSelf(pParent, xml);

    AIRString strValue = xml.GetAttrib("Align").c_str();
    if (strValue == "eAlign_Left")
    {
        m_align = eAlign_Left;
    }
    else if (strValue == "eAlign_Middle")
    {
        m_align = eAlign_Middle;
    }
    else if (strValue == "eAlign_MiddleTop")
    {
        m_align = eAlign_MiddleTop;
    }
    else if (strValue == "eAlign_RightTop")
    {
        m_align = eAlign_RightTop;
    }
    else if (strValue == "eAlign_RightBottom")
    {
        m_align = eAlign_RightBottom;
    }
    else if (strValue == "eAlign_LeftBottom")
    {
        m_align = eAlign_LeftBottom;
    }
    else 
    {
        m_align = eAlign_LeftTop;
    }

    strValue = xml.GetAttrib("AutoScroll").c_str();
    if (strValue == "TRUE")
    {
        m_bAutoScroll = true;
    }
    else
    {
        m_bAutoScroll = false;
    }

    

    strValue = xml.GetAttrib("RowSpace").c_str();
    if (strValue.length() > 0)
    {
        m_nRowSpace = atoi(strValue.c_str());
    }

    strValue = xml.GetAttrib("Scissor").c_str();
    if (strValue == "TRUE")
    {
        m_bEnableScissor = true;
    }
    else
    {
        m_bEnableScissor = false;
    }

    strValue = xml.GetAttrib("ScissorRect").c_str();
    if (strValue.length() > 0)
    {
    }
    else
    {
        m_rectScissor.left = m_renderQuad.m_rectScreen.left;
        m_rectScissor.top = m_renderQuad.m_rectScreen.top;
        m_rectScissor.right = m_renderQuad.m_rectScreen.right;
        m_rectScissor.bottom = m_renderQuad.m_rectScreen.bottom;
    }

    strValue = xml.GetAttrib("Formated").c_str();
    if (strValue == "TRUE")
    {
        m_bFormated = true;
    }
    else
    {
        m_bFormated = false;
    }

    strValue = xml.GetAttrib("TextLeftOffset").c_str();
    if (strValue.length() > 0)
    {
        m_nLeftOffset = atoi(strValue.c_str());
    }
    

    strValue = xml.GetAttrib("TextRightOffset").c_str();
    if (strValue.length() > 0)
    {
        m_nRightOffset = atoi(strValue.c_str());
    }

    strValue = xml.GetAttrib("WordFrameWidth").c_str();
    if (strValue.length() > 0)
    {
        m_nWordFrame = atoi(strValue.c_str());
    }

    strValue = xml.GetAttrib("WordFrameColor").c_str();
    if (strValue.length() > 0)
    {
        m_clrWordFrame = ATOL(strValue.c_str());
    }

    //font必须要先创建，否则后面出错
    if (xml.FindChildElem("GUIFont"))
    {
        xml.IntoElem();
        m_pFont = CreateGUIFont(xml);
        xml.OutOfElem();
        CreateGUIWords();
    }

    if (xml.FindChildElem("TextContent"))
    {
        xml.IntoElem();
        AIRString strText = xml.GetData().c_str();

        if (strText.length() > 0)
        {
            size_t nLen = strText.length() + 2;
            wchar_t* wText = new wchar_t[nLen];
            MByteToWChar(strText.c_str(), wText, nLen);

            SetText(wText);

            SafeDeleteArray( wText );   //这里会出错，奇怪！
        }
        xml.OutOfElem();
    }


    
    return true;
}


bool CGUIStaticText::SaveSelfBaseToXML(CMarkupSTL& xml )
{
	CGUIElement::SaveSelfToXML(xml);

	switch ( m_align )
	{
	case eAlign_Left:		xml.AddAttrib( "Align", "eAlign_Left" );		break;
	case eAlign_Middle:		xml.AddAttrib( "Align", "eAlign_Middle" );		break;
	case eAlign_MiddleTop:	xml.AddAttrib( "Align", "eAlign_MiddleTop" );	break;
	case eAlign_LeftTop:	xml.AddAttrib( "Align", "eAlign_LeftTop" );		break;
    case eAlign_RightTop:   xml.AddAttrib( "Align", "eAlign_RightTop");     break;
    case eAlign_RightBottom:xml.AddAttrib( "Align", "eAlign_RightBottom");  break;
    case eAlign_LeftBottom: xml.AddAttrib( "Align", "eAlign_LeftBottom");   break;
	}

	if ( m_bAutoScroll )
	{
		xml.AddAttrib( "AutoScroll", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "AutoScroll", "FALSE" );
	}

    if (m_bEnableScissor)
    {
        xml.AddAttrib("Scissor", "TRUE");
    }

	xml.AddAttrib( "RowSpace", m_nRowSpace );

    if (m_bFormated)
    {
        xml.AddAttrib("Formated", "TRUE");
    }
    else
    {
        xml.AddAttrib("Formated", "FALSE");
    }

    if (m_nLeftOffset > 0)
    {
        xml.AddAttrib("TextLeftOffset", m_nLeftOffset);
    }

    if (m_nRightOffset > 0)
    {
        xml.AddAttrib("TextRightOffset", m_nRightOffset);
    }

    if (m_nWordFrame > 0)
    {
        xml.AddAttrib("WordFrameWidth", m_nWordFrame);
        xml.AddAttrib("WordFrameColor", m_clrWordFrame.GetAsHexString().c_str());
    }
    

    //if ( m_strText.length() > 0 )
    //{
    //    AIRString s = CStringUtil::ws2s(m_strText);
    //    xml.SetData( s.c_str() );
    //}
    
	return true;
}





bool CGUIStaticText::SaveSelfToXML(CMarkupSTL& xml )
{
	SaveSelfBaseToXML(xml);

    if (m_pFont)
    {
        SaveFontToXML(xml, m_pFont);
    }
    if (m_strText.length() > 0)
    {
        xml.IntoElem();
        xml.AddElem("TextContent");
        AIRString s = CStringUtil::ws2s(m_strText);
        xml.SetData( s.c_str() );
        xml.OutOfElem();
    }

	return true;
}

bool CGUIStaticText::SaveTextBlockToXML(CMarkupSTL& xml, IFontBase* pFont)
{
    CGUIElement::SaveTextBlockToXML(xml, pFont);
    if ( m_strText.length() > 0 )
    {
        AIRString s = CStringUtil::ws2s(m_strText);
        xml.SetData( s.c_str() );
    }

    return true;
}

void CGUIStaticText::RegisterToScript()
{
    //UILUASCRIPTMGR->Declare(this->m_strName.c_str(), this);
}

const char* CGUIStaticText::GetText()
{
    char szText[256];
    WCharToMByte(m_strText.c_str(), szText, m_strText.length() + 2);
    m_strTextA = szText;
    return m_strTextA.c_str();
}

void CGUIStaticText::CreateGUIWords()
{
    for (u32 i = 0; i < m_strText.length(); i++)
    {
        //从字体接口获得字体信息
    }
}

void CGUIStaticText::SetText(const char* szText)
{
    int nLen = (int)strlen(szText);
    if ( nLen == 0 )
    {
        return;
    }
    wchar_t* wcsText = new wchar_t[nLen * 2 + 2];
    MByteToWChar(szText, wcsText, nLen * 2 + 2);
    SetText(wcsText);
    SafeDeleteArray(wcsText);
}

void CGUIStaticText::SetText(const wchar_t* wszText)
{
    CleanUp();
    //可以先用formater按自己的格式转换一下
    m_strText = wszText;
    AddWords(wszText);
}

void CGUIStaticText::AddText(const char* szText)
{
    size_t nLen = strlen(szText);
    if (nLen == 0)
    {
        return;
    }
    wchar_t* wcsText = AIR_NEW_ARRAY_T(wchar_t, nLen * 2 + 2);
    MByteToWChar(szText, wcsText, nLen * 2 + 2);
    AddText(wcsText);
    //SafeDeleteArray(wcsText);
    AIR_DELETE_ARRAY_T(wcsText, wchar_t, nLen * 2 + 2);
}

void CGUIStaticText::AddText(const wchar_t* wszText)
{
    if (wcslen(wszText) == 0)
    {
        return;
    }
    m_strText += wszText;
    AddWords(wszText);
}

void CGUIStaticText::SetWords(const wchar_t* wszText)
{
    CGUIRenderLine* pLastRow = NULL;  //最后一行
    if (m_vtRenderLines.size() == 0)
    {
        CGUIRenderLine* pLine = new CGUIRenderLine;
        //pLine->SetAlign(m_align);
        m_vtRenderLines.push_back(pLine);
        Rectf rectLine;
        rectLine.left = m_renderQuad.m_rectScreen.left + m_nLeftOffset;
        rectLine.top = m_renderQuad.m_rectScreen.top;
        rectLine.right = m_renderQuad.m_rectScreen.right - m_nRightOffset;
        rectLine.bottom = rectLine.top + m_pFont->GetWordHeight();
        pLine->SetRect(rectLine);
        pLastRow = pLine;
    }
    else
    {
        pLastRow = m_vtRenderLines[m_vtRenderLines.size() - 1];
    }

    if (m_bFormated)   //如果要格式化
    {
        IGUIFormater* pFormater = GUISYSTEM->GetGUIFormater();
        if (pFormater)
        {
            GUIELEMENT_LIST lstElement;
            pFormater->FormatText(wszText, lstElement, m_pFont);

            for (GUIELEMENT_ITER it = lstElement.begin(); it != lstElement.end(); ++it)
            {
                pLastRow->AddChild(*it);

                int nNextPosX = (*it)->GetRight() + (*it)->GetWidth();
                if (nNextPosX > m_renderQuad.m_rectScreen.right)
                {
                    //m_nCursorPosX = m_renderQuad.m_rectScreen.left;
                    //m_nCursorPosY += m_pFont->GetWordHeight();
                    CGUIRenderLine* pNewLine = new CGUIRenderLine;
                    m_vtRenderLines.push_back(pNewLine);
                    Rectf rectLastLine, rectLine;
                    pLastRow->GetRenderRect(rectLastLine);
                    rectLine.left = m_renderQuad.m_rectScreen.left + m_nLeftOffset;
                    rectLine.top = rectLastLine.bottom + m_nRowSpace;
                    rectLine.right = m_renderQuad.m_rectScreen.right - m_nRightOffset;
                    rectLine.bottom = rectLine.top + m_pFont->GetWordHeight();
                    pNewLine->SetRect(rectLine);

                    pLastRow = pNewLine;
                }
            }
        }
    }
    else
    {
        for (u32 i = 0; i < wcslen(wszText); i++)
        {
            // MFC的editctrl传过来的字符串里，断行符是"\r\n",
            // 所以遇到'\r'时跳过，遇到'\n'才是真正的断行
            if ( wszText[i] == L'\r' )
            {
                continue;
            }
            else if (/*wszText[i] == L'\r' || */wszText[i] == L'\n')   //回车符
            { 
                //m_nCursorPosX = m_renderQuad.m_rectScreen.left;
                //m_nCursorPosY += m_pFont->GetWordHeight();
                CGUIRenderLine* pNewLine = new CGUIRenderLine;
                m_vtRenderLines.push_back(pNewLine);
                Rectf rectLastLine, rectLine;
                pLastRow->GetRenderRect(rectLastLine);
                rectLine.left = m_renderQuad.m_rectScreen.left + m_nLeftOffset;
                rectLine.top = rectLastLine.bottom + m_nRowSpace;
                rectLine.right = m_renderQuad.m_rectScreen.right - m_nRightOffset;
                rectLine.bottom = rectLine.top + m_pFont->GetWordHeight();
                pNewLine->SetRect(rectLine);

                pLastRow = pNewLine;
                continue;
            }
            CGUIWord* pWord = new CGUIWord(wszText[i], m_pFont, m_clrFont, m_nWordFrame, m_clrWordFrame);
            pLastRow->AddChild(pWord);

            //----------------------
            // 临时加的，当字插入行的时候，字的颜色取文本框的默认颜色
            pWord->SetColor( m_renderQuad.m_clrDiffuse );
            //----------------------

            int nNextPosX = pWord->GetRight() + pWord->GetWidth();
            if (nNextPosX > m_renderQuad.m_rectScreen.right)
            {
                //m_nCursorPosX = m_renderQuad.m_rectScreen.left;
                //m_nCursorPosY += m_pFont->GetWordHeight();
                CGUIRenderLine* pNewLine = new CGUIRenderLine;
                m_vtRenderLines.push_back(pNewLine);
                Rectf rectLastLine, rectLine;
                pLastRow->GetRenderRect(rectLastLine);
                rectLine.left = m_renderQuad.m_rectScreen.left + m_nLeftOffset;
                rectLine.top = rectLastLine.bottom + m_nRowSpace;
                rectLine.right = m_renderQuad.m_rectScreen.right - m_nRightOffset;
                rectLine.bottom = rectLine.top + m_pFont->GetWordHeight();
                pNewLine->SetRect(rectLine);

                pLastRow = pNewLine;
            }
        }
    }

    CGUIElement* pLastElement = pLastRow->GetLastElement();//m_vtWords[m_vtWords.size() - 1];
    if ( !pLastElement )
    {
        return;
    }
    Rectf rectWord;
    pLastElement->GetRealRect(rectWord);
    if (rectWord.bottom > m_realRect.bottom)
    {
        m_realRect.bottom = rectWord.bottom;
    }

    ////-----------------------------------------
    //Rectf rectLow;
    //pLastRow->GetRealRect( rectLow );
    //m_realRect.right = rectLow.right;
    ////-----------------------------------------

    //if (m_bAutoScroll && m_realRect.bottom > m_renderQuad.m_rectScreen.bottom)
    //{
    //    //所有字要向上移动
    //    MoveRealRect(0, m_realRect.bottom - m_renderQuad.m_rectScreen.bottom);
    //}

    //m_strText.append( wszText );
    
    CaculateTextAlignPos();
}

void CGUIStaticText::AddWords(const wchar_t* wszText)
{
    if (wszText[0] == 0)
    {
        return;
    }
    if (m_vtRenderLines.size() == 0)
    {
        //CacluateNoWordsCursorPos();
    }
    else
    {
        /*
        if (m_strText[m_nCursorOffset - 1] == L'\r')   //回车符
        { 
            m_nCursorPosX = m_renderQuad.m_rectScreen.left;
            m_nCursorPosY += m_pFont->GetWordHeight();
        }
        else
        {
            //取得当前cursor的前一个字
            CGUIWord* pPreWord = m_vtWords[m_nCursorOffset - 1];
            Rectf rectLastScreen;
            pPreWord->GetRealRect(rectLastScreen);
            m_nCursorPosX = rectLastScreen.right;
            m_nCursorPosY = rectLastScreen.top;
        }
        */
    }
    
    SetWords(wszText);

    m_realRect.right = m_realRect.left;
    for ( size_t i = 0; i < m_vtRenderLines.size(); ++i )
    {
        Rectf rc = m_vtRenderLines[i]->GetRealRect();
        if ( rc.right > m_realRect.right )
        {
            m_realRect.right = rc.right;
        }
    }
}



void CGUIStaticText::MoveRealRect(int dx, int dy)
{
    m_realRect.Move(dx, dy);

    for (u32 i = 0; i < m_vtRenderLines.size(); i++)
    {
        CGUIRenderLine* pLine = m_vtRenderLines[i];
        pLine->Move(dx, dy);
    }
    //MoveWords(dx, dy);
}

int CGUIStaticText::GetLinesNum() const
{
    return m_vtRenderLines.size();
}

void CGUIStaticText::SetFont( IFontBase* pFont )
{
    m_pFont = pFont;
}

void CGUIStaticText::SetAlign(eTextAlign align)
{
    m_align = align;
    CaculateTextAlignPos();
}

void CGUIStaticText::CaculateTextAlignPos()
{
    CGUIRenderLine* pLastLine = NULL;   //上一次的行
    int nStartX = 0;
    int nStartY = 0;
    int dx = 0;
    int dy = 0;
    switch (m_align)
    {
    case eAlign_LeftTop:
        {
            nStartX = m_realRect.left;
            nStartY = m_realRect.top;
            for (GUILINE_ITER it = m_vtRenderLines.begin(); it != m_vtRenderLines.end(); ++it)
            {
                CGUIRenderLine* pRenderLine = *it;
                if (pLastLine != NULL)
                {
                    nStartY = pLastLine->GetBottom() + m_nRowSpace;
                }
                dx = nStartX - pRenderLine->GetLeft() + m_nLeftOffset;
                dy = nStartY - pRenderLine->GetTop();
                pRenderLine->Move(dx, dy);
				pLastLine = pRenderLine;
            }
        }
        break;
    case eAlign_Left:
        {
            int nLineTotalHeight = CaculateLineTotalHeight();
            nStartY = m_renderQuad.m_rectScreen.top + (m_renderQuad.m_rectScreen.GetHeight() - nLineTotalHeight) / 2;
            nStartX = m_renderQuad.m_rectScreen.left;
            for (GUILINE_ITER it = m_vtRenderLines.begin(); it != m_vtRenderLines.end(); ++it)
            {
                CGUIRenderLine* pRenderLine = *it;
                if (pLastLine != NULL)
                {
                    nStartY = pLastLine->GetBottom() + m_nRowSpace;
                }
                //else
                //{
                //    nStartY = m_realRect.top;
                //}
                dx = nStartX - pRenderLine->GetLeft() + m_nLeftOffset;
                dy = nStartY - pRenderLine->GetTop();
                pRenderLine->Move(dx, dy);
				pLastLine = pRenderLine;
            }
        }
        break;
    case eAlign_MiddleTop:
        {
            nStartY = m_realRect.top;
            for (GUILINE_ITER it = m_vtRenderLines.begin(); it != m_vtRenderLines.end(); ++it)
            {
                CGUIRenderLine* pRenderLine = *it;
                nStartX = m_renderQuad.m_rectScreen.left + (m_renderQuad.m_rectScreen.GetWidth() - pRenderLine->GetWidth()) / 2;
                if (pLastLine != NULL)
                {
                    nStartY = pLastLine->GetBottom() + m_nRowSpace;
                }
                dx = nStartX - pRenderLine->GetLeft();
                dy = nStartY - pRenderLine->GetTop();
                pRenderLine->Move(dx, dy);
				pLastLine = pRenderLine;
            }
        }
        break;
    case eAlign_RightTop:
        nStartY = m_realRect.top;
        for (GUILINE_ITER it = m_vtRenderLines.begin(); it != m_vtRenderLines.end(); ++it)
        {
            CGUIRenderLine* pRenderLine = *it;
            nStartX = m_renderQuad.m_rectScreen.left + m_renderQuad.m_rectScreen.GetWidth() - pRenderLine->GetWidth();
            if (pLastLine != NULL)
            {
                nStartY = pLastLine->GetBottom() + m_nRowSpace;
            }
            dx = nStartX - pRenderLine->GetLeft() - m_nRightOffset;
            dy = nStartY - pRenderLine->GetTop();
            pRenderLine->Move(dx, dy);
			pLastLine = pRenderLine;
        }
        break;
    case eAlign_Middle:
        {
            int nLineTotalHeight = CaculateLineTotalHeight();
            nStartY = m_renderQuad.m_rectScreen.top + (m_renderQuad.m_rectScreen.GetHeight() - nLineTotalHeight) / 2;
            for (GUILINE_ITER it = m_vtRenderLines.begin(); it != m_vtRenderLines.end(); ++it)
            {
                CGUIRenderLine* pRenderLine = *it;
                nStartX = m_renderQuad.m_rectScreen.left + (m_renderQuad.m_rectScreen.GetWidth() - pRenderLine->GetWidth()) / 2;
                if (pLastLine != NULL)
                {
                    nStartY = pLastLine->GetBottom() + m_nRowSpace;
                }
                dx = nStartX - pRenderLine->GetLeft();
                dy = nStartY - pRenderLine->GetTop();
                pRenderLine->Move(dx, dy);
                pLastLine = pRenderLine;
            }
        }
        break;
    case eAlign_RightBottom:
        {
            int nLineTotalHeight = CaculateLineTotalHeight();
            nStartY = m_renderQuad.m_rectScreen.bottom - nLineTotalHeight;
            for (GUILINE_ITER it = m_vtRenderLines.begin(); it != m_vtRenderLines.end(); ++it)
            {
                CGUIRenderLine* pRenderLine = *it;
                nStartX = m_renderQuad.m_rectScreen.left + m_renderQuad.m_rectScreen.GetWidth() - pRenderLine->GetWidth();
                if (pLastLine != NULL)
                {
                    nStartY = pLastLine->GetBottom() + m_nRowSpace;
                }
                dx = nStartX - pRenderLine->GetLeft() - m_nRightOffset;
                dy = nStartY - pRenderLine->GetTop();
                pRenderLine->Move(dx, dy);
                pLastLine = pRenderLine;
            }
        }
        
        break;
    case eAlign_LeftBottom:
        {
            int nLineTotalHeight = CaculateLineTotalHeight();
            nStartY = m_renderQuad.m_rectScreen.bottom - nLineTotalHeight;
            for (GUILINE_ITER it = m_vtRenderLines.begin(); it != m_vtRenderLines.end(); ++it)
            {
                CGUIRenderLine* pRenderLine = *it;
                nStartX = m_renderQuad.m_rectScreen.left;
                if (pLastLine != NULL)
                {
                    nStartY = pLastLine->GetBottom() + m_nRowSpace;
                }
                dx = nStartX - pRenderLine->GetLeft() + m_nLeftOffset;
                dy = nStartY - pRenderLine->GetTop();
                pRenderLine->Move(dx, dy);
                pLastLine = pRenderLine;
            }
        }
        break;
    }

    
}

int CGUIStaticText::CaculateLineTotalHeight()
{
    int nTotalLineHeight = 0;
    for (GUILINE_ITER it = m_vtRenderLines.begin(); it != m_vtRenderLines.end(); ++it)
    {
        CGUIRenderLine* pRenderLine = *it;
        nTotalLineHeight += pRenderLine->GetHeight();
        nTotalLineHeight += m_nRowSpace;
    }

    return nTotalLineHeight;
}

void CGUIStaticText::DeleteLine(size_t nLineIndex, bool bCombine)
{
    if (nLineIndex >= m_vtRenderLines.size())
    {
        return;
    }

    int n = 0;
    int dy = 0;   //被删除的line的高度
    int nDeleteLineBottom = 0;
    for (GUILINE_ITER it = m_vtRenderLines.begin(); it != m_vtRenderLines.end();)
    {
        if (n == nLineIndex)
        {
            CGUIRenderLine* pLine = *it;
			//if ( pLine->GetIndex() < 0 )
			//{
			//	OutputDebugString( GetString("delete invalid string: %d\n", n) );
			//	m_vtRenderLines.erase(it);
			//	return;
			//}
            dy = pLine->GetHeight();
            nDeleteLineBottom = pLine->GetBottom();
            pLine->CleanUp();
            SafeDelete(pLine);
            it = m_vtRenderLines.erase(it);
            break;
        }
        else
        {
            it++;
			++n;
        }
    }

    if (!bCombine)
    {
        return;
    }

    if (m_align == eAlign_LeftTop || m_align == eAlign_RightTop || m_align == eAlign_MiddleTop)  //下面的行向上移
    {
        for (size_t i = nLineIndex; i < m_vtRenderLines.size(); ++i)
        {
            m_vtRenderLines[i]->Move(0, -dy);
        }
        CGUIRenderLine* pLastLine = GetRenderLine((int)m_vtRenderLines.size() - 1);
        if (pLastLine)
        {
            m_realRect.bottom = pLastLine->GetBottom();
        }
    }
    else if (m_align == eAlign_LeftBottom || m_align == eAlign_RightBottom)
    {
        for (size_t i = 0; i < nLineIndex; ++i)
        {
            m_vtRenderLines[i]->Move(0, dy);
        }
        CGUIRenderLine* pFirstLine = GetRenderLine(0);
        if (pFirstLine)
        {
            m_realRect.top = pFirstLine->GetTop();
        }
        else
            m_realRect.top = m_renderQuad.m_rectScreen.top;
    }
    else
    {
        for (size_t i = 0; i < nLineIndex; ++i)
        {
            m_vtRenderLines[i]->Move(0, dy);
        }

        CGUIRenderLine* pFirstLine = GetRenderLine(0);
        if (pFirstLine)
        {
            m_realRect.top = pFirstLine->GetTop();
        }
        else
            m_realRect.top = m_renderQuad.m_rectScreen.top;
        CaculateTextAlignPos();
    }


	////////////////////////////////////////////////////////////////////////////////
	//int nLeft, nTop, nRight, nBottom;
	//if ( m_vtRenderLines.size() == 0 )
	//{
	//	//SetWidth( 0 );
	//	//SetHeight( 0 );
	//	return;
	//}
	//else
	//{
	//	nLeft = nRight = m_vtRenderLines[0]->GetLeft();
	//	nTop = nBottom = m_vtRenderLines[0]->GetTop();
	//	for ( int i = 0; i < m_vtRenderLines.size(); ++i )
	//	{
	//		if ( m_vtRenderLines[i]->GetRight() > nRight )
	//		{
	//			nRight = m_vtRenderLines[i]->GetRight();
	//		}
	//		nBottom += m_vtRenderLines[i]->GetHeight();
	//	}
	//	SetRect( Rectf(nLeft, nTop, nRight, nBottom) );
	//	SetRealRect( Rectf(nLeft, nTop, nRight, nBottom) );
	//}
	////////////////////////////////////////////////////////////////////////////////

    /*
    //调整后面的line的位置
    if (nDeleteLineBottom > m_renderQuad.m_rectScreen.top)
    {
        for (; nLineIndex < m_vtRenderLines.size(); nLineIndex++)
        {
            m_vtRenderLines[nLineIndex]->Move(0, dy);
        }
    }
    else
    {
        //调整前面的位置
        for (int i = 0; i < nLineIndex; i++)
        {
            CGUIRenderLine* pLine = m_vtRenderLines[i];
            pLine->Move(0, -dy);
        }
    }
    

    if (m_vtRenderLines.size() == 0 || m_realRect.top > m_renderQuad.m_rectScreen.top)
    {
        m_realRect = m_renderQuad.m_rectScreen;
    }
    */
}

CGUIRenderLine* CGUIStaticText::GetRenderLine(size_t nIndex)
{
    if (nIndex >= 0 && nIndex < m_vtRenderLines.size())
    {
        return m_vtRenderLines[nIndex];
    }

    return NULL;
}

void CGUIStaticText::AddLineText(const char* szText)
{
    int nLen = (int)strlen(szText);
    if (nLen == 0)
    {
        return;
    }
    nLen += 2;
    wchar_t* wcsText = AIR_NEW_ARRAY_T(wchar_t, nLen);
    MByteToWChar(szText, wcsText, nLen);
    AddLineText(wcsText);
    //SafeDeleteArray(wcsText); 
    AIR_DELETE_ARRAY_T(wcsText, wchar_t, nLen);
}

void CGUIStaticText::AddLineText(const wchar_t* wcText)
{
    CGUIRenderLine* pLastRow = NULL;  //最后一行
    if (m_vtRenderLines.size() == 0)
    {
        
    }
    else
    {
        pLastRow = m_vtRenderLines[m_vtRenderLines.size() - 1];
    }
    int nNewLineTop = m_realRect.top;
    if (pLastRow != NULL)
    {
        nNewLineTop = pLastRow->GetBottom() + m_nRowSpace;
    }
    CGUIRenderLine* pLine = new CGUIRenderLine;
    //pLine->SetAlign(m_align);
    m_vtRenderLines.push_back(pLine);
    Rectf rectLine;
    rectLine.left = m_renderQuad.m_rectScreen.left + m_nLeftOffset;
    rectLine.top = nNewLineTop;
    rectLine.right = m_renderQuad.m_rectScreen.right - m_nRightOffset;
    rectLine.bottom = rectLine.top + m_pFont->GetWordHeight();
    pLine->SetRect(rectLine);
    pLastRow = pLine;

    if (m_bFormated)   //如果要格式化
    {
        IGUIFormater* pFormater = GUISYSTEM->GetGUIFormater();
        if (pFormater)
        {
            GUIELEMENT_LIST lstElement;
            pFormater->FormatText(wcText, lstElement, m_pFont);

            for (GUIELEMENT_ITER it = lstElement.begin(); it != lstElement.end(); ++it)
            {
                pLastRow->AddChild(*it);

                int nNextPosX = (*it)->GetRight() + (*it)->GetWidth();
                if (nNextPosX > m_renderQuad.m_rectScreen.right)
                {
                    //m_nCursorPosX = m_renderQuad.m_rectScreen.left;
                    //m_nCursorPosY += m_pFont->GetWordHeight();
                    CGUIRenderLine* pNewLine = new CGUIRenderLine;
                    m_vtRenderLines.push_back(pNewLine);
                    Rectf rectLastLine, rectLine;
                    pLastRow->GetRenderRect(rectLastLine);
                    rectLine.left = m_renderQuad.m_rectScreen.left + m_nLeftOffset;
                    rectLine.top = rectLastLine.bottom + m_nRowSpace;
                    rectLine.right = m_renderQuad.m_rectScreen.right - m_nRightOffset;
                    rectLine.bottom = rectLine.top + m_pFont->GetWordHeight();
                    pNewLine->SetRect(rectLine);

                    pLastRow = pNewLine;
                }
            }
        }
    }
    else
    {
        for (u32 i = 0; i < wcslen(wcText); i++)
        {
            if (wcText[i] == L'\r' || wcText[i] == L'\n')   //回车符
            { 
                //m_nCursorPosX = m_renderQuad.m_rectScreen.left;
                //m_nCursorPosY += m_pFont->GetWordHeight();
                CGUIRenderLine* pNewLine = new CGUIRenderLine;
                m_vtRenderLines.push_back(pLine);
                Rectf rectLastLine, rectLine;
                pLastRow->GetRenderRect(rectLastLine);
                rectLine.left = m_renderQuad.m_rectScreen.left + m_nLeftOffset;
                rectLine.top = rectLastLine.bottom + m_nRowSpace;
                rectLine.left = m_renderQuad.m_rectScreen.right - m_nRightOffset;
                rectLine.bottom = rectLine.top + m_pFont->GetWordHeight();
                pNewLine->SetRect(rectLine);

                //pLastRow->CaculateWordPosition();

                pLastRow = pNewLine;
                continue;
            }
            CGUIWord* pWord = new CGUIWord(wcText[i], m_pFont, m_clrFont, m_nWordFrame, m_clrWordFrame);
            pLastRow->AddChild(pWord);
            //pWord->SetScreenPos(m_nCursorPosX, m_nCursorPosY);

            int nNextPosX = pWord->GetRight() + pWord->GetWidth();
            if (nNextPosX > m_renderQuad.m_rectScreen.right && i < wcslen(wcText) - 1)
            {
                CGUIRenderLine* pNewLine = new CGUIRenderLine;
                m_vtRenderLines.push_back(pLine);
                Rectf rectLastLine, rectLine;
                pLastRow->GetRenderRect(rectLastLine);
                rectLine.left = m_renderQuad.m_rectScreen.left + m_nLeftOffset;
                rectLine.top = rectLastLine.bottom + m_nRowSpace;
                rectLine.left = m_renderQuad.m_rectScreen.right - m_nRightOffset;
                rectLine.bottom = rectLine.top + m_pFont->GetWordHeight();
                pNewLine->SetRect(rectLine);

                pLastRow = pNewLine;
            }
        }
    }
    

    //pLastRow->CaculateWordPosition();
    

    CGUIElement* pLastElement = pLastRow->GetLastElement();//m_vtWords[m_vtWords.size() - 1];
	if ( pLastElement )
	{
		Rectf rectWord;
		pLastElement->GetRealRect(rectWord);
		if (rectWord.bottom > m_realRect.bottom)
		{
			m_realRect.bottom = rectWord.bottom;
		}
	}

    if (m_bAutoScroll && m_realRect.bottom > m_renderQuad.m_rectScreen.bottom)
    {
        //所有字要向上移动
        MoveRealRect(0, m_renderQuad.m_rectScreen.bottom - m_realRect.bottom);
    }

    CaculateTextAlignPos();
}


void CGUIStaticText::SetScissorRect( const RECT& rect )
{
    m_rectScissor = rect;
}

void CGUIStaticText::GetScissorRect(RECT& rect)
{
    rect = m_rectScissor;
}

void CGUIStaticText::SetEnableScissor(bool b )
{
    m_bEnableScissor = b;
}

void CGUIStaticText::SetRowSpace(unsigned int nRowSpace)
{
    m_nRowSpace = nRowSpace;

}

void CGUIStaticText::SetAutoScroll(bool bAutoScroll)
{
    m_bAutoScroll = bAutoScroll;
}

void CGUIStaticText::SetFont(const FONTTYPE& ft)
{
    if (m_pFont)
    {
        if (m_pFont->GetFontType() == ft)
        {
            return;
        }
        //FONTMGR->ReleaseFont(m_pFont);
    }
    //字体变了，要删除原来的字体

    for (size_t i = 0; i < m_vtRenderLines.size(); ++i)
    {
        m_vtRenderLines[i]->CleanUp();
        SafeDelete(m_vtRenderLines[i]);
    }
    m_vtRenderLines.clear();
    
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

void CGUIStaticText::SetFontColor(const SColor color)
{
    m_clrFont = color;
    if (!m_bFormated)
    {
        SetWordsColor(m_clrFont);
    }
}

void CGUIStaticText::SetWordsColor(const SColor color)
{
    for (size_t i = 0; i < m_vtRenderLines.size(); ++i)
    {
        CGUIRenderLine* pLine = m_vtRenderLines[i];
        pLine->SetWordsColor(color);
    }
}


void CGUIStaticText::SetIsDrawFrame( bool b )
{
	CGUIElement::SetIsDrawFrame(b);
	for ( size_t i = 0; i < m_vtRenderLines.size(); ++i )
	{
		m_vtRenderLines[i]->SetIsDrawFrame( b );
	}
}

void CGUIStaticText::SetFormated(bool bFormated)
{
    m_bFormated = bFormated;
}

bool CGUIStaticText::OnLButtonDown(int x, int y)
{
    size_t lineCount = m_vtRenderLines.size();
    CGUIRenderLine* pLine = NULL;
    for (size_t i = 0; i < lineCount; ++i)
    {
        pLine = m_vtRenderLines[i];
        if (pLine->IsInRect(x, y))
        {
            return pLine->OnLButtonDown(x, y);
        }
    }

    return OnLButtonDownSelf(x, y);
}


void CGUIStaticText::AddAutoSizeLine(const char* szText, int nMaxWidth )
{
	SetWidth( nMaxWidth );
	AddLineText( szText );
	//int nLeft, nTop, nRight, nBottom;
	//if ( m_vtRenderLines.size() == 0 )
	//{
	//	//SetWidth( 0 );
	//	//SetHeight( 0 );
	//	return;
	//}
	//else
	//{
	//	nLeft = nRight = m_vtRenderLines[0]->GetLeft();
	//	nTop = nBottom = m_vtRenderLines[0]->GetTop();
	//	for ( int i = 0; i < m_vtRenderLines.size(); ++i )
	//	{
	//		if ( m_vtRenderLines[i]->GetRight() > nRight )
	//		{
	//			nRight = m_vtRenderLines[i]->GetRight();
	//		}
	//		nBottom += m_vtRenderLines[i]->GetHeight();
	//	}
	//	SetRect( Rectf(nLeft, nTop, nRight, nBottom) );
	//	SetRealRect( Rectf(nLeft, nTop, nRight, nBottom) );
	//}
}


void CGUIStaticText::CalWholeRealSize( Rectf& rc )
{
	int nLeft, nTop, nRight, nBottom;
	if ( m_vtRenderLines.size() == 0 )
	{
		rc = m_realRect;
	}
	else
	{
		nLeft = nRight = m_vtRenderLines[0]->GetLeft();
		nTop = nBottom = m_vtRenderLines[0]->GetTop();
		for ( size_t i = 0; i < m_vtRenderLines.size(); ++i )
		{
			if ( m_vtRenderLines[i]->GetRight() > nRight )
			{
				nRight = m_vtRenderLines[i]->GetRight();
			}
			nBottom += m_vtRenderLines[i]->GetHeight();
		}
		rc.SetRect( nLeft, nTop, nRight, nBottom );
		m_realRect.SetRect( nLeft, nTop, nRight, nBottom );
	}
}

void CGUIStaticText::AutoSizeByWords(const char* szText, int nMaxWidth /* = 0 */)
{
    CleanUp();

    int nLen = (int)strlen(szText);
    if (nLen == 0)
    {
        return;
    }

    wchar_t* wcText = new wchar_t[nLen * 2 + 2];
    MByteToWChar(szText, wcText, nLen * 2 + 2);
    AutoSizeByWords(wcText, nMaxWidth);
    SafeDeleteArray(wcText);
}

void CGUIStaticText::AutoSizeByWords(const wchar_t* wcText, int nMaxWidth /* = 0 */)
{
    if (wcslen(wcText) == 0)
    {
        return;
    }
    CGUIRenderLine* pLastRow = NULL;  //最后一行
    if (m_vtRenderLines.size() == 0)
    {
        CGUIRenderLine* pLine = new CGUIRenderLine;
        //pLine->SetAlign(m_align);
        m_vtRenderLines.push_back(pLine);
        Rectf rectLine;
        rectLine.left = m_renderQuad.m_rectScreen.left + m_nLeftOffset;
        rectLine.top = m_renderQuad.m_rectScreen.top;
        rectLine.right = m_renderQuad.m_rectScreen.right - m_nRightOffset;
        rectLine.bottom = rectLine.top + m_pFont->GetWordHeight();
        pLine->SetRect(rectLine);
        pLastRow = pLine;
    }
	else
	{
		pLastRow = m_vtRenderLines[ m_vtRenderLines.size()-1 ];
	}

    int nLineWidth = 0;
    //int nMaxLineWidth = 0;
    int nTotalHeight = 0;
    bool bReachMaxWidth = false; //是否已经达到最大宽
    if (m_bFormated)   //如果要格式化
    {
        IGUIFormater* pFormater = GUISYSTEM->GetGUIFormater();
        if (pFormater)
        {
            GUIELEMENT_LIST lstElement;
            pFormater->FormatText(wcText, lstElement, m_pFont);

            for (GUIELEMENT_ITER it = lstElement.begin(); it != lstElement.end(); ++it)
            {
                nLineWidth += (*it)->GetRealWidth();
                if (nLineWidth <= nMaxWidth)
                {
                    pLastRow->AddChild(*it);
                    //nTotalHeight += pLastRow->GetHeight();
                    
                }
                else
                {
                    bReachMaxWidth = true;
                    CGUIRenderLine* pNewLine = new CGUIRenderLine;
                    m_vtRenderLines.push_back(pNewLine);

                    Rectf rectLastLine, rectLine;
                    pLastRow->GetRenderRect(rectLastLine);
                    rectLine.left = m_renderQuad.m_rectScreen.left + m_nLeftOffset;
                    rectLine.top = rectLastLine.bottom + m_nRowSpace;
                    rectLine.right = m_renderQuad.m_rectScreen.right - m_nRightOffset;
                    rectLine.bottom = rectLine.top + m_pFont->GetWordHeight();
                    nTotalHeight += rectLine.GetHeight();
                    pNewLine->SetRect(rectLine);

                    pLastRow = pNewLine;
                    pLastRow->AddChild(*it);
                    nLineWidth -= nMaxWidth;
                }

                if (!bReachMaxWidth)
                {
                    nTotalHeight = pLastRow->GetHeight();
                }
            }
        }
    }
    else
    {
        nTotalHeight = m_pFont->GetWordHeight();
        for (size_t i = 0; i < wcslen(wcText); i++)
        {
            CGUIWord* pWord = new CGUIWord(wcText[i], m_pFont, m_clrFont, m_nWordFrame, m_clrWordFrame);
            nLineWidth += pWord->GetWordWidth();
            //nMaxLineWidth = nLineWidth;
            if (nLineWidth <= nMaxWidth)
            {
                pLastRow->AddChild(pWord);
            }
            else
            {
                bReachMaxWidth = true;
                CGUIRenderLine* pNewLine = new CGUIRenderLine;
                m_vtRenderLines.push_back(pNewLine);

                Rectf rectLastLine, rectLine;
                pLastRow->GetRenderRect(rectLastLine);
                rectLine.left = m_renderQuad.m_rectScreen.left + m_nLeftOffset;
                rectLine.top = rectLastLine.bottom + m_nRowSpace;
                rectLine.right = m_renderQuad.m_rectScreen.right - m_nRightOffset;
                rectLine.bottom = rectLine.top + m_pFont->GetWordHeight();
                nTotalHeight += rectLine.GetHeight();
                pNewLine->SetRect(rectLine);

                pLastRow = pNewLine;
                pLastRow->AddChild(pWord);
            }

            if (!bReachMaxWidth)
            {
                nTotalHeight = pLastRow->GetHeight();
            }
        }
    }
    

    if (bReachMaxWidth)
    {
        SetWidth(nMaxWidth);
        SetHeight(nTotalHeight);
    }
    else
    {
        SetWidth(nLineWidth);
        SetHeight(nTotalHeight);
    }
}

/*
void CGUIStaticText::AddChild(CGUIElement* pChild)
{
    CGUIRenderLine* pLastRow = NULL;

    if (m_vtRenderLines.size() == 0)
    {
        pLastRow = new CGUIRenderLine;
        m_vtRenderLines.push_back(pLastRow);
        pLastRow->SetRect(Rectf(m_renderQuad.m_rectScreen.left, m_renderQuad.m_rectScreen.top, m_renderQuad.m_rectScreen.left, m_renderQuad.m_rectScreen.top));
    }
    else
    {
        pLastRow = m_vtRenderLines[m_vtRenderLines.size() - 1];
    }

    if (pChild->GetWidth() + pLastRow->GetRight() > GetRight())
    {
        CGUIRenderLine* pNewLine = new CGUIRenderLine;
        m_vtRenderLines.push_back(pNewLine);
        Rectf rectLastLine, rectLine;
        pLastRow->GetRenderRect(rectLastLine);

        rectLine.left = m_renderQuad.m_rectScreen.left;
        rectLine.top = rectLastLine.bottom + m_nRowSpace;
        rectLine.right = m_renderQuad.m_rectScreen.left;
        rectLine.bottom = rectLine.top;

        pNewLine->SetRect(rectLine);
        pLastRow = pNewLine;
        pLastRow->AddChild(pChild);
    }
    else
    {
        pLastRow->AddChild(pChild);
    }
    
}
*/

void CGUIStaticText::SetSaveLine(bool bSaveLine)
{
    m_bSaveLine = bSaveLine;
}

void CGUIStaticText::SetLineSelected(size_t nLineIndex, bool bSelected)
{
    if (nLineIndex >= m_vtRenderLines.size())
    {
        return;
    }

    m_vtRenderLines[nLineIndex]->SetSelected(bSelected);
}

void CGUIStaticText::ClearText()
{
    ClearChild();
}

void CGUIStaticText::SetTextLeftOffset(int nLeftOffset)
{
    m_nLeftOffset = nLeftOffset;
    CaculateTextAlignPos();
}

void CGUIStaticText::SetTextRightOffset(int nRightOffset)
{
    m_nRightOffset = nRightOffset;
    CaculateTextAlignPos();
}

void CGUIStaticText::SetWordFrameWidth(int nWidth)
{
    if (m_nWordFrame != nWidth)
    {
        m_nWordFrame = nWidth;
        WAIRString strText = m_strText;
        ClearText();
        SetText(strText.c_str());
    }
}

void CGUIStaticText::SetWordFrameColor(const SColor& color)
{
    if (m_clrWordFrame != color)
    {
        m_clrWordFrame = color;
        WAIRString strText = m_strText;
        ClearText();
        SetText(strText.c_str());
    }
}
