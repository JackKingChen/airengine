// ***************************************************************
//  File: GUIBuilderMgr.h
//  Date: 2010-2-12
//  Author: liangairan
//  Description: 控件builder管理器，如果有第三方控件，可以利用该管理器
//               注册自己的控件
// ***************************************************************


#pragma once
#include "GUIBuilder.h"
#include "Singleton.h"


// 控件定制管理器
class AIRGuiExport CGUIBuilderMgr : public Singleton<CGUIBuilderMgr>
{

    BE_SINGLETON(CGUIBuilderMgr);
public:
    ~CGUIBuilderMgr();

    // 注册控件的定制类
    bool                    Register(const char* szBuilder, CGUIBuilder* pBuilder);

    // 获取控件的定制类
    CGUIBuilder*            GetBuilder(const AIRString& strName);

    // 清空
    void                    CleanUp();



    //static void KillSelf();
protected:

    typedef  AIRMap<AIRString, CGUIBuilder*>  BUILDER_MAP;
    typedef  AIRMap<AIRString, CGUIBuilder*>::iterator BUILDER_ITER;

    BUILDER_MAP             m_mapBuilder;
private:
    //static CGUIBuilderMgr*  m_pInstance;
};






//=============================================================
//! [添加代码] - [HJJ] - [2009-4-22] - 开始 :
//! [描述]: 
#define	UIBUILDERMGR	CGUIBuilderMgr::GetInstance()
//! [添加代码] - [HJJ] - [2009-4-22] - 结束 !
//=============================================================