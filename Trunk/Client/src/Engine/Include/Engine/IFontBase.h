/********************************************************************
created:	2009/6/26
filename: 	IFontBase.h
author:		liangairan

purpose:	�������

*********************************************************************/

#pragma once 
#include "Common.h"
#include "AIRTypes.h"
#include "Rect.h"
#include "Color.h"
#include "EngineConfig.h"
//#include "WordRenderable.h"
#include "IGUIRenderer.h"

//�ο�HJJ��AURO
//lbh�޸� 2011-05-09

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
		m_bAsPound(true), m_strFaceName("����"), m_strFileName("simsun.ttc")
    {

    }

    s32         m_nFontSize;       // �����С(��)
    u32         m_nBorderSize;     //��ߵ����أ�Ĭ��Ϊ0
    bool        m_bBold;           // �Ƿ����
    bool        m_bItalic;         // �Ƿ�б��
    bool        m_bUnderLine;      // �Ƿ���»���
    //BOOL        m_bStrikeLine;     // �Ƿ��ɾ����
    //BYTE        m_nFontStyle;      // ������: eFS_NORMAL / eFS_SHADOW / eFS_FRAME
    c8        m_nFontHint;       // ����ƽ������: eFH_BMP / eFH_G2_BMP / eFH_G4_BMP / eFH_G8_BMP
	bool      m_bAsPound;  //size�Ƿ���pound
    //s32         m_nFontSizeID;     // �ֺ�ID
    AIRString m_strFaceName;     // ��������
    AIRString m_strFileName;     // �����ļ���
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

//һ������ĳһ�������������Ϣ
typedef struct tagWordInfo
{
    wchar_t wCode;
    Rectf   rectUV;  //��������
    int     nWidth;  //����Ŀ��  glpm.gmCellIncX
    int     nHeight; //����ĸ߶�  tm.tmHeight
    int     nFontWidth;   //�����������Сglpm.gmBlackBoxX
    int     nFontHeight;  //glpm.gmBlackBoxY
    int     nOffsetX;   //����ƫ�ƣ�һ����glpm.gmptGlyphOrigin.x
    int     nOffsetY;   //����ƫ�ƣ�һ����tm.tmAscent - glpm.gmptGlyphOrigin.y
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

    //���ĳ���ֵ���Ϣ
    virtual LPWORDINFO GetWordInfo(wchar_t wch) = 0;

    //�������������ļ�
    virtual bool SaveFontTextureToFile(const char* szFilename) = 0;

	virtual ITexture* GetTexture() = 0;
protected:
    FONTTYPE  m_type;
    u8  m_nTexturePixelBytes;   //���������ֽ�
private:
};
