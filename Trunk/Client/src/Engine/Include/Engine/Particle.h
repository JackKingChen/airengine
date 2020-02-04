/********************************************************************
	created:	2009/11/28
	created:	28:11:2009   17:09
	filename: 	d:\My Codes\Engine\Engine\Include\Particle.h
	file path:	d:\My Codes\Engine\Engine\Include
	file base:	Particle
	file ext:	h
	author:		liangairan
	
	purpose:	粒子类
*********************************************************************/


#pragma once 
#include "EngineConfig.h"
#include "Engine.h"
#include "ColorValue.h"
#include "Color.h"
#include "SceneNode.h"

class EngineExport CParticle //: public EngineAllocator(CParticle)
{
	EngineAllocator(CParticle)
public:
	enum PARTICLE_TYPE
	{
		PARTICLE_NORMAL,
		PARTICLE_EMITTER,
		PARTICLE_RENDERABLEOBJECT,
	};
public:
    CParticle();
    virtual ~CParticle();

    //创建相关的渲染物
    void CreateRenderable(const char* szName, const char* szFilename);

    //计算renderable的变换
    void CaculateRenderableTransform();

    Renderable* GetRenderable()
    {
        return m_pRenderable;
    }

    void SetRenderable(Renderable* pRenderable);
protected:
public:
    Vector3Df       m_position;             //位置
    Vector3Df       m_direction;            //运动方向
    float           m_fRotationAngle;       //旋转角度，绕法向量旋转，单位弧度
    float           m_fRotationSpeed;       //旋转速度，单位弧度
    float           m_fLifeTime;            //生命剩余时间
    float           m_fTotalTimeToLive;     //生命值 
    CColorValue     m_color;                //粒子当前颜色
    CColorValue     m_colorDelta;           //每一帧的颜色变化
    float           m_fWidth;               //当前粒子宽
    float           m_fHeight;              //当前粒子高
    PARTICLE_TYPE   m_particleType;         //粒子类型
    bool            m_bTrail;               //是否有拖尾
    u32             m_nSegmentIndex;        //索引段
	float			m_fAxisRotateSpeed;		//记录粒子绕轴旋转的角速度(单位:度/秒)
	float			m_fInitWidth;			//粒子初始宽度
	float			m_fInitHeight;			//粒子初始高度
    //BillBoardChain::ChainSegment*   m_pChainSeg;   //拖尾的段

private:
    Renderable*  m_pRenderable;   //粒子附带的渲染物
    CSceneNode*   m_pSceneNode;    //渲染物的场景结点，该结点不由场景创建
};
