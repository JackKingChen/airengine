#pragma once 

#include "Common.h"
#include "EngineConfig.h"
#include "RTTI.h"

class EngineExport IReference
{
	DECLARE_ROOT_RTTI;
public:
    IReference() : m_nRef(1)
        //, m_bLoad(false) 
    {

    } 
    virtual ~IReference() {}

    s32 AddRef() const { return ++m_nRef; }

    s32 DropRef() const { return --m_nRef; }

    virtual s32 Release() = 0;

    virtual int GetRef() { return m_nRef; }

    //virtual const AIRString& GetName() const { return m_strName; }

    //virtual void SetName(const AIRString& strName) { m_strName = strName; }

protected:
    //AIRString m_strName;

    //bool m_bLoad;    //是否已经创建

private:
    mutable int m_nRef;
};

