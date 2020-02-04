// ***************************************************************
//  文件: FontManager.h
//  日期: 2009-9-12
//  作者: liangairan
//  说明: 字体管理器
// ***************************************************************
#pragma once 

#include "IFontBase.h"
#include "Singleton.h"
#define USEFONT CFTFont

#ifndef USEFONT
#define USEFONT CGDIFont
#endif

class EngineExport CFontManager : public Singleton<CFontManager>
{
    BE_SINGLETON(CFontManager);
public: 
    ~CFontManager();

    //Create default fonts
    IFontBase* CreateDefaultFonts();

    //get the fonts by the font type
    IFontBase* GetFont(const FONTTYPE& ft);

    IFontBase* CreateFontW(const FONTTYPE& ft, const wchar_t* wszText);

    IFontBase* CreateFontA(const FONTTYPE& ft, const char* szText);

    //bool LoadFonts();
    void Clear();

	void SaveFontTextureToFile();

protected:

    void    ReleaseFont(IFontBase* pFont);

private:

    void    AddFont(IFontBase* pFont);

    static CFontManager* m_pInstance;

    //bool   IsFontExist(const FONTTYPE& ft);

    AIRList<IFontBase*>   m_lstFonts;
    typedef AIRList<IFontBase*>::iterator FONT_ITER;
};

#define FONTMGR CFontManager::GetInstance() 