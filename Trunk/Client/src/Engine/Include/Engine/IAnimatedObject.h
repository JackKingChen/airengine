/********************************************************************
	created:	2010/10/28
	created:	28:10:2010   18:52
	filename: 	IAnimatedObject.h
	author:		liangairan
	
	purpose:	活动物体的抽象接口类，所有会“动”的物体都接近
*********************************************************************/

#pragma once 

#include "EngineConfig.h"

class EngineExport IAnimatedObject
{
public:
    IAnimatedObject() : m_bPaused(false) {}
    virtual ~IAnimatedObject() {}

    //暂停
    virtual void Pause()
    {
        m_bPaused = true;
    }

    //播放
    virtual void Play()
    {
        m_bPaused = false;
    }

    bool IsPaused() const
    {
        return m_bPaused;
    }

    //是否已经完成整个动画
    virtual bool IsCompleted() = 0;

    virtual void Start() = 0;

    //virtual void Stop() = 0;

    virtual void Update(float fTimeElapse) = 0;
protected:
    bool m_bPaused;
    
private:
};
