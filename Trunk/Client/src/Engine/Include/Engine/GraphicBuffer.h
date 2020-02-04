#pragma once 

#include "EngineConfig.h"

class EngineExport GraphicBuffer// : public EngineAllocator(GraphicBuffer)
{
	EngineAllocator(GraphicBuffer)
public:
    GraphicBuffer();
    virtual ~GraphicBuffer();

    virtual void* Lock() = 0;

    virtual void Unlock() = 0;
protected:
private:
};
