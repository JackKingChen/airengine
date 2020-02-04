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
    // ����:  GetTemplate
    // ����:  �������ƻ��GUIԪ��ģ��
    // ����:  Name - ģ��GUIԪ�ص�����
    // ����:  ģ��GUIԪ��ָ��
    //---------------------------------------------------------
    virtual CGUIElement* GetTemplate( const char* Name );

    //---------------------------------------------------------
    // ����:  AddTemplate
    // ����:  ����һ��ģ��GUIԪ��
    // ����:  pTemplate - ģ��GUIԪ��ָ��
    // ����:  ����ɹ�?
    //---------------------------------------------------------
    virtual bool AddTemplate( CGUIElement* pTemplate );

    //---------------------------------------------------------
    // ����:  ReleaseTemplate
    // ����:  �ͷ�ģ��GUIԪ��
    // ����:  ��
    // ����:  ��
    //---------------------------------------------------------
    virtual void ReleaseTemplate( void );
private:
    typedef list<CGUIElement*>       GUITEMPLATE_LIST;
    typedef GUITEMPLATE_LIST::iterator GUITEMPLATE_LIST_ITER;

    GUITEMPLATE_LIST    m_lstTemplate;
};
