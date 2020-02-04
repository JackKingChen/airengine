/********************************************************************
created:	2009/6/26
filename: 	IFontBase.h
author:		liangairan

purpose:	字体基类

*********************************************************************/

#pragma once 
#include "Common.h"
#include "AIRTypes.h"
#include "Rect.h"
#include "Color.h"
#include "EngineConfig.h"
//#include "WordRenderable.h"
#include "IGUIRenderer.h"

//参考HJJ的AURO
//lbh修改 2011-05-09

enum EFONTSOURCE
{
    FONT_GDI,
    FONT_FREETYPE2,
};

#if AIR_TARGET_PLATFORM != AIR_PLATFORM_WIN32
#define GGO_BITMAP 1
#endif


typedef struct tagFontType
{
    tagFontType() : m_nFontSize(9), m_nBorderSize(0), m_bBold(false), m_bItalic(false), m_bUnderLine(false), m_nFontHint(GGO_BITMAP),
		m_bAsPound(true), m_strFaceName("宋体"), m_strFileName("simsun.ttc")
    {

    }

    s32         m_nFontSize;       // 字体大小(磅)
    u32         m_nBorderSize;     //描边的像素，默认为0
    bool        m_bBold;           // 是否粗体
    bool        m_bItalic;         // 是否斜体
    bool        m_bUnderLine;      // 是否加下划线
    //BOOL        m_bStrikeLine;     // 是否加删除线
    //BYTE        m_nFontStyle;      // 字体风格: eFS_NORMAL / eFS_SHADOW / eFS_FRAME
    c8        m_nFontHint;       // 字体平滑参数: eFH_BMP / eFH_G2_BMP / eFH_G4_BMP / eFH_G8_BMP
	bool      m_bAsPound;  //size是否是pound
    //s32         m_nFontSizeID;     // 字号ID
    AIRString m_strFaceName;     // 字体名称
    AIRString m_strFileName;     // 字体文件名
    bool operator == (const tagFontType& ft) const
    {
        return (m_nFontSize == ft.m_nFontSize
            && m_nBorderSize == ft.m_nBorderSize
            && m_bBold == ft.m_bBold
            && m_bItalic == ft.m_bItalic
            && m_bUnderLine == ft.m_bUnderLine
            && m_nFontHint == ft.m_nFontHint
            && m_strFaceName == ft.m_strFaceName
            && m_strFileName == ft.m_strFileName
			&& m_bAsPound == ft.m_bAsPound
            );
    }

    tagFontType& operator = (const tagFontType& ft)
    {
        m_nFontSize = ft.m_nFontSize;
        m_nBorderSize = ft.m_nBorderSize;
        m_bBold = ft.m_bBold;
        m_bItalic = ft.m_bItalic;
        m_bUnderLine = ft.m_bUnderLine;
        m_nFontHint = ft.m_nFontHint;
        m_strFaceName = ft.m_strFaceName;
        m_strFileName = ft.m_strFileName;
		m_bAsPound = ft.m_bAsPound;
        return *this;
    }

}FONTTYPE, *LPFONTTYPE;

//一个字在某一种字体里面的信息
typedef struct tagWordInfo
{
    wchar_t wCode;
    Rectf   rectUV;  //纹理坐标
    int     nWidth;  //字体的宽度  glpm.gmCellIncX
    int     nHeight; //字体的高度  tm.tmHeight
    int     nFontWidth;   //字体的真正大小glpm.gmBlackBoxX
    int     nFontHeight;  //glpm.gmBlackBoxY
    int     nOffsetX;   //向右偏移，一般是glpm.gmptGlyphOrigin.x
    int     nOffsetY;   //向下偏移，一般是tm.tmAscent - glpm.gmptGlyphOrigin.y
}WORDINFO, *LPWORDINFO;

class EngineExport IFontBase// : public EngineAllocator(IFontBase)
{
	EngineAllocator(IFontBase)
public:
    IFontBase() : m_nTexturePixelBytes(0) {}
    virtual ~IFontBase() {}

    virtual bool CreateW(const FONTTYPE& ft, const wchar_t* szWords) = 0; // { return true; };
    virtual bool Create(const FONTTYPE& ft, const char* szWords) = 0;

    virtual void Render(const wchar_t* szText, const AIR::Rectf& area, const CColorValue& color = CColorValue::White) = 0;
    virtual void Render(const char* szText, const AIR::Rectf& area, const CColorValue& color = CColorValue::White) = 0;

    virtual void Render(const Rectf& rectScreen, const LPWORDINFO pWordInfo, f32 fDepth, const CColorValue& color = CColorValue::White) = 0;

    virtual void Release() = 0;

    virtual void AddWords(const wchar_t* wszWords) = 0;
    virtual void AddWords(const char* szWords) = 0;

    virtual const FONTTYPE& GetFontType() const { return m_type; }

	virtual const FONTTYPE* GetFontTypePtr() const { return &m_type; }

    //virtual CWordRenderable* AddWord(wchar_t wCH) = 0;
    virtual LPWORDINFO AddWord(wchar_t wCH) = 0;

    virtual u32 GetSpaceWidth() = 0;

    virtual u32 GetWordHeight() = 0;

    virtual void    GetNextWordPos(int& x, int& y) const = 0;

    virtual LPWORDINFO FindWord(wchar_t wCH) = 0;

    //获得某个字的信息
    virtual LPWORDINFO GetWordInfo(wchar_t wch) = 0;

    //保存字体纹理到文件
    virtual bool SaveFontTextureToFile(const char* szFilename) = 0;

	virtual ITexture* GetTexture() = 0;
protected:
    FONTTYPE  m_type;
    u8  m_nTexturePixelBytes;   //纹理像素字节
private:
};
