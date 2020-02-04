#include "stdafx.h"
#include "EventListener.h"
#include "Engine.h"

EventListener::EventListener()
{

}

EventListener::~EventListener()
{
    ENGINE_INST->RemoveEventListener(this);
}
