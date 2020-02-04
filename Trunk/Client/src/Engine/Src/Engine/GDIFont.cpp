#include "stdafx.h"
#include "GDIFont.h"
#include "Util.h"
#include "MyMath.h"
#include "Engine.h"
#include "TextureMgr.h"
#include "MemoryAllocatorConfig.h"

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32

CGDICanvas::CGDICanvas()
{
    m_hDC = CreateCompatibleDC(NULL);
    m_hBrush = CreateSolidBrush(RGB(255, 255, 255));
    SelectObject(m_hDC, m_hBrush);
}

CGDICanvas::~CGDICanvas()
{
    //DeleteObject(m_hFont);
    DeleteObject(m_hBrush);
    DeleteDC(m_hDC);
}

bool CGDICanvas::SetFont(HFONT hFont)
{
    HGDIOBJ OldFont			= SelectObject(m_hDC,hFont);
    DeleteObject(OldFont);
    return true;
}

void CGDICanvas::TextOut(const AIRString& Text, s32 X, s32 Y )
{
    ::TextOut( m_hDC, X, Y, Text.c_str(), (int)strlen(Text.c_str()) );
}

CGDIFont::CGDIFont() : m_pTexture(NULL), m_nOffsetX(0), m_nOffsetY(0), m_pFontTexBuffer(NULL), m_bTextureDirty(false)
{

}

CGDIFont::~CGDIFont()
{
	if (m_pFontTexBuffer)
	{
        assert(m_nTexturePixelBytes != 0);
		AIR_DELETE_ARRAY_T(m_pFontTexBuffer, u8, 512 * 512 * m_nTexturePixelBytes);
	}
    Release();
}

bool CGDIFont::Create(const FONTTYPE& ft, const char* szWords)
{
    int nLen = strlen(szWords);
    wchar_t* wszText = AIR_NEW_ARRAY_T(wchar_t, nLen * 2 + 2);
    MByteToWChar(szWords, wszText, nLen * 2 + 2);
    bool bRes = CreateW(ft, wszText);
    //delete [] wszText;
	AIR_DELETE_ARRAY_T( wszText, wchar_t, nLen * 2 + 2 );

    return bRes;
}

bool CGDIFont::CreateW(const FONTTYPE& ft, const wchar_t* szWords)
{
    // 设置字体格式
    m_type = ft;

    CGDICanvas canvas;

    int nHeight = -MulDiv( m_type.m_nFontSize, ::GetDeviceCaps(canvas.GetDC(), LOGPIXELSY), 72 );

    if (CreateGDIFont(m_type, nHeight) == NULL)
        return false;

    canvas.SetFont(m_hFont);

	TEXTMETRICW tm;
	GetTextMetricsW( canvas.GetDC(), &tm );
	m_nFontHeight = tm.tmHeight;

    //创建纹理 已经被注释，要实现
    CreateFontTexture();
    //if (m_pTexture == NULL)
    //{
    //    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    //    m_pTexture = pRenderer->CreateTexture(m_type.m_strFaceName.c_str(), 512, 512);
    //}

	size_t szlen = UnicodeStrLen(szWords);
    for (u32 i = 0; i < szlen; i++)
    {
        wchar_t wCH = szWords[i];
        //CWordRenderable* pWord = FindWord(wCH);
        LPWORDINFO pWord = FindWord(wCH);

        if (pWord)
        {
            continue;
        }


        u8* pPixel = NULL;
        u8* pTemp = NULL;

        // 将文字的UNICODE装化为多字节
        wchar_t str[10] = { wCH, 0 };
        char buf[10];
        WCharToMByte( str, buf, sizeof(str) );

        // 如果是非ASCII字符或使用平滑字符的话,
        // 则用平滑的方式输出字符
        //else
        //{
            TEXTMETRICW tm;
            GetTextMetricsW( canvas.GetDC(), &tm );
            m_nFontHeight = tm.tmHeight;
            //SIZE ws = dib->m_pCanvas->GetTextSize( AIRString(buf) );
            SIZE ws; // = dib->m_pCanvas->GetTextSize( AIRString(buf) );
            //SIZE		sText;
            memset( &ws, 0x0, sizeof(SIZE) );
            GetTextExtentPoint32( canvas.GetDC(), buf, (int)strlen(buf), &ws );
            MAT2         mmat2 = {0,1,0,0,0,0,0,1};
            GLYPHMETRICS glpm;
            u32        nLen;
            //u8         temp;
            nLen = ::GetGlyphOutlineW( canvas.GetDC(), wCH, m_type.m_nFontHint, &glpm, 0, NULL, &mmat2 );
            if (nLen == 0 && wCH != ' ')
            {
                continue;
            }

            int nBorderOffset = m_type.m_nFontHint == FONT_BMP ? m_type.m_nBorderSize : 0;

            pWord = new WORDINFO;
            pWord->wCode = wCH;
            pWord->nWidth = glpm.gmCellIncX + nBorderOffset * 2;
            pWord->nHeight = tm.tmHeight + nBorderOffset * 2;
            pWord->nFontWidth = glpm.gmBlackBoxX + nBorderOffset * 2;
            pWord->nFontHeight = glpm.gmBlackBoxY + nBorderOffset * 2;
            pWord->nOffsetX = glpm.gmptGlyphOrigin.x - nBorderOffset;
            pWord->nOffsetY = tm.tmAscent - glpm.gmptGlyphOrigin.y - nBorderOffset;

            //改为空格也照写入纹理
            if (wCH == ' ')
            {
                m_nSpaceWidth = glpm.gmCellIncX;
                continue;
            }

            m_nCharLineW = ( glpm.gmBlackBoxX / 4 + ( glpm.gmBlackBoxX % 4 == 0 ? 0 : 1 ) ) * 4;
            // 字符的实际大小
            u32 realH = /*glpm.gmBlackBoxY;*/ nLen / m_nCharLineW;
            u32 realW = /*glpm.gmBlackBoxX;*/  m_nCharLineW;

            if (m_type.m_nFontHint == FONT_BMP)
            {
                realH = glpm.gmBlackBoxY;
                realW = glpm.gmBlackBoxX;

            }
            // 字符的兼容大小
            u32 nGridHeight = AIR::Max<LONG>(ws.cy, (LONG)realH);
            u32 nGridWidth  = AIR::Max<LONG>(ws.cx, (LONG)realW);
            //int nRBHeight = AIR::Get2Pow( nGridHeight );
            //int nRBWidth  = AIR::Get2Pow( nGridWidth );
            u32 cnt = nGridHeight * nGridWidth * 4;

            if( (signed)nLen > 0 )
            {
                // 从内存块管理器申请缓冲区以保存文字的点阵数据
                //pMBBuf = MEMORY_MGR->GetFreeMemBlock( nLen );
                //ASSERT( pMBBuf );

                // 锁定该内存块
                //pMBBuf->Lock();

                u32 nLentemp;
                // 带灰度的模式
                // 开辟缓冲区保存文字像素数据
                
                
                pTemp = AIR_NEW_ARRAY_T(u8, nLen);
                memset(pTemp, 0, nLen * sizeof(u8));
                pPixel = AIR_NEW_ARRAY_T( u8, nLen * 4/*cnt*/);
                memset(pPixel, 0, nLen * 4 * sizeof(u8));
                
                nLentemp = ::GetGlyphOutlineW( canvas.GetDC(), wCH, m_type.m_nFontHint, &glpm, nLen, pTemp/*m_pBuf*/, &mmat2 );
                
                m_nGlyWidth = nLentemp / glpm.gmBlackBoxY;
            }

            if (m_nOffsetX + glpm.gmBlackBoxX + nBorderOffset * 2 > m_pTexture->GetWidth())
            {
                m_nOffsetX = 0;
                m_nOffsetY += tm.tmHeight + nBorderOffset * 2;
            }

            if (m_type.m_nFontHint != FONT_BMP)
            {
                
                //首先把像素写入一段内存

                //int row, pixel;
                //int rBegin, rEnd, cBegin, cEnd, rOff, cOff;
                //int i = 0;

                //rBegin = (nGridHeight - realH) / 2;
                //rEnd   = rBegin + realH;
                //cBegin = (nGridWidth - realW) / 2;
                //cEnd   = cBegin + realW;
                //计算灰度，暂定64位
                u32 m = ((m_type.m_nFontHint == FONT_BMP_GRAY2 ) ? 4 : (( m_type.m_nFontHint == FONT_BMP_GRAY4 ) ? 16 : 64));

                for (u32 nRow = 0; nRow < glpm.gmBlackBoxY; nRow++)
                {
                    for (u32 nCol = 0; nCol < glpm.gmBlackBoxX; nCol++)
                    {
                        u32 nIndex = (nRow * glpm.gmBlackBoxX + nCol) * 4;
                        //int col = pTemp[i++] * 255 / m;

                        //int index_color = 4 * (int)glpm.gmBlackBoxX *y + 4*x;

                        u32 mono_pitch_bytes = (((8 * (u32)glpm.gmBlackBoxX + 31) & (~31)) / 8);
                        u32 index_monob = mono_pitch_bytes * nRow + nCol;

                        u8 col = (u8)(pTemp[index_monob] / (float)m * 255); //Alpha
                        //col = col < 255 ? 0 : 255;
                        //col = col < 30 ? 0 : ( col > 230 ? 255 : col );
                        pPixel[ nIndex ]   = 255;
                        pPixel[ nIndex + 1 ] = 255;
                        pPixel[ nIndex + 2 ] = 255;
                        pPixel[ nIndex + 3 ] = col;
                    }
                }

                // 将文字的像素保存到纹理中
                if (m_nOffsetY < m_pTexture->GetHeight() && pPixel)
                    SaveBitmapPixelToTexture(pWord, pPixel, m_nOffsetX + glpm.gmptGlyphOrigin.x, m_nOffsetY + tm.tmAscent - glpm.gmptGlyphOrigin.y, glpm.gmBlackBoxX, glpm.gmBlackBoxY, 32);
                else
                {
                    //SafeDeleteArray(pTemp);
                    //SafeDeleteArray(pPixel);
					AIR_DELETE_ARRAY_T(pTemp, u8, nLen);
					AIR_DELETE_ARRAY_T(pPixel, u8, nLen * 4);
					SafeDelete(pWord);
                    return false;
                }
            }
            else    //2位的BMP图
            {
                if (m_type.m_nBorderSize > 0)
                {
                    SColor clrBorder(255, 0, 0, 0);
                    //左边
                    SaveBitmapPixelToTexture(pWord, pTemp, m_nOffsetX, m_nOffsetY + m_type.m_nBorderSize, glpm.gmBlackBoxX, glpm.gmBlackBoxY, 1, clrBorder);
                    //左上边
                    SaveBitmapPixelToTexture(pWord, pTemp, m_nOffsetX, m_nOffsetY, glpm.gmBlackBoxX, glpm.gmBlackBoxY, 1, clrBorder);
                    //上边
                    SaveBitmapPixelToTexture(pWord, pTemp, m_nOffsetX + m_type.m_nBorderSize, m_nOffsetY, glpm.gmBlackBoxX, glpm.gmBlackBoxY, 1, clrBorder);
                    //右上边
                    SaveBitmapPixelToTexture(pWord, pTemp, m_nOffsetX + m_type.m_nBorderSize * 2, m_nOffsetY, glpm.gmBlackBoxX, glpm.gmBlackBoxY, 1, clrBorder);
                    //右边
                    SaveBitmapPixelToTexture(pWord, pTemp, m_nOffsetX + m_type.m_nBorderSize * 2, m_nOffsetY + m_type.m_nBorderSize, glpm.gmBlackBoxX, glpm.gmBlackBoxY, 1, clrBorder);
                    //右下边
                    SaveBitmapPixelToTexture(pWord, pTemp, m_nOffsetX + m_type.m_nBorderSize * 2, m_nOffsetY + m_type.m_nBorderSize * 2, glpm.gmBlackBoxX, glpm.gmBlackBoxY, 1, clrBorder);
                    //下边
                    SaveBitmapPixelToTexture(pWord, pTemp, m_nOffsetX + m_type.m_nBorderSize, m_nOffsetY + m_type.m_nBorderSize * 2, glpm.gmBlackBoxX, glpm.gmBlackBoxY, 1, clrBorder);
                    //左下边
                    SaveBitmapPixelToTexture(pWord, pTemp, m_nOffsetX, m_nOffsetY + m_type.m_nBorderSize * 2, glpm.gmBlackBoxX, glpm.gmBlackBoxY, 1, clrBorder);
                }

                // 将文字的像素保存到纹理中
                if (m_nOffsetY < m_pTexture->GetHeight() && pTemp)
                {
                    SaveBitmapPixelToTexture(pWord, pTemp, m_nOffsetX + m_type.m_nBorderSize, m_nOffsetY + m_type.m_nBorderSize, glpm.gmBlackBoxX, glpm.gmBlackBoxY);
                }
                else
                {
                    //SafeDeleteArray(pTemp);
                    //SafeDeleteArray(pPixel);
					AIR_DELETE_ARRAY_T(pTemp, u8, nLen);
					AIR_DELETE_ARRAY_T(pPixel, u8, nLen * 4);
					SafeDelete(pWord);
                    return false;
                }

            }
            
            //pWord->SetSize(glpm.gmCellIncX, tm.tmHeight);
            float fLeft = (float)m_nOffsetX / m_pTexture->GetWidth();
            float fTop = (float)m_nOffsetY / m_pTexture->GetWidth();
            float fRight = fLeft + (float)(glpm.gmBlackBoxX + nBorderOffset * 2) / m_pTexture->GetWidth();
            float fBottom = fTop + (float)(glpm.gmBlackBoxY + nBorderOffset * 2) / m_pTexture->GetWidth();
            pWord->rectUV = Rectf(fLeft, fTop, fRight, fBottom);

            m_nOffsetX += glpm.gmBlackBoxX + nBorderOffset * 2;
            m_nOffsetX = m_nOffsetX < m_pTexture->GetWidth() ? m_nOffsetX : 0;

            if (m_nOffsetX == 0)
            {
                m_nOffsetY += tm.tmHeight + nBorderOffset * 2;
                m_nOffsetY += m_nOffsetX > m_pTexture->GetWidth() ? (tm.tmHeight  + nBorderOffset * 2) : 0;
            }
            
            
        //}

        // 释放申请的内存块
        //SafeDeleteArray(pTemp);
        //SafeDeleteArray(pPixel);
		AIR_DELETE_ARRAY_T(pTemp, u8, nLen);
		AIR_DELETE_ARRAY_T(pPixel, u8, nLen * 4);

        //AddRenderWord(pWord);
        //AddWordInfoToTexture( fg, (u8*)pMB->m_pMemPtr );
        m_mapWordInfo.insert(make_pair(wCH, pWord));
    }

    return true;
}


void CGDIFont::SaveBitmapPixelToTexture(LPWORDINFO pWord, u8* pPixel, int nPosX, int nPosY, int nWidth, int nHeight, int nBmpBits /* = 1 */, const SColor& color)
{
    //IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    //if (m_pTexture == NULL)
    //{
    //    m_pTexture = pRenderer->CreateTexture(m_type.m_strFaceName.c_str(), 512, 512);
    //}
    CreateFontTexture();

    if (nBmpBits == 1)
    {
        //m_pTexture->CopyMonoFontToTexture(pPixel, nPosX, nPosY, nWidth, nHeight, m_nGlyWidth, color);
		CopyMonoFontToTextureBuffer(pPixel, nPosX, nPosY, nWidth, nHeight, m_nGlyWidth, color);
    }
    else
	{
        //m_pTexture->CopyPixelToTexture(pPixel, nPosX, nPosY, nWidth, nHeight, nBmpBits);
		CopyPixelToTextureBuffer(pPixel, nPosX, nPosY, nWidth, nHeight, nBmpBits);
	}
	m_bTextureDirty = true;
}

void CGDIFont::Release()
{
    if (m_pTexture)
    {
        //m_pTexture->Clear();
        //SafeDelete(m_pTexture);
        TEXTURE_MANAGER->ClearTexture(m_pTexture);
        m_pTexture = NULL;
    }
    

    for (WORDINFO_ITER it = m_mapWordInfo.begin(); it != m_mapWordInfo.end(); it++)
    {
        LPWORDINFO pWord = it->second;
        AIR_DELETE_T(pWord, WORDINFO);
    }

    m_mapWordInfo.clear();
    //IFontBase::Release();
}

void CGDIFont::Render(const WCHAR* szText, const AIR::Rectf& area, const CColorValue& color)
{
    int  nOffsetX = area.left;
    int  nOffsetY = area.top;

    
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    IGUIRenderer* pGUIRenderer = ENGINE_INST->GetGUIRenderer();
    //pRenderer->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
    //pRenderer->SetTextureFilterPoint(0, ETYPE_MAGNIFICATION);
    Rectf rectScreen;
	AIRVector<LPWORDINFO> vecWords;

	size_t szlen = UnicodeStrLen(szText);
    for (u32 i = 0; i < szlen; i++)
    {
		/*
        if (szText[i] == ' ')  //如果是空格
        {
            nOffsetX += m_nSpaceWidth;

            nOffsetX = nOffsetX > area.right ? area.left : nOffsetX;

            if (nOffsetX == 0)
            {
                nOffsetY += m_nFontHeight;
            }
            continue;
        }
		*/
        //CWordRenderable* pRenderWord = FindWord(szText[i]);
        LPWORDINFO pRenderWord = FindWord(szText[i]);
        if (pRenderWord)
        {
			vecWords.push_back(pRenderWord);

			//ENGINE_INST->GetGUIRenderer()->Render(rectScreen, pRenderWord->rectUV, m_pTexture, color);

        }
        else
        {
            //写入纹理，再渲染
            pRenderWord = AddWordToTexture(szText[i]);
			vecWords.push_back(pRenderWord);
			/*
            rectScreen.left = nOffsetX + pRenderWord->nOffsetX;
            rectScreen.right = rectScreen.left + pRenderWord->nFontWidth;
            rectScreen.top = nOffsetY + pRenderWord->nOffsetY;
            rectScreen.bottom = rectScreen.top + pRenderWord->nHeight;
            if (pGUIRenderer)
            {
                pGUIRenderer->Render(rectScreen, pRenderWord->rectUV, pGUIRenderer->GetMinZ() + 0.01f, m_pTexture, color);
            }
            else
                pRenderer->Draw2DImage(rectScreen, pRenderWord->rectUV, m_pTexture, color);
            */
        }
        
    }

	PreRender();
	for (size_t i = 0; i < vecWords.size(); ++i)
	{
		LPWORDINFO pRenderWord = vecWords[i];
		rectScreen.left = nOffsetX + pRenderWord->nOffsetX;
		rectScreen.right = rectScreen.left + pRenderWord->nFontWidth;
		rectScreen.top = nOffsetY + pRenderWord->nOffsetY;
		rectScreen.bottom = rectScreen.top + pRenderWord->nFontHeight;
		if (pGUIRenderer)
		{
			pGUIRenderer->Render(rectScreen, pRenderWord->rectUV, /*pGUIRenderer->GetMinZ() + 0.01f*/1.0f, m_pTexture, color);
		}
		else
			pRenderer->Draw2DImage(rectScreen, pRenderWord->rectUV, m_pTexture, color);

		nOffsetX += pRenderWord->nWidth;

		nOffsetX = nOffsetX > area.right ? area.left : nOffsetX;

		if (nOffsetX == 0)
		{
			nOffsetY += pRenderWord->nHeight;
		}
	}
    
    //pRenderer->SetTextureFilterLinear(0, ETYPE_MAGNIFICATION);
}

void CGDIFont::Render(const char *szText, const AIR::Rectf& area, const CColorValue& color)
{
    int nLen = strlen(szText);
    wchar_t* wszText = AIR_NEW_ARRAY_T(wchar_t, nLen * 2 + 2);
    MByteToWChar(szText, wszText, nLen);
    Render(wszText, area, color);
    //delete [] wszText;
	AIR_DELETE_ARRAY_T( wszText, wchar_t, nLen * 2 + 2 );
    //CFontBase::Render(szText, area);
}

HFONT CGDIFont::CreateGDIFont(const FONTTYPE& ft, int nHeight)
{
    // 创建字体
    LOGFONT font;
    // 指定字符集，下列值是预定义的：
    font.lfCharSet			= GB2312_CHARSET | ANSI_CHARSET | DEFAULT_CHARSET;;
    // 指定裁剪精度，裁剪精度定义如何裁剪部分超出裁剪区的字符
    font.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
    // 指定移位向量和设备X轴之间的一个角度，以十分之一度为单位。移位向量平行于正文行的基线。
    font.lfEscapement		= 0;
    // 字体高度
    font.lfHeight			= nHeight;
    // 是否斜体
    font.lfItalic			= ft.m_bItalic;	
    // 指定每个字符的基线和设备X轴之间的角度。
    font.lfOrientation		= 0;			
    // 选择TrueType字体
    font.lfOutPrecision		= OUT_TT_PRECIS;													
    // 指定字体间距和字体族，低端二位指定字体的字符间距
    font.lfPitchAndFamily	= DEFAULT_PITCH;									
    // 指向输出质量，输出质量定义GDI如何仔细地将逻辑字体属性与实际物理字体属性相匹配。
    font.lfQuality			= PROOF_QUALITY;												
    // 删除线
    font.lfStrikeOut		= FALSE;
    // 下划线
    font.lfUnderline		= ft.m_bUnderLine;
    // 是否粗体
    font.lfWeight			= ( !ft.m_bBold ? FW_NORMAL : FW_BOLD );							
    // 指定所要求字体的字符的逻辑单位的平均宽度。
    font.lfWidth			= 0;		
    strcpy( font.lfFaceName,(const char *)ft.m_strFaceName.c_str() );
    m_hFont = ::CreateFontIndirect(&font);

    return m_hFont;
}

/*
void CGDIFont::AddRenderWord(CWordRenderable* pRenderable)
{
    m_mapWords.insert(make_pair(pRenderable->GetCode(), pRenderable));
}
*/

LPWORDINFO CGDIFont::FindWord(wchar_t wch)
{
    WORDINFO_ITER it = m_mapWordInfo.find(wch);

    if (it != m_mapWordInfo.end())
    {
        return it->second;
    }
    return NULL;
}

void CGDIFont::AddWords(const char* szWords)
{
    int nLen = (int)strlen(szWords);
    if (nLen == 0)
    {
        return;
    }
    wchar_t* wszText = AIR_NEW_ARRAY_T(wchar_t, nLen * 2 + 2);
    MByteToWChar(szWords, wszText, nLen * 2 + 2);
    AddWords(wszText);
    //delete [] wszText;
	//SafeDeleteArray( wszText );
    AIR_DELETE_ARRAY_T(wszText, wchar_t, nLen * 2 + 2);
}

void CGDIFont::AddWords(const wchar_t* wszWords)
{
    if (wszWords == NULL)
        return;

	size_t szlen = UnicodeStrLen(wszWords);
    for (u32 i = 0; i < szlen; i++)
    {
        wchar_t wCode = wszWords[i];
        AddWordToTexture(wCode);
    }
}

/*
CWordRenderable* CGDIFont::AddWord(wchar_t wCH)
{
    return NULL; //AddWordToTexture(wCH);
}
*/

LPWORDINFO CGDIFont::AddWord(wchar_t wCH)
{
    return NULL;
}

LPWORDINFO CGDIFont::GetWordInfo(wchar_t wch)
{
    WORDINFO_ITER it = m_mapWordInfo.find(wch);
    if (it != m_mapWordInfo.end())
    {
        return it->second;
    }

    return AddWordInfo(wch);
}

LPWORDINFO CGDIFont::AddWordInfo(wchar_t wch)
{
    return AddWordToTexture(wch);
}

LPWORDINFO CGDIFont::AddWordToTexture(wchar_t wCH)
{
    //如果已经存在，则返回
    LPWORDINFO pWord = FindWord(wCH);
    if (pWord)
    {
        return pWord;
    }

    CGDICanvas canvas;

    //m_type.m_nFontSize = MulDiv( m_type.m_nFontSize, ::GetDeviceCaps(canvas.GetDC(), LOGPIXELSY), 72 );
    int nHeight = -MulDiv( m_type.m_nFontSize, ::GetDeviceCaps(canvas.GetDC(), LOGPIXELSY), 72 );

    if (CreateGDIFont(m_type, nHeight) == NULL)
        return NULL;

    canvas.SetFont(m_hFont);

    

    u8* pPixel = NULL;
    u8* pTemp = NULL;

    // 将文字的UNICODE装化为多字节
    wchar_t str[10] = { wCH, 0 };
    char buf[10];
    WCharToMByte( str, buf, sizeof(str) );


    TEXTMETRICW tm;
    GetTextMetricsW( canvas.GetDC(), &tm );
    m_nFontHeight = tm.tmHeight;
    //SIZE ws = dib->m_pCanvas->GetTextSize( AIRString(buf) );
    SIZE ws; // = dib->m_pCanvas->GetTextSize( AIRString(buf) );
    //SIZE		sText;
    memset( &ws, 0x0, sizeof(SIZE) );
    GetTextExtentPoint32( canvas.GetDC(), buf, (int)strlen(buf), &ws );
    MAT2         mmat2 = {0,1,0,0,0,0,0,1};
    GLYPHMETRICS glpm;
    u32        nLen;
    //u8         temp;
    nLen = ::GetGlyphOutlineW( canvas.GetDC(), wCH, m_type.m_nFontHint, &glpm, 0, NULL, &mmat2 );
    if (nLen == 0 && wCH != ' ')
    {
        return NULL;
    }

    int nBorderOffset = m_type.m_nFontHint == FONT_BMP ? m_type.m_nBorderSize : 0;

    pWord = AIR_NEW_T(WORDINFO);
    pWord->wCode = wCH;
    pWord->nWidth = glpm.gmCellIncX + nBorderOffset * 2;
    pWord->nHeight = tm.tmHeight + nBorderOffset * 2;
    pWord->nFontWidth = glpm.gmBlackBoxX + nBorderOffset * 2;
    pWord->nFontHeight = glpm.gmBlackBoxY + nBorderOffset * 2;
    pWord->nOffsetX = glpm.gmptGlyphOrigin.x - nBorderOffset;
    pWord->nOffsetY = tm.tmAscent - glpm.gmptGlyphOrigin.y - nBorderOffset;

    if (wCH == ' ')  //如果是空格
    {
        m_nSpaceWidth = glpm.gmCellIncX;
		pWord->nOffsetX = m_nSpaceWidth;
		pWord->nFontWidth = m_nSpaceWidth;
		pWord->nOffsetY = m_nFontHeight;
		pWord->nHeight = m_nFontHeight;
		pWord->nFontHeight = m_nFontHeight;
        m_mapWordInfo.insert(make_pair(wCH, pWord));
        return pWord;
        //return NULL;
    }

    m_nCharLineW = ( glpm.gmBlackBoxX / 4 + ( glpm.gmBlackBoxX % 4 == 0 ? 0 : 1 ) ) * 4;
    // 字符的实际大小
    u32 realH = /*glpm.gmBlackBoxY;*/ nLen / m_nCharLineW;
    u32 realW = /*glpm.gmBlackBoxX;*/  m_nCharLineW;

    if (m_type.m_nFontHint == FONT_BMP)
    {
        realH = glpm.gmBlackBoxY;
        realW = glpm.gmBlackBoxX;

    }
    // 字符的兼容大小
    u32 nGridHeight = AIR::Max<LONG>(ws.cy, (LONG)realH);
    u32 nGridWidth  = AIR::Max<LONG>(ws.cx, (LONG)realW);
    //int nRBHeight = AIR::Get2Pow( nGridHeight );
    //int nRBWidth  = AIR::Get2Pow( nGridWidth );
    u32 cnt = nGridHeight * nGridWidth * 4;

    if( (signed)nLen > 0 )
    {
        // 从内存块管理器申请缓冲区以保存文字的点阵数据
        //pMBBuf = MEMORY_MGR->GetFreeMemBlock( nLen );
        //ASSERT( pMBBuf );

        // 锁定该内存块
        //pMBBuf->Lock();

        u32 nLentemp;
        // 带灰度的模式
        // 开辟缓冲区保存文字像素数据


        pTemp = AIR_NEW_ARRAY_T(u8, nLen);
        memset(pTemp, 0, nLen * sizeof(u8));
        pPixel = AIR_NEW_ARRAY_T( u8, nLen * 4);
        memset(pPixel, 0, nLen * 4 * sizeof(u8));

        nLentemp = ::GetGlyphOutlineW( canvas.GetDC(), wCH, m_type.m_nFontHint, &glpm, nLen, pTemp/*m_pBuf*/, &mmat2 );

        m_nGlyWidth = nLentemp / glpm.gmBlackBoxY;
    }

    if (m_nOffsetX + glpm.gmBlackBoxX + nBorderOffset * 2 > m_pTexture->GetWidth())
    {
        m_nOffsetX = 0;
        m_nOffsetY += tm.tmHeight + nBorderOffset * 2;
    }

    if (m_type.m_nFontHint != FONT_BMP)
    {

        //首先把像素写入一段内存

        //int row, pixel;
        //int rBegin, rEnd, cBegin, cEnd, rOff, cOff;
        //int i = 0;

        //rBegin = (nGridHeight - realH) / 2;
        //rEnd   = rBegin + realH;
        //cBegin = (nGridWidth - realW) / 2;
        //cEnd   = cBegin + realW;
        //计算灰度，暂定64位
        int m = ((m_type.m_nFontHint == FONT_BMP_GRAY2 ) ? 4 : (( m_type.m_nFontHint == FONT_BMP_GRAY4 ) ? 16 : 64));

        for (u32 nRow = 0; nRow < glpm.gmBlackBoxY; nRow++)
        {
            for (u32 nCol = 0; nCol < glpm.gmBlackBoxX; nCol++)
            {
                u32 nIndex = (nRow * glpm.gmBlackBoxX + nCol) * 4;
                //int col = pTemp[i++] * 255 / m;

                //int index_color = 4 * (int)glpm.gmBlackBoxX *y + 4*x;

                u32 mono_pitch_bytes = (((8 * (int)glpm.gmBlackBoxX + 31) & (~31)) / 8);
                u32 index_monob = mono_pitch_bytes * nRow + nCol;

                u8 col = (u8)(pTemp[index_monob] / (float)m * 255); //Alpha
                //col = col < 255 ? 0 : 255;
                //col = col < 30 ? 0 : ( col > 230 ? 255 : col );
                pPixel[ nIndex ]   = 255;
                pPixel[ nIndex + 1 ] = 255;
                pPixel[ nIndex + 2 ] = 255;
                pPixel[ nIndex + 3 ] = col;
            }
        }

        // 将文字的像素保存到纹理中
        if (m_nOffsetY < m_pTexture->GetHeight())
            SaveBitmapPixelToTexture(pWord, pPixel, m_nOffsetX, m_nOffsetY, glpm.gmBlackBoxX, glpm.gmBlackBoxY, 32);
        else
        {
            //SafeDeleteArray(pTemp);
            //SafeDeleteArray(pPixel);
            AIR_DELETE_ARRAY_T(pTemp, u8, nLen);
            AIR_DELETE_ARRAY_T(pPixel, u8, nLen * 4);
			SafeDelete(pWord);
            return false;
        }
    }
    else
    {
        if (m_type.m_nBorderSize > 0)
        {
            SColor clrBorder(255, 0, 0, 0);
            //左边
            SaveBitmapPixelToTexture(pWord, pTemp, m_nOffsetX, m_nOffsetY + m_type.m_nBorderSize, glpm.gmBlackBoxX, glpm.gmBlackBoxY, 1, clrBorder);
            //左上边
            SaveBitmapPixelToTexture(pWord, pTemp, m_nOffsetX, m_nOffsetY, glpm.gmBlackBoxX, glpm.gmBlackBoxY, 1, clrBorder);
            //上边
            SaveBitmapPixelToTexture(pWord, pTemp, m_nOffsetX + m_type.m_nBorderSize, m_nOffsetY, glpm.gmBlackBoxX, glpm.gmBlackBoxY, 1, clrBorder);
            //右上边
            SaveBitmapPixelToTexture(pWord, pTemp, m_nOffsetX + m_type.m_nBorderSize * 2, m_nOffsetY, glpm.gmBlackBoxX, glpm.gmBlackBoxY, 1, clrBorder);
            //右边
            SaveBitmapPixelToTexture(pWord, pTemp, m_nOffsetX + m_type.m_nBorderSize * 2, m_nOffsetY + m_type.m_nBorderSize, glpm.gmBlackBoxX, glpm.gmBlackBoxY, 1, clrBorder);
            //右下边
            SaveBitmapPixelToTexture(pWord, pTemp, m_nOffsetX + m_type.m_nBorderSize * 2, m_nOffsetY + m_type.m_nBorderSize * 2, glpm.gmBlackBoxX, glpm.gmBlackBoxY, 1, clrBorder);
            //下边
            SaveBitmapPixelToTexture(pWord, pTemp, m_nOffsetX + m_type.m_nBorderSize, m_nOffsetY + m_type.m_nBorderSize * 2, glpm.gmBlackBoxX, glpm.gmBlackBoxY, 1, clrBorder);
            //左下边
            SaveBitmapPixelToTexture(pWord, pTemp, m_nOffsetX, m_nOffsetY + m_type.m_nBorderSize * 2, glpm.gmBlackBoxX, glpm.gmBlackBoxY, 1, clrBorder);
        }
        // 将文字的像素保存到纹理中
        if (m_nOffsetY < m_pTexture->GetHeight())
            SaveBitmapPixelToTexture(pWord, pTemp, m_nOffsetX + m_type.m_nBorderSize, m_nOffsetY + m_type.m_nBorderSize, glpm.gmBlackBoxX, glpm.gmBlackBoxY);
        else
        {
            //SafeDeleteArray(pTemp);
            //SafeDeleteArray(pPixel);
            AIR_DELETE_ARRAY_T(pTemp, u8, nLen);
            AIR_DELETE_ARRAY_T(pPixel, u8, nLen * 4);
			SafeDelete(pWord);
            return false;
        }
    }

    //pWord->SetSize(glpm.gmCellIncX, tm.tmHeight);
    float fLeft = (float)m_nOffsetX / m_pTexture->GetWidth();
    float fTop = (float)m_nOffsetY / m_pTexture->GetWidth();
    float fRight = fLeft + (float)(glpm.gmBlackBoxX + m_type.m_nBorderSize * 2) / m_pTexture->GetWidth();
    float fBottom = fTop + (float)(glpm.gmBlackBoxY + m_type.m_nBorderSize * 2) / m_pTexture->GetWidth();
    pWord->rectUV = Rectf(fLeft, fTop, fRight, fBottom);
    //pWord->rectUV = Rectf((float)m_nOffsetX / m_pTexture->GetWidth(), (float)m_nOffsetY / m_pTexture->GetWidth(), 
    //    (float)glpm.gmBlackBoxX / m_pTexture->GetWidth(), (float)glpm.gmBlackBoxY / m_pTexture->GetWidth());

    m_nOffsetX += glpm.gmBlackBoxX + nBorderOffset * 2;
    m_nOffsetX = m_nOffsetX < m_pTexture->GetWidth() ? m_nOffsetX : 0;

    if (m_nOffsetX == 0)
    {
        m_nOffsetY += tm.tmHeight + nBorderOffset * 2;
        m_nOffsetY += m_nOffsetX > m_pTexture->GetWidth() ? (tm.tmHeight + nBorderOffset * 2) : 0;
    }
    //}

    // 释放申请的内存块
    //SafeDeleteArray(pTemp);
    //SafeDeleteArray(pPixel);
    AIR_DELETE_ARRAY_T(pTemp, u8, nLen);
    AIR_DELETE_ARRAY_T(pPixel, u8, nLen * 4);

    //AddRenderWord(pWord);
    m_mapWordInfo.insert(make_pair(wCH, pWord));

    return pWord;
}

void CGDIFont::Render(const Rectf& rectScreen, const LPWORDINFO pWordInfo, f32 fDepth, const CColorValue& color /* = 0xffffffff */)
{
	PreRender();
	ENGINE_INST->GetGUIRenderer()->Render(rectScreen, pWordInfo->rectUV, fDepth, m_pTexture, color);
}

bool CGDIFont::SaveFontTextureToFile(const char* szFilename)
{
    if (m_pTexture)
    {
        return m_pTexture->SaveToFile(szFilename);
    }
    return false;
}

bool CGDIFont::CreateFontTexture()
{
    if (m_pTexture == NULL)
    {
        //IRenderer* pRenderer = ENGINE_INST->GetRenderer();
        TextureParam param;
        param.pixelFormat = RPF_A8R8G8B8;
        param.nWidth = 512;
        param.nHeight = 512;
        param.nMipmaps = 1;
        m_pTexture = TEXTURE_MANAGER->CreateTexture(GetString("%s_%d_%d_%d_%d_%d", m_type.m_strFaceName.c_str(), m_type.m_bBold, m_type.m_bItalic, m_type.m_bUnderLine, 
            m_type.m_nFontHint, m_type.m_nFontSize), param/*512, 512, 1*/);

        m_nTexturePixelBytes = EngineParamSetting::GetPixelFormatSize(m_pTexture->GetPixelFormat());

		if (m_pTexture && m_pFontTexBuffer == NULL)
		{
			m_pFontTexBuffer = AIR_NEW_ARRAY_T(u8, 512 * 512 * EngineParamSetting::GetPixelFormatSize(m_pTexture->GetPixelFormat()));
			memset(m_pFontTexBuffer, 0, 512 * 512 * EngineParamSetting::GetPixelFormatSize(m_pTexture->GetPixelFormat()));
		}
    }
    return true;
}

void CGDIFont::PreRender()
{
	if (m_bTextureDirty)
	{
		UpdateTexture();
		m_bTextureDirty = false;
	}
}

void CGDIFont::UpdateTexture()
{
	const TextureParam& param = m_pTexture->GetTextureParam();
	u32 nBytes = param.pixelFormat == RPF_L8 ? 1 : 4;
    m_pTexture->CopyPixelBoxToTexture((const u8*)m_pFontTexBuffer, NULL, 0, 512 * 512 * nBytes, 512 * nBytes, 4);
	static bool bSaveTexture = false;

	if (!bSaveTexture)
	{
		//保存成文件
		m_pTexture->SaveToFile("D:\\font.png");
		m_pTexture->SaveToFile("D:\\font.bmp");
		m_pTexture->SaveToFile("D:\\font.dds");
		bSaveTexture = true;
	}
}

bool CGDIFont::CopyMonoFontToTextureBuffer(const u8* pPixel, int nPosX, int nPosY, int nWidth, int nHeight, int nGlyWidth, SColor color /* = SColor */)
{
	int nTexRow = 0;
	int nTexCol = 0;
	int nOffsetPixel = 0;
	int nOffsetTex = (nPosY * 512 + nPosX) * 4;
	//int col = 0;
	for( int row = 0; row < nHeight; row++ )
	{
		nTexRow = row + nPosY;
		for (int col = 0; col < nGlyWidth; col++)
		{
			nOffsetPixel = row * nGlyWidth + col;
			for (int i = 0; i < 8; i++)
			{
				nTexCol = nPosX + col;
				//int nOffsetTex = (nTexRow * m_nWidth + nTexCol) * 4;
				//int nOffsetPixel = row * nOffsetX + col;
				if  ( col * 8 + i >= nWidth )
				{
					//nOffsetPixel--;
					break;
				}

				u8 nGray = (pPixel[nOffsetPixel] & ( 0x80 >> i));
				if (nGray)
				{
					*(UINT *)&m_pFontTexBuffer[nOffsetTex] = color.color;
				}
				else
				{
					//*(UINT *)&pTexData[nOffsetTex] = 0;

				}
				//col++; 
				nOffsetTex += sizeof(u32);

			}

		}
		nOffsetTex += (512 * sizeof(u32) - nWidth * sizeof(u32));
	}
	return true;
}

bool CGDIFont::CopyPixelToTextureBuffer(const u8* pPixel, int nPosX, int nPosY, int nOffsetX, int nOffsetY, int nBits /* = 1 */)
{
	if (nBits == 8)
	{
		//D3DLOCKED_RECT d3dlr;

		//m_pInterface->LockRect( 0, &d3dlr, NULL/*&rc*/, 0 );
		//u8* pTexData = (u8*)d3dlr.pBits;

		int nTexRow = 0;
		int nTexCol = 0;
		int nOffsetPixel = 0;
		//int col = 0;
		for( int row = 0; row < nOffsetY; row++ )
		{
			nTexRow = row + nPosY;
			for (int col = 0; col < nOffsetX; col++)
			{
				nOffsetPixel = row * nOffsetX + col;
				for (int i = 0; i < 8; i++)
				{
					nTexCol = nPosX + col;
					int nOffsetTex = (nTexRow * m_pTexture->GetWidth() + nTexCol) * 4;
					//int nOffsetPixel = row * nOffsetX + col;
					if  ( col + i >= nOffsetX )
					{
						//nOffsetPixel--;
						break;
					}

					u8 nGray = (pPixel[nOffsetPixel] & ( 0x80 >> i));
					if (nGray)
					{
						*(UINT *)&m_pFontTexBuffer[nOffsetTex] = 0xffffffff;
					}
					else
					{
						*(UINT *)&m_pFontTexBuffer[nOffsetTex] = 0;

					}
					//col++;

				}

			}
		}
		//m_pInterface->UnlockRect(0);

	}
	else
	{
		//Load colorized bitmap buffer into texture surface

		int size  = nOffsetX * 4;
		int sid, did;
		for( int i = nPosY; i < nPosY + nOffsetY; ++i )
		{
			did = ( i * m_pTexture->GetHeight() + nPosX ) * 4;
			sid = (i - nPosY) * size;
			memcpy( &m_pFontTexBuffer[did], &pPixel[sid], size );
		}
	}
	return true;
}


#endif