#pragma once

#include "EngineConfig.h"
#include "AISInputSystem.h"
#include "Singleton.h"

class EngineExport GUILayerEventProcessor : public AIS::EventProcessor
{
public:
	GUILayerEventProcessor(const char* szName);
	virtual ~GUILayerEventProcessor();
};

class EngineExport SceneLayerEventProcessor : public AIS::EventProcessor
{
public:
	SceneLayerEventProcessor(const char* szName);
	virtual ~SceneLayerEventProcessor();
};

class EngineExport AISInputDirector : public Singleton<AISInputDirector>
{
	BE_SINGLETON(AISInputDirector);

public:
	~AISInputDirector();

	inline GUILayerEventProcessor* GetGUILayerEventProcessor() 
	{
		return mpUIEvtProc;
	}
	inline SceneLayerEventProcessor* GetSceneLayerEventProcessor()
	{
		return mpSceneEvtProc;
	}

	bool IsKeyPressed(AIS::KeyCode key) const;

	bool IsKeyModiferPressed(AIS::Keyboard::Modifier key) const;

	bool IsMouseButtonPressed(AIS::MouseButtonID mbtn);

	// Director methods
	virtual bool initialise(unsigned long hWnd);
	virtual void shutdown(void);
	virtual void update(void);

protected:

	GUILayerEventProcessor* mpUIEvtProc;
	SceneLayerEventProcessor* mpSceneEvtProc;
};