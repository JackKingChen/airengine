#pragma once
#include "IFontBase.h"
#include "GUIElement.h"

const int TWINKLE_TIME = 1000;  //��˸ʱ��


class AIRGuiExport CGUIWord : public CGUIElement
{
public:
    CGUIWord(wchar_t wCode, IFontBase* pFont, SColor color = 0xffffffff, int nBorder = 0, SColor clrBorder = SColor(0xff000000), bool bTwinkle = false);
    virtual ~CGUIWord(void);

    void            SetScreenPos(int nX, int nY);

    //�������ռ���صĿ�ȣ��������ֵ�ʵ�ʿ��
    int             GetWordWidth() const;

    //�������ռ���صĸ߶�
    int             GetWordHeight() const;

    virtual void    Move(int dx, int dy);

    virtual void    MoveTo(int nPosX, int nPosY);

    wchar_t         GetCode();

    LPWORDINFO      GetWordInfo();

    //�������ľ���
    void            GetFontRect(Rectf& rect);

protected:
    virtual void    RenderSelf();

    virtual void    UpdateSelf();

private:
    LPWORDINFO  m_pWord;
    IFontBase*  m_pFont;
    bool        m_bTwinkle;   //��˸
    bool        m_bTwinkleDirect;
    u32       m_dwTwinkleStart;
    int         m_nBorder;    //�������
    SColor      m_clrBorder;  //�����ɫ

    Rectf       m_fontRect;  //ʵ����Ⱦ����
    //Rect      m_screenRect;  //��Ļ�ռ����
};
