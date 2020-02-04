#pragma once
#include "ft2build.h"
#include "IFontBase.h"
#include FT_FREETYPE_H
/************************************************************************
简陋的freetype类，并没有实现，粗体，斜体，等
************************************************************************/

enum FontHintType
{
	FONT_MONO,
	FONT_BITMAP,
};

class CFTFont: public IFontBase
{
public:
    CFTFont();
    virtual ~CFTFont();

    virtual bool CreateW(const FONTTYPE& ft, const wchar_t* szWords);
    virtual bool Create(const FONTTYPE& ft, const char* szWords);
   
    virtual void Render(const wchar_t* szText, const AIR::Rectf& area, const CColorValue& color = CColorValue::White);
    virtual void Render(const char* szText, const AIR::Rectf& area, const CColorValue& color = CColorValue::White);
    
    virtual void Render(const Rectf& rectScreen, const LPWORDINFO pWordInfo, f32 fDepth, const CColorValue& color = CColorValue::White);
    virtual void Release();

    virtual void AddWords(const wchar_t* wszWords);

    virtual void AddWords(const char* szWords);
    virtual LPWORDINFO AddWord(wchar_t wCH);
   

    virtual u32 GetSpaceWidth() { return m_nSpaceWidth; }

    virtual u32 GetWordHeight() { return m_nFontHeight; }

    virtual void    GetNextWordPos(int& x, int& y) const
    {
        x = m_nNextWordPosX;
        y = m_nNextWordPosY;
    }

    virtual LPWORDINFO FindWord(wchar_t wch);

    virtual LPWORDINFO GetWordInfo(wchar_t wch);

    virtual bool SaveFontTextureToFile(const char* szFilename);

	ITexture* GetTexture() { return m_pTexture; }
protected:
private:
    void PreRender();
    bool CopyPixelToTextureBuffer(const u8* pPixel, int nPosX, int nPosY, int nOffsetX, int nOffsetY, int nBits = 1);
    
    LPWORDINFO AddWordToTexture(wchar_t wCH);
    bool        CreateFontTexture();

    bool        CreateFTFont(const FONTTYPE& ft);

	bool GetCharGlyphInfo(wchar_t wCH, LPWORDINFO pWordInfo);

    //FT_Library    m_ftLibrary;
    FT_Face       m_ftFace;
    FT_GlyphSlot  m_ftSlot;
    FT_Matrix     m_matItailc;
    
    bool          m_bBold;          //是否加粗
    bool          m_bItailc;        //是否是斜体
    bool          m_bUnderline;     //是否有下滑线
       
    unsigned int    m_nOffsetX;           // 从纹理的当前位置(X坐标为m_nOffsetX, Y坐标
    unsigned int    m_nOffsetY;           // 为m_nOffsetY)开始添加文字像素数据
    
    unsigned int    m_nNextWordPosX;      //下一个字的位置
    unsigned int    m_nNextWordPosY;

    int             m_nAcender;           //Origin上的最大距离
    int             m_nDescender;         //Origin下的最小距离（是负值）
    int             m_nHeight;            // |m_nAcender| + |m_nDescender|
    int             m_nLineSpace;         //行与行之间的空格，如果是竖排文字，怎是列于列
    int             m_nOrigin;            //文字的线基准
    
    u32             m_nTexHeight;         //纹理高度
    u32             m_nTexWidth;          //纹理宽度
    
    u32             m_nSpaceWidth;         //没用到，为了编译能通过
    u32             m_nFontHeight;
    ITexture* m_pTexture;

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    HASH_MAP<wchar_t, LPWORDINFO> m_mapWordInfo;
    typedef HASH_MAP<wchar_t, LPWORDINFO>::iterator WORDINFO_ITER;
#else
	AIRMap<wchar_t, LPWORDINFO> m_mapWordInfo;
	typedef AIRMap<wchar_t, LPWORDINFO>::iterator WORDINFO_ITER;
#endif
   
    u8* m_pFontTexBuffer;  //纹理内存占用
    bool  m_bTextureDirty;   //纹理内存是否被修改过

	s32 m_nBaseLine;         //基线到文字上边界的距离，单位是像素
	s32 m_nUnderLinePos;     //下划线到基线的偏 移
	s32 m_nUnderLineThick;   //下划线的宽度
};


class FTFaceMgr : public Singleton<FTFaceMgr>
{
	BE_SINGLETON(FTFaceMgr);
public:
	~FTFaceMgr();

	void Clear();

	bool Init();

	FT_Face GetFontFace(const FixedString& strFontname);

	FT_Library GetFTLibrary()
	{
		return m_ftLibrary;
	}
private:
	AIRMap<FixedString, FT_Face> m_mapFontFace;
	FT_Library m_ftLibrary;
};
