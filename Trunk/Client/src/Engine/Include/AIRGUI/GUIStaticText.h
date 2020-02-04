/********************************************************************
	created:	2009/11/20
	created:	20:11:2009   18:23
	filename: 	d:\My Codes\Engine\AIRGUI\GUIStaticText.h
	file path:	d:\My Codes\Engine\AIRGUI
	file base:	GUIStaticText
	file ext:	h
	author:		liangairan
	
	purpose:	文本控件，支持字体点选，表情渲染
*********************************************************************/
#pragma once
#include "Common.h"
#include "GUIElement.h"
//#include "GUIWord.h"
#include "AIRGUI.h"
#include "IFontBase.h"

class CGUIRenderLine;

enum eTextAlign
{
    eAlign_LeftTop,
    eAlign_Middle,
    eAlign_Left,
    eAlign_MiddleTop,
    eAlign_RightTop,
    eAlign_RightBottom,
    eAlign_LeftBottom,
};

class AIRGuiExport CGUIStaticText : public CGUIElement
{
public:
    typedef AIRVector <CGUIRenderLine*>::iterator GUILINE_ITER;
public:
    CGUIStaticText();
    virtual ~CGUIStaticText();

    //virtual void            CleanUp(void);

    virtual void            Update(void);

    virtual const wchar_t*  GetTextW(void);

    virtual const char*     GetText(void);

    virtual void            SetText(const char* szText);

    virtual void            SetText(const wchar_t* wszText);

    virtual void            AddText(const char* szText);

    virtual void            AddText(const wchar_t* wszText);

    virtual void            Move(int dx, int dy);

    virtual void            MoveRealRect(int dx, int dy);

    virtual void            MoveRealRectTo(int nPosX, int nPosY);

    virtual void            SetAlign(eTextAlign align);

    //删除某一行
    //@bCombine 是否合并，如果是，则把所有line上移或下移
    virtual void            DeleteLine(size_t nLineIndex, bool bCombine = false);

    //添加新一行字体
    virtual void            AddLineText(const char* szText);

    virtual void            AddLineText(const wchar_t* wcText);

    //返回文本的行数
    int                     GetLinesNum(void) const;

    virtual void            SetFont( IFontBase* pFont ); 

    virtual void            SetFont(const FONTTYPE& ft);

    virtual void            SetFontColor(const SColor color);

    virtual void            SetScissorRect( const RECT& rect );

    virtual void            GetScissorRect(RECT& rect);

    virtual void            SetEnableScissor( bool b );

    virtual eTextAlign      GetTextAlign()
    {
        return m_align;
    }

    virtual IFontBase*      GetFont()
    {
        return m_pFont;
    }

    virtual bool            IsAutoScroll()
    {
        return m_bAutoScroll;
    }

    virtual bool            IsScissorEnable()
    {
        return m_bEnableScissor;
    }

    virtual int             GetRowSpace()
    {
        return m_nRowSpace;
    }

    //virtual void  SetTextAlign(eTextAlign align);

    virtual void  SetRowSpace(unsigned int nRowSpace);

    virtual void  SetAutoScroll(bool bAutoScroll);

    virtual SColor GetFontColor()
    {
        return m_clrFont;
    }

	virtual void            SetIsDrawFrame( bool b );

    virtual CGUIRenderLine* GetRenderLine(size_t nIndex);

    virtual bool IsFormated()
    {
        return m_bFormated;
    }

    virtual void SetFormated(bool bFormated);

    virtual bool OnLButtonDown(int x, int y);

    //auto size by all the words
    //超过该最大宽度，就换行显示
    virtual void AutoSizeByWords(const char* szText, int nMaxWidth = 0);

    virtual void AutoSizeByWords(const wchar_t* wcText, int nMaxWidth = 0);

    //virtual void AddChild(CGUIElement* pChild);

    void SetSaveLine(bool bSaveLine);

    bool IsSaveLine() 
    {
        return m_bSaveLine;
    }

    //设置某行是否被选择
    void SetLineSelected(size_t nLineIndex, bool bSelected);

	bool					GetIsEmpty( void )
	{
		return m_vtRenderLines.empty();
	}

	virtual void	AddAutoSizeLine(const char* szText, int nMaxWidth = 0 );

	virtual void	CalWholeRealSize( Rectf& rc );

    //清空text
    virtual void    ClearText();

    int GetTextLeftOffset() const
    {
        return m_nLeftOffset;
    }

    int GetTextRightOffset() const
    {
        return m_nRightOffset;
    }

    void SetTextLeftOffset(int nLeftOffset);

    //设置右偏移
    void SetTextRightOffset(int nRightOffset);

    //获得字描边宽度
    int  GetWordFrame() const
    {
        return m_nWordFrame;
    }

    SColor GetWordFrameColor() const
    {
        return m_clrWordFrame;
    }

    //设置字体的描边宽度
    void SetWordFrameWidth(int nWidth);

    //设置字体的描边颜色
    void SetWordFrameColor(const SColor& color);

protected:
    virtual void            RenderSelf(void);
    virtual bool            CreateSelf(CGUIElement* pParent, CMarkupSTL& xml/*TiXmlElement* pElement*/);

	virtual bool			SaveSelfToXML(CMarkupSTL& xml );
	virtual bool			SaveSelfBaseToXML(CMarkupSTL& xml );
    virtual bool            SaveTextBlockToXML(CMarkupSTL& xml, IFontBase* pFont);
	
    //virtual bool            SaveChildToXML(CMarkupSTL& xml);

    virtual void            RegisterToScript(void);

    virtual void            SetWords(const wchar_t* wszText);
    virtual void            AddWords(const wchar_t* wszText);

    virtual void            SetWordsColor(const SColor color);

    virtual void            ClearChild();

    virtual void            CaculateTextAlignPos();
    //计算所有line加起来的高度
    virtual int             CaculateLineTotalHeight();
private:
    void                    CreateGUIWords();

protected:
    
    //计算字的位置
    //virtual void CaculateWordsPosition();
    //virtual void LineReturn();
    //virtual void OnStatusChange();
    //virtual void RenderSelf();

    WAIRString                    m_strText;
    AIRString                     m_strTextA;
    IFontBase*                      m_pFont;
    eTextAlign                      m_align;
    AIRVector <CGUIRenderLine*>   m_vtRenderLines;
    int                             m_nRowSpace;        //每行的间距
    bool                            m_bAutoScroll;      //自动滚屏
    RECT                            m_rectScissor;      //裁剪区域
    bool                            m_bEnableScissor;
    SColor                          m_clrFont;          //字体的颜色
    bool                            m_bFormated;        //是否已经格式化完毕
    bool                            m_bSaveLine;        //是否要把每行的元素都保存
    int                             m_nLeftOffset;      //左偏移
    int                             m_nRightOffset;     //文本右偏移
    int                             m_nWordFrame;       //字体描边宽度
    SColor                          m_clrWordFrame;     //描边颜色
};
