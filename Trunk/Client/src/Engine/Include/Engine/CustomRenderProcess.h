#pragma once 
#include "EngineConfig.h"
#include "MemoryAlloter.h"
#include "MemoryAllocatorConfig.h"

class CustomRenderProcess;

typedef void (*ProcessUpdateCallBack)(CustomRenderProcess* pProcess);

class EngineExport CustomRenderProcess// : public EngineAllocator(CustomRenderProcess)
{
	EngineAllocator(CustomRenderProcess)
public:
	CustomRenderProcess() : m_bActive(false), m_pProcessUpdateFunc(0) {}
	virtual ~CustomRenderProcess() {}

	virtual void Render() = 0;

	virtual void Clear() {};

	bool IsActive() const
	{
		return m_bActive;
	}

	void SetActive(bool bActive)
	{
		m_bActive = bActive;
	}

	const AIRString& GetName() const
	{
		return m_strName;
	}

	void SetName(const char* szName)
	{
		m_strName = szName;
	}

    void SetProcessUpdateCallBack(ProcessUpdateCallBack pFunc)
    {
        m_pProcessUpdateFunc = pFunc;
    }
protected:

	AIRString m_strName;

	bool m_bActive;

    ProcessUpdateCallBack  m_pProcessUpdateFunc;
private:
};
