/********************************************************************
	created:	2009/12/01
	created:	1:12:2009   10:35
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\FrameController.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	FrameController
	file ext:	h
	author:		liangairan
	
	purpose:	帧控制器，每帧渲染前需要处理的东西
*********************************************************************/

#include "Engine.h"
#include <set>

class EngineExport CFrameController
{
public:
    CFrameController();
    virtual ~CFrameController();

    //每帧更新函数，必须由用户自己实现
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
