#include "stdafx.h"
#include "FrameController.h"


CFrameController::CFrameController()
{

}

CFrameController::~CFrameController()
{

}

CFrameControllerManager* CFrameControllerManager::m_pInstance = NULL;

CFrameControllerManager::CFrameControllerManager()
{

}

CFrameControllerManager::~CFrameControllerManager()
{

}

void CFrameControllerManager::AddController(CFrameController* pControl)
{
    m_lstControllers.insert(pControl);
}

void CFrameControllerManager::DestroyController(CFrameController* pControl)
{
    CONTROLLER_SET_ITER it = m_lstControllers.find(pControl);
    if (it != m_lstControllers.end())
    {
        m_lstControllers.erase(it);
    }
}

void CFrameControllerManager::UpdateAllController(float fTimeElapse)
{
    for (CONTROLLER_SET_ITER it = m_lstControllers.begin(); it != m_lstControllers.end(); it++)
    {
        (*it)->Update(fTimeElapse);
    }
}
