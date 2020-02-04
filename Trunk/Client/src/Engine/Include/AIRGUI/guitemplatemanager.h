#pragma once
//#include "Common.h"
//#include "Singleton.h"
#include "GUIElement.h"

class AIRGuiExport CGUITemplateManager// : public Singleton<CGUITemplateManager>
{
public:
    CGUITemplateManager();
    ~CGUITemplateManager();
    //---------------------------------------------------------
    // 名称:  GetTemplate
    // 功能:  根据名称获得GUI元素模板
    // 参数:  Name - 模板GUI元素的名称
    // 返回:  模板GUI元素指针
    //---------------------------------------------------------
    virtual CGUIElement* GetTemplate( const char* Name );

    //---------------------------------------------------------
    // 名称:  AddTemplate
    // 功能:  插入一个模板GUI元素
    // 参数:  pTemplate - 模板GUI元素指针
    // 返回:  插入成功?
    //---------------------------------------------------------
    virtual bool AddTemplate( CGUIElement* pTemplate );

    //---------------------------------------------------------
    // 名称:  ReleaseTemplate
    // 功能:  释放模板GUI元素
    // 参数:  无
    // 返回:  无
    //---------------------------------------------------------
    virtual void ReleaseTemplate( void );
private:
    typedef list<CGUIElement*>       GUITEMPLATE_LIST;
    typedef GUITEMPLATE_LIST::iterator GUITEMPLATE_LIST_ITER;

    GUITEMPLATE_LIST    m_lstTemplate;
};
