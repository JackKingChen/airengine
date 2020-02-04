/********************************************************************
	created:	2010/07/16
	created:	16:7:2010   10:35
	filename: 	AIRKeyframeAnimationRes.h
	author:		liangairan
	
	purpose:	关键帧动画
*********************************************************************/

#pragma once 

#include "EngineResource.h"

#include "VertexAnimation.h"

class EngineExport AIRKeyframeAnimationRes : public Resource
{
public:
    AIRKeyframeAnimationRes();

    virtual ~AIRKeyframeAnimationRes();

    virtual s32 Release();

    void Clear();

    //从文件创建
    bool CreateFromFile(const char* szFilename);

    //从二进制文件创建
    bool CreateFromBinaryFile(const char* szFilename);

    void CreateAnimationStates(AIRMap<AIRString, CAIRAnimationState*>& mapAnimations);

    virtual bool Cache();

    VertexAnimation* GetVertexAnimation()
    {
        if (!m_mapAnimations.empty())
        {
            return m_mapAnimations.begin()->second;
        }
        return NULL;
    }

    const AIRString&  GetName() const { return m_strName; }
protected:
private:

    typedef AIRMap<AIRString, VertexAnimation*> VertexAnimationList;
    typedef VertexAnimationList::iterator VertexAnimationListIter;

    VertexAnimationList   m_mapAnimations;
	AIRString m_strName;
};
