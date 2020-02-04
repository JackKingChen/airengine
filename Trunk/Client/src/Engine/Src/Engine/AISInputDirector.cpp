#include "stdafx.h"
#include "AISInputDirector.h"

GUILayerEventProcessor::GUILayerEventProcessor(const char* szName)
: AIS::EventProcessor(szName)
{
	SetReturnImmediateWhenHandleSuccess( true );
}
GUILayerEventProcessor::~GUILayerEventProcessor()
{

}

SceneLayerEventProcessor::SceneLayerEventProcessor(const char* szName)
: AIS::EventProcessor(szName)
{

}
SceneLayerEventProcessor::~SceneLayerEventProcessor()
{

}


//--------------------------------------------------------------------------------------------------

AISInputDirector::AISInputDirector()
: mpUIEvtProc(NULL)
, mpSceneEvtProc(NULL)
{

}

AISInputDirector::~AISInputDirector()
{

}

bool AISInputDirector::IsKeyPressed(AIS::KeyCode key) const
{
	if (AIS::AISInputSystem::GetInstance()->GetKeyboardDevice())
		return AIS::AISInputSystem::GetInstance()->GetKeyboardDevice()->isKeyDown( key );
	else
		return false;
}

bool AISInputDirector::IsKeyModiferPressed(AIS::Keyboard::Modifier key) const
{
	if (AIS::AISInputSystem::GetInstance()->GetKeyboardDevice())
		return AIS::AISInputSystem::GetInstance()->GetKeyboardDevice()->isModifierDown( key );
	else
		return false;
}

bool AISInputDirector::IsMouseButtonPressed(AIS::MouseButtonID mbtn)
{
	if (AIS::AISInputSystem::GetInstance()->GetMouseDevice())
		return AIS::AISInputSystem::GetInstance()->GetMouseDevice()->isButtonDown( mbtn );
	else
		return false;
}

// Director methods
bool AISInputDirector::initialise(unsigned long hWnd)
{
	AIS::AISInputSystem::CreateInstance();
	AIS::InputEventListener* pEvtListener = new AIS::InputEventListener("InputEventListener");
	mpUIEvtProc = new GUILayerEventProcessor("GUILayerEventProcessor");
	mpSceneEvtProc = new SceneLayerEventProcessor("SceneLayerEventProcessor");
	pEvtListener->AddChildEventProcessor( (AIS::EventProcessor*)mpUIEvtProc );
	pEvtListener->AddChildEventProcessor( (AIS::EventProcessor*)mpSceneEvtProc );
    
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID || AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS
	return AIS::AISInputSystem::GetInstance()->SetupInput( hWnd, pEvtListener, false );
#else
    return AIS::AISInputSystem::GetInstance()->SetupInput( hWnd, pEvtListener, true );
#endif
    
}

void AISInputDirector::shutdown(void)
{
	AIS::AISInputSystem::GetInstance()->ShutdownInput();
	AIS::AISInputSystem::DestroyInstance();
}

void AISInputDirector::update()
{
	AIS::AISInputSystem::GetInstance()->UpdateInput();
}