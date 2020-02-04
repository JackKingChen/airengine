#pragma once
#include "ft2build.h"
#include "IFontBase.h"
#include FT_FREETYPE_H
/************************************************************************
��ª��freetype�࣬��û��ʵ�֣����壬б�壬��
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
    
    bool          m_bBold;          //�Ƿ�Ӵ�
    bool          m_bItailc;        //�Ƿ���б��
    bool          m_bUnderline;     //�Ƿ����»���
       
    unsigned int    m_nOffsetX;           // ������ĵ�ǰλ��(X����Ϊm_nOffsetX, Y����
    unsigned int    m_nOffsetY;           // Ϊm_nOffsetY)��ʼ���������������
    
    unsigned int    m_nNextWordPosX;      //��һ���ֵ�λ��
    unsigned int    m_nNextWordPosY;

    int             m_nAcender;           //Origin�ϵ�������
    int             m_nDescender;         //Origin�µ���С���루�Ǹ�ֵ��
    int             m_nHeight;            // |m_nAcender| + |m_nDescender|
    int             m_nLineSpace;         //������֮��Ŀո�������������֣�����������
    int             m_nOrigin;            //���ֵ��߻�׼
    
    u32             m_nTexHeight;         //����߶�
    u32             m_nTexWidth;          //������
    
    u32             m_nSpaceWidth;         //û�õ���Ϊ�˱�����ͨ��
    u32             m_nFontHeight;
    ITexture* m_pTexture;

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    HASH_MAP<wchar_t, LPWORDINFO> m_mapWordInfo;
    typedef HASH_MAP<wchar_t, LPWORDINFO>::iterator WORDINFO_ITER;
#else
	AIRMap<wchar_t, LPWORDINFO> m_mapWordInfo;
	typedef AIRMap<wchar_t, LPWORDINFO>::iterator WORDINFO_ITER;
#endif
   
    u8* m_pFontTexBuffer;  //�����ڴ�ռ��
    bool  m_bTextureDirty;   //�����ڴ��Ƿ��޸Ĺ�

	s32 m_nBaseLine;         //���ߵ������ϱ߽�ľ��룬��λ������
	s32 m_nUnderLinePos;     //�»��ߵ����ߵ�ƫ ��
	s32 m_nUnderLineThick;   //�»��ߵĿ��
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
