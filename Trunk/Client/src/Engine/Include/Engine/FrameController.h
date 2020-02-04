/********************************************************************
	created:	2009/12/01
	created:	1:12:2009   10:35
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\FrameController.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	FrameController
	file ext:	h
	author:		liangairan
	
	purpose:	֡��������ÿ֡��Ⱦǰ��Ҫ����Ķ���
*********************************************************************/

#include "Engine.h"
#include <set>

class EngineExport CFrameController
{
public:
    CFrameController();
    virtual ~CFrameController();

    //ÿ֡���º������������û��Լ�ʵ��
    virtual void Update(float fTimeElapse) = 0;
protected:
private:
};

class EngineExport CFrameControllerManager
{
public:
    virtual ~CFrameControllerManager();

    void AddController(CFrameController* pControl);

    void DestroyController(CFrameController* pControl);

    void UpdateAllController(float fTimeElapse);

    static CFrameControllerManager* GetInstance() 
    {
        if (m_pInstance == NULL)
        {
            m_pInstance = new CFrameControllerManager;
        }
        return m_pInstance;
    }
protected:
private:
    CFrameControllerManager();

    static CFrameControllerManager* m_pInstance;

    typedef std::set<CFrameController*> CONTROLLER_SET;
    typedef CONTROLLER_SET::iterator CONTROLLER_SET_ITER;

    CONTROLLER_SET   m_lstControllers;
};
