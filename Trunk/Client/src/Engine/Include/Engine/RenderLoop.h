#pragma once
#include "EngineConfig.h"
#include "MemoryAlloter.h"
#include "MemoryAllocatorConfig.h"

class CustomRenderProcess;

class EngineExport RenderLoop// : public EngineAllocator(RenderLoop)
{
	 EngineAllocator(RenderLoop)
public:
	RenderLoop() {};
	virtual ~RenderLoop() {};

	virtual void Render() = 0;

	const AIRString& GetName() const
	{
		return m_strName;
	}
protected:
	AIRString m_strName;

	AIRVector<CustomRenderProcess*> m_vtCustomRenderObjs;
private:
};

//前向渲染的类
class EngineExport RLForward : public RenderLoop
{
public:
	RLForward();
	virtual ~RLForward();

	virtual void Render();
protected:
private:
};
