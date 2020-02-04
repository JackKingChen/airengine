#pragma once 

#include "IFontBase.h"
//#include "WordRenderable.h"

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32

enum EFONTGRAY
{
    FONT_BMP = GGO_BITMAP,
    FONT_BMP_GRAY2 = GGO_GRAY2_BITMAP,
    FONT_BMP_GRAY4 = GGO_GRAY4_BITMAP,
    FONT_BMP_GRAY8 = GGO_GRAY8_BITMAP,
};

class CGDICanvas
{
public:
    CGDICanvas();
    ~CGDICanvas();

    bool  SetFont(HFONT hFont);

    //---------------------------------------------------------
    // 名称:  TextOut
    // 功能:  通过GDI在位图里输出文字
    // 参数:  Text - 字符串指针; X,Y - 字符串输出的位置
    // 返回:  无
    //---------------------------------------------------------
    void	TextOut( const AIRString& Text, s32 X, s32 Y );

    HDC  GetDC() { return m_hDC; }

protected:
private:

    HDC   m_hDC;
    //HFONT		m_hFont;	// GDI字体
    HBRUSH		m_hBrush;	// 画刷
};


class CGDIFont : public IFontBase
{
public:
    CGDIFont();
    ~CGDIFont();

    virtual bool CreateW(const FONTTYPE& ft, const wchar_t* szWords);
    virtual bool Create(const FONTTYPE& ft, const char* szWords);

    virtual void Render(const WCHAR* szText, const AIR::Rectf& area, const CColorValue& color = CColorValue::White);
    virtual void Render(const char* szText, const AIR::Rectf& area, const CColorValue& color = CColorValue::White);

    virtual void Render(const Rectf& rectScreen, const LPWORDINFO pWordInfo, f32 fDepth, const CColorValue& color = CColorValue::White);
    virtual void Release();

    virtual void AddWords(const wchar_t* wszWords);

    virtual void AddWords(const char* szWords);

    //virtual CWordRenderable* AddWordToTexture(wchar_t wCH);

    //virtual CWordRenderable* AddWord(wchar_t wCH);
    virtual LPWORDINFO AddWord(wchar_t wCH);

    virtual u32 GetSpaceWidth() { return m_nSpaceWidth; }

    virtual u32 GetWordHeight() { return m_nFontHeight; }

    virtual void    GetNextWordPos(int& x, int& y) const
    {
        x = m_nNextWordPosX;
        y = m_nNextWordPosY;
    }

    //virtual CWordRenderable* FindWord(wchar_t wch);
    virtual LPWORDINFO FindWord(wchar_t wch);

    virtual LPWORDINFO GetWordInfo(wchar_t wch);

    virtual bool SaveFontTextureToFile(const char* szFilename);

	ITexture* GetTexture() { return m_pTexture; }
protected:
private:
	void PreRender();
	void UpdateTexture();
	bool CopyMonoFontToTextureBuffer(const u8* pPixel, int nPosX, int nPosY, int nWidth, int nHeight, int nGlyWidth, SColor color = SColor(0xffffffff));
	bool CopyPixelToTextureBuffer(const u8* pPixel, int nPosX, int nPosY, int nOffsetX, int nOffsetY, int nBits = 1);
    //CWordRenderable* FindWord(wchar_t wch);
    //void AddRenderWord(CWordRenderable* pRenderable);

    LPWORDINFO AddWordInfo(wchar_t wch);

    LPWORDINFO AddWordToTexture(wchar_t wCH);

    HFONT CreateGDIFont(const FONTTYPE& ft, int nHeight);

    //void SaveBitmapPixelToTexture(CWordRenderable* pWord, u8* pPixel, int nPosX, int nPosY, int nWidth, int nHeight, int nBmpBits = 1);
    void SaveBitmapPixelToTexture(LPWORDINFO pWord, u8* pPixel, int nPosX, int nPosY, int nWidth, int nHeight, int nBmpBits = 1, const SColor& color = SColor(0xffffffff));

    HFONT m_hFont;

    s32 m_nCharLineW;

    //FONTTYPE m_fontType;

    u32         m_nOffsetX;      // 从纹理的当前位置(X坐标为m_nOffsetX, Y坐标
    u32         m_nOffsetY;      // 为m_nOffsetY)开始添加文字像素数据
    u32         m_nFontBmpWidth;
    u32         m_nFontBmpHeight;

    u32         m_nGlyWidth;     // = glysize / gmBlockBoxY

    u32         m_nSpaceWidth;
    u32         m_nWordHeight;   //一个字的完整高度，一般是gmBlockBoxY
    u32         m_nFontHeight;

    int         m_nNextWordPosX; //下一个字的位置
    int         m_nNextWordPosY;

    bool        CreateFontTexture();

    ITexture* m_pTexture;

    //HASH_MAP<wchar_t, CWordRenderable*> m_mapWords;
    //typedef HASH_MAP<wchar_t, CWordRenderable*>::iterator WORD_ITER;

    HASH_MAP<wchar_t, LPWORDINFO> m_mapWordInfo;
    typedef HASH_MAP<wchar_t, LPWORDINFO>::iterator WORDINFO_ITER;

	u8* m_pFontTexBuffer;  //纹理内存占用
	bool  m_bTextureDirty;   //纹理内存是否被修改过
};

#endif
