// ***************************************************************
//  �ļ�: IGUIFormater.h
//  ����: 2010-2-24
//  ����: liangairan
//  ˵��: GUI���ָ�ʽ���������࣬��Ҫ���û��Լ�ȥʵ��
// ***************************************************************


#pragma once

#include "AIRGUI.h"

#include "GUIElement.h"
#include "IFontBase.h"

class AIRGuiExport IGUIFormater
{
public:
    IGUIFormater(void);
    virtual ~IGUIFormater(void);

    virtual void FormatText(const char* szText, std::list<CGUIElement*>& lstElement, IFontBase* pFont)
    {
        size_t nLen = strlen(szText) + 2;
        wchar_t* wcsText = new wchar_t[nLen];
        MByteToWChar(szText, wcsText, (u32)nLen);
        FormatText(wcsText, lstElement, pFont);
        delete [] wcsText;
    }

    //��ʽ���ַ���
    //@wcText  ����ʽ�����ַ���
    //@lstElement �����UIԪ��
    virtual void FormatText(const wchar_t* wcText, std::list<CGUIElement*>& lstElement, IFontBase* pFont) = 0;

    //ȡ��UIԪ�صĸ�ʽ���ַ���
    virtual void GetElementCode(CGUIElement* pElement, wchar_t* pTextOut) = 0;
};
