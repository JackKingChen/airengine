// ***************************************************************
//  File: GUIEditBox.h
//  Date: 2010-2-12
//  Author: liangairan
//  Description: Edit�ؼ�
// ***************************************************************


#pragma once 

#include "GUIStaticText.h"
#include "GUIWord.h"

const int EDITBOX_MAX_SIZE = 1024;

class CGUIEditBox : public CGUIStaticText
{
public:
    // �༭��״̬
    enum eEditBoxState                       
    {
        eEDITBOX_INPUT      = 0x0000,    // ����״̬
        eEDITBOX_NOTINPUT   = 0x0001,    // ������״̬
        eEDITBOX_SELECTED   = 0x0002,    // ѡ���ı���,�ȴ���һ������״̬
        eEDITBOX_MAX        = 0xffff
    };

    CGUIEditBox();
    virtual ~CGUIEditBox();

    // move the edit box
    virtual void    Move(int dx, int dy);

    // update the edit box
    virtual void    Update();

    // �����ַ�������
    virtual bool    OnChar(char ch);

    // get whether the edit box is on password mode
    virtual bool    IsPasswordMode() const;

    // ��������ַ�����
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

    // ���ƹ��
    virtual void    RenderCursor( void );

    // ���ù��λ��
    virtual void    SetCursorPos( int x, int y );

    // update edit box content
    virtual void    UpdateString( void );
    
    // move cursor right
    virtual void    MoveCursorRight( void );

    // move cursor left
    virtual void    MoveCursorLeft( void );

    // ��ǰɾ��
    virtual void    ForwardDelete( void );

    // ���ɾ��
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
    u8                    m_nCurState;                        //Editbox��ǰ״̬
    u8                    m_nLastState;                       //Editbox��һ�ε�״̬
    u32                     m_dwCurRenderTime;                  //��ǰ����Ⱦʱ��
    u32                     m_dwLastRenderTime;                 //��һ�ε���Ⱦʱ��in milion second
    u32                     m_dwMaxWords;                       //�������������ַ���
    bool                    m_bRenderCursor;                    //�Ƿ���Ⱦ���
    bool                    m_bPassword;                        //����ģʽ
    bool                    m_bMultiLine;                       //����
    Rectf                   m_fontRect;                         //�����
    int                     m_nCursorOffset;                    //�ڼ�����
    GUIWORD_ITER            m_curIt;                            //��ǰ���ĵ�����
    int                     m_nCursorPosX;                      //����X����
    int                     m_nCursorPosY;                      //����Y����
    wchar_t                 m_wcText[EDITBOX_MAX_SIZE];         //��ʾ���ַ�
    wchar_t                 m_wcTemp[EDITBOX_MAX_SIZE];         //��ʱ���ַ���
    wchar_t                 m_wcPassword[EDITBOX_MAX_SIZE];     //��������
    char                    m_szText[EDITBOX_MAX_SIZE * 2 + 1]; //����
    RECT                    m_rectScissor;                      //�ü���
    Rectf                   m_rcCursor;                         //��Ⱦ���ľ���
    AIRList<CGUIWord*>    m_lstWords;                          //��������
    
    //WAIRString  m_strText;
};
