#pragma once 

#include "EngineConfig.h"
#include "MemoryAllocatorConfig.h"

enum EventListenerType
{
    Event_FrameStart = 0,
    Event_FrameEnd,
    Event_SceneRender,
    Event_RenderTargetUpdate,
    Event_ResetTechnique,
    Event_EngineClear,
	Event_RenderShadowmap,
    Event_WindowResize,
    Event_Max,
};

//����Ǽ���EventListener�ģ�һ��������Singleton����Ϊ��֤����Engine�����Singleton�ĸ����ͷ�
class EngineExport EventListener// : public EngineAllocator(EventListener)
{
	EngineAllocator(EventListener)
public:
    EventListener();
    virtual ~EventListener();

    virtual void RegisterEvents() = 0;

    virtual void OnEvent(EventListenerType type, void* pParam1, void* pParam2, void* pParam3) = 0;
protected:
private:
};

