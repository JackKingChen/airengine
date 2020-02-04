// ***************************************************************
//  文件: IGUIFormater.h
//  日期: 2010-2-24
//  作者: liangairan
//  说明: GUI文字格式化，抽象类，需要由用户自己去实现
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

    //格式化字符串
    //@wcText  被格式化的字符串
    //@lstElement 输出的UI元素
    virtual void FormatText(const wchar_t* wcText, std::list<CGUIElement*>& lstElement, IFontBase* pFont) = 0;

    //取得UI元素的格式化字符串
    virtual void GetElementCode(CGUIElement* pElement, wchar_t* pTextOut) = 0;
};
