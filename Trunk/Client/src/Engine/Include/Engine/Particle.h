/********************************************************************
	created:	2009/11/28
	created:	28:11:2009   17:09
	filename: 	d:\My Codes\Engine\Engine\Include\Particle.h
	file path:	d:\My Codes\Engine\Engine\Include
	file base:	Particle
	file ext:	h
	author:		liangairan
	
	purpose:	������
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

    //������ص���Ⱦ��
    void CreateRenderable(const char* szName, const char* szFilename);

    //����renderable�ı任
    void CaculateRenderableTransform();

    Renderable* GetRenderable()
    {
        return m_pRenderable;
    }

    void SetRenderable(Renderable* pRenderable);
protected:
public:
    Vector3Df       m_position;             //λ��
    Vector3Df       m_direction;            //�˶�����
    float           m_fRotationAngle;       //��ת�Ƕȣ��Ʒ�������ת����λ����
    float           m_fRotationSpeed;       //��ת�ٶȣ���λ����
    float           m_fLifeTime;            //����ʣ��ʱ��
    float           m_fTotalTimeToLive;     //����ֵ 
    CColorValue     m_color;                //���ӵ�ǰ��ɫ
    CColorValue     m_colorDelta;           //ÿһ֡����ɫ�仯
    float           m_fWidth;               //��ǰ���ӿ�
    float           m_fHeight;              //��ǰ���Ӹ�
    PARTICLE_TYPE   m_particleType;         //��������
    bool            m_bTrail;               //�Ƿ�����β
    u32             m_nSegmentIndex;        //������
	float			m_fAxisRotateSpeed;		//��¼����������ת�Ľ��ٶ�(��λ:��/��)
	float			m_fInitWidth;			//���ӳ�ʼ���
	float			m_fInitHeight;			//���ӳ�ʼ�߶�
    //BillBoardChain::ChainSegment*   m_pChainSeg;   //��β�Ķ�

private:
    Renderable*  m_pRenderable;   //���Ӹ�������Ⱦ��
    CSceneNode*   m_pSceneNode;    //��Ⱦ��ĳ�����㣬�ý�㲻�ɳ�������
};
