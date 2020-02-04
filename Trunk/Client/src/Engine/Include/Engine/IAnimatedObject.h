/********************************************************************
	created:	2010/10/28
	created:	28:10:2010   18:52
	filename: 	IAnimatedObject.h
	author:		liangairan
	
	purpose:	�����ĳ���ӿ��࣬���лᡰ���������嶼�ӽ�
*********************************************************************/

#pragma once 

#include "EngineConfig.h"

class EngineExport IAnimatedObject
{
public:
    IAnimatedObject() : m_bPaused(false) {}
    virtual ~IAnimatedObject() {}

    //��ͣ
    virtual void Pause()
    {
        m_bPaused = true;
    }

    //����
    virtual void Play()
    {
        m_bPaused = false;
    }

    bool IsPaused() const
    {
        return m_bPaused;
    }

    //�Ƿ��Ѿ������������
    virtual bool IsCompleted() = 0;

    virtual void Start() = 0;

    //virtual void Stop() = 0;

    virtual void Update(float fTimeElapse) = 0;
protected:
    bool m_bPaused;
    
private:
};
