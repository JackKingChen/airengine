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
    // ����:  TextOut
    // ����:  ͨ��GDI��λͼ���������
    // ����:  Text - �ַ���ָ��; X,Y - �ַ��������λ��
    // ����:  ��
    //---------------------------------------------------------
    void	TextOut( const AIRString& Text, s32 X, s32 Y );

    HDC  GetDC() { return m_hDC; }

protected:
private:

    HDC   m_hDC;
    //HFONT		m_hFont;	// GDI����
    HBRUSH		m_hBrush;	// ��ˢ
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

    u32         m_nOffsetX;      // ������ĵ�ǰλ��(X����Ϊm_nOffsetX, Y����
    u32         m_nOffsetY;      // Ϊm_nOffsetY)��ʼ���������������
    u32         m_nFontBmpWidth;
    u32         m_nFontBmpHeight;

    u32         m_nGlyWidth;     // = glysize / gmBlockBoxY

    u32         m_nSpaceWidth;
    u32         m_nWordHeight;   //һ���ֵ������߶ȣ�һ����gmBlockBoxY
    u32         m_nFontHeight;

    int         m_nNextWordPosX; //��һ���ֵ�λ��
    int         m_nNextWordPosY;

    bool        CreateFontTexture();

    ITexture* m_pTexture;

    //HASH_MAP<wchar_t, CWordRenderable*> m_mapWords;
    //typedef HASH_MAP<wchar_t, CWordRenderable*>::iterator WORD_ITER;

    HASH_MAP<wchar_t, LPWORDINFO> m_mapWordInfo;
    typedef HASH_MAP<wchar_t, LPWORDINFO>::iterator WORDINFO_ITER;

	u8* m_pFontTexBuffer;  //�����ڴ�ռ��
	bool  m_bTextureDirty;   //�����ڴ��Ƿ��޸Ĺ�
};

#endif
