// ***************************************************************
//  LODInfo   version:  1.0   ·  date: 01/04/2009
//  -------------------------------------------------------------
//  LOD类
//  -------------------------------------------------------------
//  Copyright (C) 2009 - All Rights Reserved
// ***************************************************************
// 作者 梁蔼然
// ***************************************************************
#pragma once
#include "Common.h"
#include "Util.h"
#include "IIndexBuffer.h"
#include "Engine.h"
#include "IRenderer.h"

class EngineExport CLODInfo //: public EngineAllocator(CLODInfo)
{
	EngineAllocator(CLODInfo)
public:
    CLODInfo() : m_nLODLevel(1), m_nLeftLODDelta(0), m_nTopLODDelta(0), m_nRightLODDelta(0), m_nBottomLODDelta(0), m_pIndexBuffer(NULL)
    {
    }

    CLODInfo(int nLODLevel, int nLeftDelta, int nTopDelta, int nRightDelta, int nBottomDelta):m_nLODLevel(nLODLevel),
        m_nLeftLODDelta(nLeftDelta), m_nTopLODDelta(nTopDelta), m_nRightLODDelta(nRightDelta), m_nBottomLODDelta(nBottomDelta), m_pIndexBuffer(NULL)
    {
    }
    ~CLODInfo(){};

    bool CreateIndexBuffer(u32 nIndexCount)
    {
        if (m_pIndexBuffer != NULL)
        {
            m_pIndexBuffer->Release();
            
            SafeDelete<IIndexBuffer*>(m_pIndexBuffer);
               
            
        }
        IRenderer* pRenderer = ENGINE_INST->GetRenderer();
        
        m_pIndexBuffer = pRenderer->CreateIndexBuffer(nIndexCount);
        
        return (m_pIndexBuffer != NULL);
    }

    void CleanUp()
    {
        if (m_pIndexBuffer)
        {
            m_pIndexBuffer->Release();
            SafeDelete<IIndexBuffer*>(m_pIndexBuffer);
        
        }
    };

    IIndexBuffer* GetIndexBuffer() { return m_pIndexBuffer; }

    void SetIndexBuffer(IIndexBuffer* pIndexBuffer)
    {
        m_pIndexBuffer = pIndexBuffer;
    }

    bool operator ==(const CLODInfo& info)
    {
        return (m_nLODLevel == info.m_nLODLevel && m_nTopLODDelta == info.m_nTopLODDelta && m_nLeftLODDelta == info.m_nLeftLODDelta
            && m_nRightLODDelta == info.m_nRightLODDelta && m_nBottomLODDelta == info.m_nBottomLODDelta);
    };

    void SetLODLevel(int nLODLevel)
    {
        m_nLODLevel = nLODLevel;
    }

    void SetMaxLOD(int nMaxLOD)
    {
        m_nMaxLOD = nMaxLOD;
    }

    void ComputeLOD(float fCameraDis, float fBlockWidth, float fDeltaH)
    {
        if (fDeltaH < fBlockWidth / 100)    //暂时屏蔽掉，因为编辑器的关系liangairan 2010-05-05
        {
            m_nLODLevel = m_nMaxLOD;
            return;
        }

        m_nLODLevel = (int)(fCameraDis / fBlockWidth) + 1;
        m_nLODLevel = m_nLODLevel > m_nMaxLOD ? m_nMaxLOD : m_nLODLevel;
    }

public:
    int  m_nLODLevel;
    //int  m_nRealCell;
    int  m_nLeftLODDelta;
    int  m_nTopLODDelta;
    int  m_nRightLODDelta;
    int  m_nBottomLODDelta;
    static int  m_nMaxLOD;

private:
    IIndexBuffer* m_pIndexBuffer;
};

//int CLODInfo::m_nMaxLOD = 3;
