#pragma once

#include "IReference.h"



class EngineExport IEffect : public IReference
{
public:
    IEffect() : m_nID(0), m_nPassNum(0) {}
    virtual ~IEffect() {}

    virtual bool  Create(const char* szFilename) = 0;

    virtual int   Release() = 0;

    virtual bool  SetValue(const char* szValueName, const void *pValue, int nSize) = 0;

    virtual bool  SetBool(const char* szValueName, bool bValue) = 0;

    virtual bool  SetVector4(const char* szValueName, float* pValue) = 0;

    virtual bool  SetMatrix(const char* szValueName, float* pValue) = 0;

    virtual bool  SetFloat(const char* szValueName, float fValue) = 0;

    virtual bool  SetTechnique(const char* szName) = 0;

    virtual bool  Begin(unsigned int* pNumPass) = 0;

    virtual bool  End() = 0;

    virtual bool  BeginPass(int nStart) = 0;

    virtual bool  EndPass() = 0;

    virtual bool  BeginParameterBlock() = 0;

    virtual bool  EndParameterBlock() = 0;

    virtual void  Clear() = 0;

    virtual s32   GetID() { return m_nID; }

    virtual void  SetID(s32 nID) { m_nID = nID; }

    virtual const AIRString& GetName() const { return m_strName; }

    s32 GetPassCount() { return m_nPassNum; }

    //virtual void  SetParameters(const std::map<AIRString, void*>& parameters) = 0;
protected:
    s32 m_nID;
    s32 m_nPassNum;
    AIRString m_strName;
private:
};
