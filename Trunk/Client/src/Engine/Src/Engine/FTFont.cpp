#include "stdafx.h"
#include "FTFont.h"
//#include "Include/freetype/ftoutln.h"
#include "Engine.h"
#include "TextureMgr.h"
#include "Util.h"
#include "FileSystem.h"
#include "MemoryAllocatorConfig.h"
#include "StringUtil.h"
#include "ftoutln.h"
#include "ftbitmap.h"

//#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
//	#define CREATE_TEX_WITH_RGBA
//#endif

CFTFont::CFTFont():
m_nTexHeight(0),
m_nTexWidth(0),
m_pTexture(NULL),
m_nOffsetX(0), 
m_nOffsetY(0), 
m_nNextWordPosX(0),
m_nNextWordPosY(0)
, m_pFontTexBuffer(0)
, m_bTextureDirty(false)
, m_nBaseLine(0)
, m_nUnderLinePos(0)
, m_nUnderLineThick(0)
, m_ftFace(NULL)
, m_ftSlot(NULL)
{

}

CFTFont::~CFTFont()
{
    Release();
}

bool CFTFont::Create(const FONTTYPE &ft, const char *szWords)
{
    int nLen = strlen(szWords);
    wchar_t* wszText = AIR_NEW_ARRAY_T(wchar_t, nLen * 2 + 2);
    MByteToWChar(szWords, wszText, nLen * 2 + 2);
    bool bRes = CreateW(ft, wszText);
    //SafeDeleteArray( wszText );
    AIR_DELETE_ARRAY_T(wszText, wchar_t, nLen * 2 + 2);
    return bRes;
}

bool CFTFont::CreateW(const FONTTYPE& ft, const wchar_t* szWords)
{
    m_type = ft; 
	if (ft.m_nFontHint == GGO_BITMAP)
	{
		m_type.m_nFontHint = FONT_BITMAP;
	}
	else
		m_type.m_nFontHint = FONT_MONO;
    //创建字体
    if ( !m_pTexture )
    {
        CreateFontTexture();
    }
    if ( !CreateFTFont(ft) )
    {
        return false;
    }

	int szlen = UnicodeStrLen( szWords );
    for (size_t i = 0 ; i < szlen; i++)
    {
        wchar_t wch = szWords[i];
        LPWORDINFO lpWord = FindWord(wch);
        if ( !lpWord )
        {
            AddWordToTexture(wch);
        }
    }
    
    return true;
}

void CFTFont::Render(const wchar_t* szText, const AIR::Rectf& area, const CColorValue& color)
{
    int  nOffsetX = area.left;
    int  nOffsetY = area.top;


    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    IGUIRenderer* pGUIRenderer = ENGINE_INST->GetGUIRenderer();
    //pRenderer->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
    //pRenderer->SetTextureFilterPoint(0, ETYPE_MAGNIFICATION);
    Rectf rectScreen;
    AIRVector<LPWORDINFO> vecWords;

	s32 nFrameWidth;
	s32 nFrameHeight;
	pRenderer->GetBackBufferSize(nFrameWidth, nFrameHeight);

	u32 szLen = UnicodeStrLen(szText);
    for (u32 i = 0; i < szLen; i++)
    {
		wchar_t wchWord = szText[i];
        LPWORDINFO pRenderWord = FindWord(wchWord);
        if (pRenderWord)
        {
            vecWords.push_back(pRenderWord);
        }
        else
        {
            //写入纹理，再渲染
            pRenderWord = AddWordToTexture(wchWord);

            vecWords.push_back(pRenderWord);
            //pRenderer->Draw2DImage(rectScreen, pRenderWord->rectUV, m_pTexture, color);
            //ENGINE_INST->GetGUIRenderer()->Render(rectScreen, pRenderWord->rectUV, m_pTexture, color);
        }
        /*
        nOffsetX += pRenderWord->nWidth;

        nOffsetX = nOffsetX > area.right ? area.left : nOffsetX;

        if (nOffsetX == 0)
        {
            nOffsetY += pRenderWord->nHeight;
        }
        */
    }

    PreRender();
    for (size_t i = 0; i < vecWords.size(); ++i)
    {
        LPWORDINFO pRenderWord = vecWords[i];
        rectScreen.left = nOffsetX + pRenderWord->nOffsetX;
		if (rectScreen.left + pRenderWord->nFontWidth > nFrameWidth)
		{
			nOffsetX = area.left;
			nOffsetY += pRenderWord->nFontHeight;
			rectScreen.left = nOffsetX + pRenderWord->nOffsetX;
		}
        rectScreen.right = rectScreen.left + pRenderWord->nFontWidth;
        rectScreen.top = nOffsetY + pRenderWord->nOffsetY;
        rectScreen.bottom = rectScreen.top + pRenderWord->nFontHeight;
        if (pGUIRenderer)
        {
            pGUIRenderer->Render(rectScreen, pRenderWord->rectUV, /*pGUIRenderer->GetMinZ() + */0.01f, m_pTexture, color);
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
}

void CFTFont::Render(const char* szText, const AIR::Rectf& area, const CColorValue& color)
{
    int nLen = strlen(szText);
    wchar_t* wszText = AIR_NEW_ARRAY_T(wchar_t, nLen * 2 + 2);
    MByteToWChar(szText, wszText, nLen);
    Render(wszText, area, color);
    AIR_DELETE_ARRAY_T( wszText, wchar_t, nLen * 2 + 2 );
}

void CFTFont::Render(const Rectf& rectScreen, const LPWORDINFO pWordInfo, f32 fDepth, const CColorValue& color)
{
    ENGINE_INST->GetGUIRenderer()->Render(rectScreen, pWordInfo->rectUV, fDepth, m_pTexture, color);
}

void CFTFont::AddWords(const char* szWords)
{
    size_t nLen = strlen(szWords) + 1;
    //wchar_t* wszWords = new wchar_t[nLen * 2];
    //MultiByteToWideChar(CP_ACP, 0, szWords, nLen, wszWords, nLen * 2); 
    //AddWords(wszWords);
	AddWords( CStringUtil::s2ws(szWords).c_str() );

}

void CFTFont::AddWords(const wchar_t* wszWords)
{
	int szlen = UnicodeStrLen(wszWords);
    for (size_t i = 0 ; i < szlen ; i++)
    {
        wchar_t wch = wszWords[i];
        LPWORDINFO lpwordInfo = FindWord(wch);
        if (lpwordInfo)
        {
            continue ;
        }
        AddWordToTexture(wch);
    }
}

LPWORDINFO CFTFont::AddWord(wchar_t wCH)
{
    return NULL ;
}

void CFTFont::Release()
{
    if (m_pTexture)
    {
        TEXTURE_MANAGER->ClearTexture(m_pTexture);
        m_pTexture = NULL;
    }


    for (WORDINFO_ITER it = m_mapWordInfo.begin(); it != m_mapWordInfo.end(); it++)
    {
        LPWORDINFO pWord = it->second;
        AIR_DELETE_T(pWord, WORDINFO);
    }

    m_mapWordInfo.clear();

    if (m_pFontTexBuffer)
    {
        AIR_DELETE_ARRAY_T(m_pFontTexBuffer, u8, 512 * 512 * m_nTexturePixelBytes);
        m_pFontTexBuffer = 0;
    }
}




LPWORDINFO CFTFont::FindWord(wchar_t wch)
{
     WORDINFO_ITER it = m_mapWordInfo.find(wch);
     if ( it != m_mapWordInfo.end() )
     {
        return it->second;
     }
     return NULL;
}

LPWORDINFO CFTFont::AddWordToTexture(wchar_t wCH)
{
    LPWORDINFO lpWord = FindWord(wCH);
    if ( lpWord )
    {
       return lpWord;
    }

	//FT_Set_Pixel_Sizes(m_ftFace, m_type.m_nFontSize, m_type.m_nFontSize);
	FT_Error error = 0;
	FT_Int32 nLoadFlag = m_type.m_nFontHint == FONT_MONO ? FT_LOAD_FORCE_AUTOHINT | FT_LOAD_MONOCHROME : FT_LOAD_FORCE_AUTOHINT | FT_LOAD_RENDER;/*FT_LOAD_NO_BITMAP*/;
    error = FT_Load_Char(m_ftFace, wCH, nLoadFlag);

	if (error)
	{
		error = FT_Load_Char(m_ftFace, wCH, FT_LOAD_DEFAULT);
	}

	if (error)
	{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
		MessageBox(NULL, "ft 没返回这个字", NULL, NULL);
#endif
		PrintDebugString( "freetype return no word.\n" );
		return NULL;
	}
    //加粗
	FT_Pos xBold = 32;//1 << 4;  
	FT_Pos yBold = 32;//1 << 4;
	if (m_bBold)
	{
		if (m_ftFace->glyph->format == FT_GLYPH_FORMAT_OUTLINE)
		{
			FT_BBox oldBox;
			FT_Outline_Get_CBox(&m_ftSlot->outline , &oldBox);
			error = FT_Outline_Embolden(&m_ftSlot->outline, xBold);

			FT_BBox newBox;
			FT_Outline_Get_CBox(&m_ftSlot->outline , &newBox);
			xBold = (newBox.xMax - newBox.xMin) - (oldBox.xMax - oldBox.xMin);
			yBold = (newBox.yMax - newBox.yMin) - (oldBox.yMax - oldBox.yMin);
		}
		else if (m_ftFace->glyph->format == FT_GLYPH_FORMAT_BITMAP)
		{
			FT_Library ftLibrary = FTFaceMgr::GetInstance()->GetFTLibrary();
			error = FT_Bitmap_Embolden( ftLibrary, &m_ftSlot->bitmap, xBold, yBold );
			if ( error )
			{
				int a = 0;
			}
		}
	}
    //斜体
    if (m_bItailc)
    {
        FT_Outline_Transform(&m_ftSlot->outline, &m_matItailc);
    }

    //FT_Error error = FT_Render_Glyph(m_ftSlot, FT_RENDER_MODE_MONO);
	if (m_ftFace->glyph->format != FT_GLYPH_FORMAT_BITMAP)
	{
		error = FT_Render_Glyph(m_ftSlot, FT_RENDER_MODE_NORMAL);
	}

	m_ftSlot = m_ftFace->glyph;

	if (m_bBold && error == 0)
	{
		if ( m_ftSlot->advance.x )
			m_ftSlot->advance.x += xBold;

		if ( m_ftSlot->advance.y )
			m_ftSlot->advance.y += yBold;

		m_ftSlot->metrics.width        += xBold;
		m_ftSlot->metrics.height       += yBold;
		m_ftSlot->metrics.horiBearingY += yBold;
		m_ftSlot->metrics.horiAdvance  += xBold;
		m_ftSlot->metrics.vertBearingX -= xBold / 2;
		m_ftSlot->metrics.vertBearingY += yBold;
		m_ftSlot->metrics.vertAdvance  += yBold;

		if ( m_ftSlot->format == FT_GLYPH_FORMAT_BITMAP )
			m_ftSlot->bitmap_top += yBold >> 6;
	}

    m_nOffsetX = m_nNextWordPosX; 
    m_nOffsetY = m_nNextWordPosY;
    
    u32 nWordWidth = m_ftSlot->bitmap.width;
	u32 nWordHeight = m_ftSlot->bitmap.rows;
    if( (m_nOffsetX + nWordWidth) > m_nTexWidth)
    {
        //应该写到下一行中
        m_nOffsetX  = 0;
        m_nOffsetY += m_nHeight; 

    }

    lpWord = AIR_NEW_T (WORDINFO);
    lpWord->wCode             = wCH;
    lpWord->nHeight           = m_nHeight;
    lpWord->nWidth            = m_ftSlot->advance.x >> 6;//m_ftSlot->metrics.width / 64; //
    lpWord->nOffsetX          = m_ftSlot->metrics.horiBearingX >> 6;
    lpWord->nOffsetY          = 0; //m_ftSlot->metrics.horiBearingY >> 6; //0;
    lpWord->nFontWidth        = nWordWidth;
    lpWord->nFontHeight       = m_nHeight;

    lpWord->rectUV.left       = (m_nOffsetX ) * 1.0f / m_nTexWidth;
    lpWord->rectUV.top        = (m_nOffsetY ) * 1.0f / m_nTexHeight;
    lpWord->rectUV.right      = (m_nOffsetX + nWordWidth ) * 1.0f / m_nTexWidth;
    lpWord->rectUV.bottom     = (m_nOffsetY + m_nHeight ) * 1.0f / m_nTexHeight;
    //计算下一字体要在纹理的位置
    m_nNextWordPosX = nWordWidth + m_nOffsetX;
    m_nNextWordPosY = m_nOffsetY;

    int nPenx = (int)m_ftSlot->metrics.horiBearingX / 64;
	if (nPenx < 0)
	{
		nPenx = 0;
	}

	//s32 bBaseLine = m_type.m_bAsPound ? m_nAcender : m_nBaseLine;
    int nPenY = m_type.m_bAsPound ? m_nAcender - (int)(m_ftSlot->metrics.horiBearingY / 64) : m_nBaseLine  - (int)(m_ftSlot->metrics.horiBearingY / 64) + 1;
	//nWordWidth += nPenx;
	if (nPenY < 0)
	{
		lpWord->nOffsetY = nPenY;
		nPenY = 0;
	}

	//if (nWordHeight + 1 == m_nFontHeight && m_bBold)
	//{
	//	nPenY = 1;
	//}

    int nLen = nWordWidth * m_nHeight;

	if( nLen > 0 )
	{
//#ifdef CREATE_TEX_WITH_RGBA
//		u32 pixelsize = nLen * 4;
//#else
		u32 pixelsize = nLen * m_nTexturePixelBytes;
//#endif
		unsigned char* pPixel = AIR_NEW_ARRAY_T(unsigned char, pixelsize);
		memset( pPixel, 0x00, pixelsize );

		unsigned char* buf = m_ftSlot->bitmap.buffer;
		if ( m_ftSlot->bitmap.pixel_mode == FT_PIXEL_MODE_MONO )
		{
			for (int r = 0 ; r < m_ftSlot->bitmap.rows; r++)
			{
				if (r + nPenY >= m_nHeight)
				{
					continue;
				}

				if (r + nPenY < 0)
				{
					continue;
				}
				int index = ( (r + nPenY)* nWordWidth + nPenx );
//#ifdef CREATE_TEX_WITH_RGBA
//				index *= 4;	
//#endif
				unsigned char * pDes = & pPixel[index];
				unsigned char * src = &buf[ r * m_ftSlot->bitmap.pitch ];
				for (int l = 0 ; l < m_ftSlot->bitmap.width; l++)
				{
//#ifdef CREATE_TEX_WITH_RGBA
//					*pDes++ =0xFF;
//					*pDes++ =0xFF;
//					*pDes++ =0xFF;
//#endif
					*pDes++ = (src [l / 8] & (0x80 >> (l & 7))) ? 0xFF:0x00;
				}
			}
		}
		else if ( m_ftSlot->bitmap.pixel_mode == FT_PIXEL_MODE_GRAY )
		{
			for (int r = 0 ; r < m_ftSlot->bitmap.rows ; r ++)
			{
				//if (r + nPenY > m_nHeight || r + nPenY < 0)
				//{
				//	continue;
				//}

				//int index = ( (r + nPenY) * nWordWidth + nPenx );
//#ifdef CREATE_TEX_WITH_RGBA
//				index *= 4;	
//#endif
				//unsigned char *pDes = &pPixel[index];
				//u8* pSrc = m_ftSlot->bitmap.buffer + r * m_ftSlot->bitmap.pitch;
				//unsigned char* pDes = pPixel + nWordWidth * (r + nPenY) + nPenx;
				//unsigned char* pSrc = buf + (r * m_ftSlot->bitmap.pitch);
				u8* pSrc = m_ftSlot->bitmap.buffer + r * m_ftSlot->bitmap.pitch;
				for (int c = 0 ; c < m_ftSlot->bitmap.width ; c ++)
				{
					//if (c + nPenx >= nWordWidth)
					//{
					//	continue;
					//}

//#ifdef CREATE_TEX_WITH_RGBA
//					*pDes++ = 0xff;
//					*pDes++ = 0xff;
//					*pDes++ = 0xff;
//#endif
					//*pDes++ = buf[ r * m_ftSlot->bitmap.pitch + c];
					
					int index = ((r + nPenY) * nWordWidth + c);
					index *= m_nTexturePixelBytes;
					
					if (m_nTexturePixelBytes == 1)
					{
						u8* pDes = &pPixel[index];
						*pDes = *pSrc;
					}
					else if (m_nTexturePixelBytes == 2)
					{
						u16* pDes = (u16*)(&pPixel[index]);
						*pDes = (*pSrc & 0xff) << 8 | 0x0fff;
					}
					else if (m_nTexturePixelBytes == 4)
					{
						u32* pDes = (u32*)(&pPixel[index]);
						*pDes = (*pSrc & 0xff) << 24 | 0x00ffffff;
					}
					
					pSrc++;
					//pDes++;
				 }
			}
		}

		if (nWordWidth > 0)
		{
//#ifdef CREATE_TEX_WITH_RGBA
//			CopyPixelToTextureBuffer(pPixel, m_nOffsetX, m_nOffsetY, nWordWidth, m_nHeight, 32);
//#else
			CopyPixelToTextureBuffer(pPixel, m_nOffsetX, m_nOffsetY, nWordWidth, m_nHeight, 8);
//#endif
			//m_pTexture->CopyPixelToTexture(pPixel, m_nOffsetX, m_nOffsetY, nWordWidth, m_nHeight, 32 );
		}
    
		//delete pPixel;
		AIR_DELETE_ARRAY_T(pPixel, unsigned char, pixelsize);
		pPixel = NULL;
	}
	
	//lpWord = AIR_NEW_T (WORDINFO);
	//GetCharGlyphInfo(wCH, lpWord);
    m_mapWordInfo[wCH] = lpWord;
    return lpWord;
}


LPWORDINFO CFTFont::GetWordInfo(wchar_t wch)
{
    LPWORDINFO lpwordInfo = FindWord(wch);
    if ( lpwordInfo )
    {
        return lpwordInfo;
    }
    return AddWordToTexture(wch);
}

bool CFTFont::SaveFontTextureToFile(const char *szFilename)
{
    if (m_pTexture)
    {
        return m_pTexture->SaveToFile(szFilename);
    }
    return false;
}

bool CFTFont::CreateFTFont(const FONTTYPE& ft)
{
	/*
    FT_Error error;
    error = FT_Init_FreeType(&m_ftLibrary);
    if ( error )
    {
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
        MessageBox(NULL,"init freetype error", 0, 0);
#endif
		PrintDebugString( "init freetype error!\n" );
        return false;
    }
    AIRString strFontFile = FILESYSTEM->GetFileFullPath( ft.m_strFileName.c_str());
    error = FT_New_Face(m_ftLibrary, strFontFile.c_str(), 0, &m_ftFace);
    if ( error )
    {
        if ( error == FT_Err_Unknown_File_Format )
        {
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
            MessageBox(NULL,"可以打开和读这个文件，但不支持它的字体格式", 0, 0);
#endif
			PrintDebugString( "unknown freetype font file format!\n" );
        }
        else if (error)
        {
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
            MessageBox(NULL,"new face error", 0, 0);
#endif
			PrintDebugString( "FT_New_Face() error!\n" );
        }
    }
	*/
	m_type = ft;
	m_ftFace = FTFaceMgr::GetInstance()->GetFontFace(ft.m_strFileName.c_str());
    
    //设置字体大小
    //假设设备分辨率为100pdi
	if (ft.m_bAsPound)
		FT_Set_Char_Size(m_ftFace, FT_F26Dot6(ft.m_nFontSize << 6), FT_F26Dot6(ft.m_nFontSize << 6), 96, 96);
	else
		FT_Set_Pixel_Sizes(m_ftFace, ft.m_nFontSize, ft.m_nFontSize);

    m_bBold   = ft.m_bBold;
    m_bItailc = ft.m_bItalic;

    //设置斜体矩阵
    m_matItailc.xx = 1 << 16;
    m_matItailc.xy = 0x5800;
    m_matItailc.yx = 0;
    m_matItailc.yy = 1 << 16;

	float fScale = (f32)ft.m_nFontSize / (float)(m_ftFace->ascender - m_ftFace->descender);

    m_nAcender   = m_ftFace->size->metrics.ascender >> 6;
    m_nDescender = m_ftFace->size->metrics.descender >> 6;

	m_nBaseLine = (s32)(m_ftFace->ascender * fScale);
	m_nUnderLinePos = (s32)(m_ftFace->underline_position * fScale);
	m_nUnderLineThick = (s32)(m_ftFace->underline_thickness * fScale);
	if (m_nUnderLineThick < 1)
	{
		m_nUnderLineThick = 1;
	}

    //在每个纹理中
    m_nHeight    = m_nAcender + AIR::Abs( m_nDescender );
    m_nFontHeight = m_nHeight;
    m_ftSlot = m_ftFace->glyph;
    //CreateFontTexture();
    return true;
}

bool CFTFont::CreateFontTexture()
{
    if (!m_pTexture)
    {
        m_nTexHeight = 512;
        m_nTexWidth  = 512;
        TextureParam param;
        param.nWidth = 512;
        param.nHeight = 512;
        param.nMipmaps = 1;
//#ifdef CREATE_TEX_WITH_RGBA
//		param.pixelFormat = RPF_A8R8G8B8;
//#else
		param.pixelFormat = m_type.m_nFontHint == FONT_MONO ? RPF_ALPHA : RPF_A8R8G8B8/*RPF_R4G4B4A4*/;
//#endif
        m_pTexture = TEXTURE_MANAGER->CreateTexture(
			GetString("%s_%d_%d_%d_%d_%d", 
			m_type.m_strFaceName.c_str(), 
			m_type.m_bBold, 
			m_type.m_bItalic, 
			m_type.m_bUnderLine, 
            m_type.m_nFontHint, 
			m_type.m_nFontSize), param);

    }

    m_nTexturePixelBytes = EngineParamSetting::GetPixelFormatSize(m_pTexture->GetPixelFormat());
    if (m_pTexture && m_pFontTexBuffer == NULL)
    {
        m_pFontTexBuffer = AIR_NEW_ARRAY_T(u8, 512 * 512 * m_nTexturePixelBytes);
        memset(m_pFontTexBuffer, 0, 512 * 512 * m_nTexturePixelBytes);
    }
    return true;
}

bool CFTFont::CopyPixelToTextureBuffer(const u8* pPixel, int nPosX, int nPosY, int nOffsetX, int nOffsetY, int nBits /* = 1 */)
{
    if (nBits == 1)
    {
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

					int nOffsetTex = (nTexRow * m_pTexture->GetWidth() + nTexCol);
//#ifdef CREATE_TEX_WITH_RGBA
//                    nOffsetTex *= 4;
//#endif

                    //int nOffsetPixel = row * nOffsetX + col;
                    if  ( col + i >= nOffsetX )
                    {
                        //nOffsetPixel--;
                        break;
                    }

                    u8 nGray = (pPixel[nOffsetPixel] & ( 0x80 >> i));
                    if (nGray)
                    {
//#ifdef CREATE_TEX_WITH_RGBA
//                        *(u32 *)&m_pFontTexBuffer[nOffsetTex] = 0xffffffff;
//#else
						*(u8 *)&m_pFontTexBuffer[nOffsetTex] = 0xff;
//#endif
                    }
                    else
                    {
//#ifdef CREATE_TEX_WITH_RGBA
//                        *(u32 *)&m_pFontTexBuffer[nOffsetTex] = 0;
//#else
						*(u8 *)&m_pFontTexBuffer[nOffsetTex] = 0;
//#endif
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
		u8 nBytes = EngineParamSetting::GetPixelFormatSize(m_pTexture->GetTextureParam().pixelFormat); //nBits / 8;

        int size  = nOffsetX * nBytes;
        int sid, did;
        for( int i = nPosY; i < nPosY + nOffsetY; ++i )
        {
            did = ( i * m_pTexture->GetHeight() + nPosX ) * nBytes;
            sid = (i - nPosY) * size;

			memcpy( &m_pFontTexBuffer[did], &pPixel[sid], size );
        }
    }
    m_bTextureDirty = true;
    return true;
}

void CFTFont::PreRender()
{
    if (m_bTextureDirty)
    {
        if (m_pTexture)
        {
            const TextureParam& param = m_pTexture->GetTextureParam();
            u32 nBytes = EngineParamSetting::GetPixelFormatSize(param.pixelFormat);//param.pixelFormat == RPF_ALPHA ? 1 : 4;
            m_pTexture->CopyPixelBoxToTexture((const u8*)m_pFontTexBuffer, NULL, 0, 512 * 512 * nBytes, 512 * nBytes, 4);
			//m_pTexture->CopyPixelToTexture((const u8*)m_pFontTexBuffer, 0, 0, param.nWidth, param.nHeight);
        }
        m_bTextureDirty = false;
    }
}

bool CFTFont::GetCharGlyphInfo(wchar_t wCH, LPWORDINFO pWordInfo)
{
	FT_Error error = 0;
	FT_Int32 nLoadFlag = m_type.m_nFontHint == FONT_MONO ? FT_LOAD_FORCE_AUTOHINT | FT_LOAD_MONOCHROME : FT_LOAD_FORCE_AUTOHINT | FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL;
	error = FT_Load_Char(m_ftFace, wCH, /*FT_LOAD_DEFAULT*/nLoadFlag);

	if (error)
	{
		FT_Load_Char(m_ftFace, wCH, FT_LOAD_DEFAULT);
	}
	//加粗
	if (m_bBold)
	{
		//    FT_Outline_Embolden(&m_ftSlot->outline, 1 << 6);
	}
	//斜体
	if (m_bItailc)
	{
		//   FT_Outline_Transform(&m_ftSlot->outline, &m_matItailc);
	}

	//FT_Error error = FT_Render_Glyph(m_ftSlot, FT_RENDER_MODE_MONO);
	//FT_Error error = FT_Render_Glyph(m_ftSlot, FT_RENDER_MODE_NORMAL);

	if (error)
	{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
		MessageBox(NULL, "ft 没返回这个字", NULL, NULL);
#endif
		PrintDebugString( "freetype return no word.\n" );
		return NULL;
	}

	m_nOffsetX = m_nNextWordPosX; 
	m_nOffsetY = m_nNextWordPosY;

	u32 nWordWidth = m_ftSlot->bitmap.width;
	u32 nWordHeight = m_ftSlot->bitmap.rows;
	if( (m_nOffsetX + nWordWidth) > m_nTexWidth)
	{
		//应该写到下一行中
		m_nOffsetX  = 0;
		m_nOffsetY += m_nHeight; 

	}

	pWordInfo->wCode             = wCH;
	pWordInfo->nHeight           = m_nHeight;
	pWordInfo->nWidth            = m_ftSlot->advance.x >> 6;
	pWordInfo->nOffsetX          = m_ftSlot->metrics.horiBearingX >> 6;
	pWordInfo->nOffsetY          = 0;
	pWordInfo->nFontWidth        = nWordWidth;
	pWordInfo->nFontHeight       = nWordHeight;

	pWordInfo->rectUV.left       = (m_nOffsetX ) * 1.0f / m_nTexWidth;
	pWordInfo->rectUV.top        = (m_nOffsetY ) * 1.0f /m_nTexHeight;
	pWordInfo->rectUV.right      = (m_nOffsetX + nWordWidth ) * 1.0f / m_nTexWidth;
	pWordInfo->rectUV.bottom     = (m_nOffsetY + nWordHeight ) * 1.0f / m_nTexHeight;
	//计算下一字体要在纹理的位置
	m_nNextWordPosX = nWordWidth + m_nOffsetX;
	m_nNextWordPosY = m_nOffsetY;
	return true;
}

FTFaceMgr::FTFaceMgr() : m_ftLibrary(NULL)
{

}

FTFaceMgr::~FTFaceMgr()
{

}

bool FTFaceMgr::Init()
{
	if (m_ftLibrary == NULL)
	{
		FT_Error error;
		error = FT_Init_FreeType(&m_ftLibrary);
		if ( error )
		{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
			MessageBox(NULL,"init freetype error", 0, 0);
#endif
			PrintDebugString( "init freetype error!\n" );
			return false;
		}
	}

	return true;
}

void FTFaceMgr::Clear()
{
	if (m_ftLibrary)
	{
		FT_Done_FreeType(m_ftLibrary);
		m_ftLibrary = NULL;
	}

	m_mapFontFace.clear();
}

FT_Face FTFaceMgr::GetFontFace(const FixedString& strFontname)
{
	if (!Init())
	{
		return NULL;
	}

	AIRMap<FixedString, FT_Face>::iterator it = m_mapFontFace.find(strFontname);

	if (it != m_mapFontFace.end())
	{
		return it->second;
	}

	AIRString strFontFile = FILESYSTEM->GetFileFullPath(strFontname.AsCharPtr());
	FT_Face ftFace = NULL;
	FT_Error error = FT_New_Face(m_ftLibrary, strFontFile.c_str(), 0, &ftFace);
	if ( error )
	{
		if ( error == FT_Err_Unknown_File_Format )
		{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
			MessageBox(NULL,"可以打开和读这个文件，但不支持它的字体格式", 0, 0);
#endif
			PrintDebugString( "unknown freetype font file format!\n" );
		}
		else if (error)
		{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
			MessageBox(NULL,"new face error", 0, 0);
#endif
			PrintDebugString( "FT_New_Face() error!\n" );
		}

		return NULL;
	}

	m_mapFontFace.insert(std::make_pair(strFontname, ftFace));
	return ftFace;
}

