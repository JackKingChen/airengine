#pragma once

//#include "Singleton.h"
#include "GUIProgressBar.h"

class CGUIFactory// : public Singleton<CGUIFactory>
{
protected:
    
private:
    // ���󴴽�����ָ��
    typedef CGUIElement*                            (*tCreator)();
    // ���󴴽���������
    typedef std::map< AIRString, tCreator >       CREATOR_MAP;

    
public:
    CGUIFactory(void);
    virtual ~CGUIFactory(void);

    void                    Register(const char* szType, tCreator aCreator);

    virtual  CGUIElement*   CreateElement(const char* szTypename, const char* szElementName);

protected:
    int m_nElementCount;
    CREATOR_MAP  m_mapCreator;
};
