#pragma once

#include "Common.h"
#include "EngineConfig.h"

class EngineExport CShaderParameter
{
    
public:
    enum eType
    {
        ETYPE_FLOATARRAY,
        ETYPE_FLOAT,
        ETYPE_INTARRAY,
        ETYPE_INT,
    };
    CShaderParameter(eType type, void* pValue, u32 nSize) : m_type(type), m_pValue(pValue), m_nSize(nSize) {}
    virtual ~CShaderParameter() {}

    eType  m_type;
    void*  m_pValue;
    u32    m_nSize;
protected:
private:
};
