#include "stdafx.h"
#include "FontManager.h"
#include "Util.h"
#include "GDIFont.h"
#include "FTFont.h"
#include "Engine.h"
#include "StringUtil.h"

CFontManager* CFontManager::m_pInstance = NULL;

CFontManager::CFontManager()
{

}

CFontManager::~CFontManager()
{
    //Clear();
}

IFontBase* CFontManager::CreateDefaultFonts()
{
    FONTTYPE defaultFont;

    IFontBase* pFont = GetFont(defaultFont);

    if (pFont != NULL)
    {
        return pFont;
    }
    
    LPAPPCONFIG pConfig = ENGINE_INST->GetConfig();
    AIRString strFont = pConfig->m_szFont;
#if AIR_TARGET_PLATFORM != AIR_PLATFORM_IOS
    CStringUtil::ToLowerCase(strFont);
#endif
    
//#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
//    if (strFont == "gdi")
//    {
//        pFont = new CGDIFont();
//    }
//    else
//#endif
        pFont = new CFTFont();
	pFont->CreateW(defaultFont, CStringUtil::s2ws(" 执Lag我1023456789+你他测试yghc没有，！").c_str());
    //pFont->CreateW(defaultFont, L" 执Lag我1023456789+你他测试yghc没有，！OKABCDE.FG什么也为大跃进一定要换行啊大哥斯达黎加*—夺取试谈东恭喜@#发财政预算方案()（）帧");
    pFont->SaveFontTextureToFile("C:/font_宋体.png");
    AddFont(pFont);

    return pFont;
}

void CFontManager::SaveFontTextureToFile()
{
	FONTTYPE defaultFont;
	IFontBase* pFont = GetFont(defaultFont);
	if (pFont != NULL)
	{
		AIRString file = "C:/font.png";
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID
		file = "/sdcard/font.png";
#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS
        file = ENGINE_INST->GetExternalResourcePath() + "/font.png";
#endif
		pFont->SaveFontTextureToFile( file.c_str() );
	}
}

IFontBase* CFontManager::CreateFontW(const FONTTYPE& ft, const wchar_t* wszText)
{
    IFontBase* pFont = GetFont(ft);

    if (pFont == NULL)
    {
        //创建GDI字体
        //pFont = /*new*/AURO_NEW_VERBOSE CGDIFont();
        LPAPPCONFIG pConfig = ENGINE_INST->GetConfig();
        AIRString strFont = pConfig->m_szFont;
        CStringUtil::ToLowerCase(strFont);
//#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
//        if (strFont == "gdi")
//        {
//            pFont = new CGDIFont();
//        }
//        else
//#endif
            pFont = new CFTFont();
        pFont->CreateW(ft, wszText);
        AddFont(pFont);
    }
    else
        pFont->AddWords(wszText);

    return pFont;
}

IFontBase* CFontManager::CreateFontA(const FONTTYPE& ft, const char* szText /* = NULL */)
{
    IFontBase* pFont = GetFont(ft);

    if (pFont == NULL)
    {
        //创建GDI字体
        //pFont = /*new*/AURO_NEW_VERBOSE CGDIFont();
        pFont = new USEFONT();
        pFont->Create(ft, szText);
        AddFont(pFont);
    }
    else
        pFont->AddWords(szText);

    return pFont;
    
}

IFontBase* CFontManager::GetFont(const FONTTYPE& ft)
{
    FONT_ITER itEnd = m_lstFonts.end();
    for (FONT_ITER it = m_lstFonts.begin(); it != itEnd; ++it)
    {
        IFontBase* pFont = *it;
        if (pFont->GetFontType() == ft)
        {
            return pFont;
        }
    }


    return NULL;
}

void CFontManager::AddFont(IFontBase* pFont)
{
    m_lstFonts.push_back(pFont);
    //m_lstFonts.insert(std::make_pair(pFont->GetFontType(), pFont));
}

void CFontManager::ReleaseFont(IFontBase* pFont)
{
    pFont->Release();
    FONT_ITER it = std::find(m_lstFonts.begin(), m_lstFonts.end(), pFont);//m_lstFonts.find(pFont->GetFontType());
    if (it != m_lstFonts.end())
    {
        m_lstFonts.erase(it);
        SafeDelete(pFont);
    }
}

void CFontManager::Clear()
{
    for (FONT_ITER it = m_lstFonts.begin(); it != m_lstFonts.end(); it++)
    {
        IFontBase* pFont = *it;
        pFont->Release();
        SafeDelete(pFont);
    }
    m_lstFonts.clear();

	FTFaceMgr::GetInstance()->Clear();
}
