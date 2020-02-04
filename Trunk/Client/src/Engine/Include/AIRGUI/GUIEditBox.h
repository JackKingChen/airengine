// ***************************************************************
//  File: GUIEditBox.h
//  Date: 2010-2-12
//  Author: liangairan
//  Description: Edit控件
// ***************************************************************


#pragma once 

#include "GUIStaticText.h"
#include "GUIWord.h"

const int EDITBOX_MAX_SIZE = 1024;

class CGUIEditBox : public CGUIStaticText
{
public:
    // 编辑框状态
    enum eEditBoxState                       
    {
        eEDITBOX_INPUT      = 0x0000,    // 输入状态
        eEDITBOX_NOTINPUT   = 0x0001,    // 非输入状态
        eEDITBOX_SELECTED   = 0x0002,    // 选中文本块,等待下一步操作状态
        eEDITBOX_MAX        = 0xffff
    };

    CGUIEditBox();
    virtual ~CGUIEditBox();

    // move the edit box
    virtual void    Move(int dx, int dy);

    // update the edit box
    virtual void    Update();

    // 接收字符的输入
    virtual bool    OnChar(char ch);

    // get whether the edit box is on password mode
    virtual bool    IsPasswordMode() const;

    // 设置最大字符数量
    virtual void    SetMaxWordsNum(int nMaxWords);

    // set the font of the edit box
    virtual void    SetFont(const FONTTYPE& ft);

    virtual u32     GetMaxWordsNum()
    {
        return m_dwMaxWords;
    }

    virtual void    AddText(const wchar_t* wszText);

    virtual void    AddText(const char* szText);

protected:
    // create the edit box from xml
    virtual bool    CreateSelf(CGUIElement* pParent, CMarkupSTL& xml);

	virtual bool	SaveSelfToXML(CMarkupSTL& xml );

    // do status change call back function
    virtual void    OnStatusChange( void );

    // render edit box itself
    virtual void    RenderSelf( void );

    // 绘制光标
    virtual void    RenderCursor( void );

    // 设置光标位置
    virtual void    SetCursorPos( int x, int y );

    // update edit box content
    virtual void    UpdateString( void );
    
    // move cursor right
    virtual void    MoveCursorRight( void );

    // move cursor left
    virtual void    MoveCursorLeft( void );

    // 向前删除
    virtual void    ForwardDelete( void );

    // 向后删除
    virtual void    BackwardDelete( void );

    // set the text to edit box
    virtual void    SetWords( const wchar_t* wszText );

    // do key press call back function
    virtual bool    OnKeyPressSelf( int nKey );

    // reset the cursor position
    void            ResetCursorPos( void );

    // calculate the cursor position when the content of the edit box is empty
    virtual void    CacluateNoWordsCursorPos( void );

    // only move the words 
    virtual void    MoveWords( int dx, int dy );

    // set words color
    virtual void    SetWordsColor(const SColor color);

    virtual void    ClearChild();

    //move the words after erase some words
    virtual void    MoveWordsAfterChange(int dx);


private:
    typedef AIRList<CGUIWord*>::iterator GUIWORD_ITER;
    u8                    m_nCurState;                        //Editbox当前状态
    u8                    m_nLastState;                       //Editbox上一次的状态
    u32                     m_dwCurRenderTime;                  //当前的渲染时间
    u32                     m_dwLastRenderTime;                 //上一次的渲染时间in milion second
    u32                     m_dwMaxWords;                       //可以输入的最大字符数
    bool                    m_bRenderCursor;                    //是否渲染光标
    bool                    m_bPassword;                        //密码模式
    bool                    m_bMultiLine;                       //多行
    Rectf                   m_fontRect;                         //字体框
    int                     m_nCursorOffset;                    //第几个字
    GUIWORD_ITER            m_curIt;                            //当前光标的跌代器
    int                     m_nCursorPosX;                      //光标的X坐标
    int                     m_nCursorPosY;                      //光标的Y坐标
    wchar_t                 m_wcText[EDITBOX_MAX_SIZE];         //显示的字符
    wchar_t                 m_wcTemp[EDITBOX_MAX_SIZE];         //临时的字符串
    wchar_t                 m_wcPassword[EDITBOX_MAX_SIZE];     //密码明文
    char                    m_szText[EDITBOX_MAX_SIZE * 2 + 1]; //内容
    RECT                    m_rectScissor;                      //裁剪框
    Rectf                   m_rcCursor;                         //渲染光标的矩形
    AIRList<CGUIWord*>    m_lstWords;                          //文字容器
    
    //WAIRString  m_strText;
};
