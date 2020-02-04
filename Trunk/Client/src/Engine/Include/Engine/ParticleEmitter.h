/********************************************************************
	created:	2009/11/29
	created:	29:11:2009   11:24
	filename: 	d:\My Codes\Engine\Engine\Include\ParticleEmitter.h
	file path:	d:\My Codes\Engine\Engine\Include
	file base:	ParticleEmitter
	file ext:	h
	author:		liangairan
	
	purpose:	���ӷ���������ͬ��״�ķ����������˲�ͬ��Ч��
                ��ͬ��״�ķ�����Ҫ�̳������
*********************************************************************/


#pragma once 
#include "EngineConfig.h"
#include "Engine.h"

#include "Particle.h"

class EngineExport CParticleEmitter : public CParticle
{
public:
    CParticleEmitter();
    virtual ~CParticleEmitter();

    //��һ�����ӽ���������emitter�ı���������
    //@pParticle ���������
    virtual void    InitParitcle(CParticle* pParticle) = 0;

    //ÿһ֡���������Ӹ���
    //@fElapse ��һ֡��ʱ���
    virtual int     GenParticleCount(float fElapse);

    //���÷�������λ��
    virtual void    SetPosition(const Vector3Df& vPosition);

    //��÷�����λ��
    virtual void    GetPosition(Vector3Df& vPosition);
    virtual Vector3Df& GetPosition( void );

    //���÷��䷽��
    virtual void    SetDirection(const Vector3Df& vDirection);

    //��ȡ���䷽��
    virtual const Vector3Df&   GetDirection() const;

    //���÷����ƫ�ƽǶ�
    virtual void    SetAngle(float fAngle);

    //��ȡ�����ƫ�ƽǶ�
    virtual float   GetAngle() const;

    //����ÿ�뷢�������
    virtual void    SetEmissionRate(float fRate);

    //���ÿ�뷢��������
    virtual float   GetEmissionRate() const;

    //������С�ķ����ٶ�
    virtual void    SetMinSpeed(float fSpeed);

    //�����С�����ٶ�
    virtual float   GetMinSpeed() const;

    //�������ӵ�����ٶ�
    virtual void    SetMaxSpeed(float fSpeed);

    //��ȡ���ķ����ٶ�
    virtual float   GetMaxSpeed() const;

    //���ô��ʱ��
    virtual void    SetTimeToLive(float fTime);

    //������С�Ĵ��ʱ��
    virtual void    SetMinTimeToLive(float fTime);

    //���������ʱ��
    virtual void    SetMaxTimeToLive(float fTime);

    //��ȡ���ʱ��
    virtual float   GetTimeToLive() const;

    //��ȡ��С�Ĵ��ʱ��
    virtual float   GetMinTimeToLive() const;

    //��ȡ���Ĵ��ʱ��
    virtual float   GetMaxTimeToLive() const;

    //�������Ӳ����ĳ�ʼ��ɫ
    virtual void    SetParticleColor(const CColorValue& color);

    //���ó�ʼ���ӵ���ʼ��ɫ
    virtual void    SetColorRangeStart(const CColorValue& color);

    //���ý�����ɫ
    virtual void    SetColorRangeEnd(const CColorValue& color);

    //������ʼʱ��
    virtual void    SetStartTime(float fTime);

    //�����ʼʱ��
    virtual float   GetStartTime() const;

    //���ô��ʱ��
    virtual void    SetDuration(float fTime);

    //�������ٴ��ʱ��
    virtual void    SetMinDuration(float fTime);

    //���������ʱ��
    virtual void    SetMaxDuration(float fTime);

    //��ȡ���ʱ��
    virtual float   GetDuration() const;

    //��ȡ���ٴ��ʱ��
    virtual float   GetMinDuration() const;

    //��ȡ�����ʱ��
    virtual float   GetMaxDuration() const;

    //�����ظ�ʱ��
    virtual void    SetRepeatDelay(float fTime);

    //������С�ظ�ʱ��
    virtual void    SetRepeatDelayMin(float fTime);

    //��������ظ�ʱ��
    virtual void    SetRepeatDelayMax(float fTime);

    //��ȡ�ظ�ʱ��
    virtual float   GetRepeatDelay() const;

    //��ȡ��С���ظ�ʱ��
    virtual float   GetRepeatDelayMin() const;

    //��ȡ�����ظ�ʱ��
    virtual float   GetRepeatDelayMax() const;

    //�Ƿ��Ǳ�����ķ�����  �ù�����Ϊ̫���������ڸð汾��ȥ�����Ժ���������µı�����ķ�����
    virtual bool    IsEmitted() const;

    //���÷���
    virtual void    SetEmitted(bool bEmitted);

    //�����Ƿ����
    virtual void    SetEnable(bool bEnable);

    //��ȡ�Ƿ����
    virtual bool    IsEnable();

    //��ȡ������ɫ
    virtual void    GetEmissionColor(CColorValue& colorValue);

    //��ȡ��ʼ���ӵ���ʼ��ɫ
    virtual void    GetColorRangeStart(CColorValue& colorValue);

    //��ȡ��ʼ���ӵĽ�����ɫ
    virtual void    GetColorRangeEnd(CColorValue& colorValue);

    //��xml����
    virtual bool    LoadFromXML(CMarkupSTL& xml);

    //���浽XML���
    virtual bool    SaveToXMLNode(CMarkupSTL& xml);

    virtual void    SetIsLimitInPlane( bool b );
    virtual bool    GetIsLimitInPlane( void );

    //virtual void    SetParticleSpacePos( const Vector3Df& pos );
    //virtual void    GetParticleSpacePos( Vector3Df& vPosition );
    //virtual Vector3Df GetParticleSpacePos( void );

    virtual void SetName(const AIRString& strName);

    virtual void SetEmittedEmitterName(const AIRString& strName);

    //������͵�����
    virtual const AIRString& GetTypeStr() const
    {
        return m_strType;
    }

    //����Լ�������
    virtual const AIRString& GetName() const
    {
        return m_strName;
    }

    //��ñ����䷢����������
    virtual const AIRString& GetEmittedEmitterName() const
    {
        return m_strEmittedEmitter;
    }

    virtual void Copy(CParticleEmitter* pEmitterClone);

    virtual void SetInitScaleMin( float f )
    {
        m_fInitScaleMin = f;
    }

    virtual void SetInitScaleMax( float f )
    {
        m_fInitScaleMax = f;
    }

    virtual float GetInitScaleMin( void )
    {
        return m_fInitScaleMin;
    }

    virtual float GetInitScaleMax( void )
    {
        return m_fInitScaleMax;
    }

    virtual float GetScale( void )
    {
        return AIR::RangeRandom( m_fInitScaleMin, m_fInitScaleMax );
    }

    // ���÷��������ӳ�������ӳٷ���ʱ��
    virtual void SetDelayEmitTime( float time )
    {
        m_fDelayEmitTime = time;
    }

    // ��÷��������ӳ�������ӳٷ���ʱ��
    virtual float GetDelayEmitTime( void )
    {
        return m_fDelayEmitTime;
    }

    virtual bool IsDelayEmitComplete()
    {
        return (m_fDelayEmitTime - (m_fTotalTimeToLive - m_fLifeTime)) <= 0;
    }

	virtual void SetVertAngle( float fVertAngle )
	{
		m_fVertAngle = fVertAngle;
		AIR::DegreeToRadius(m_fVertAngle);
	}

	virtual float GetVertAngle( void )
	{
		return m_fVertAngle;
	}

    /*
	virtual void SetAccumEmitCount( u32 AccumEmitCount )
	{
		m_nAccumEmitCount = AccumEmitCount;
	}

	virtual u32 GetAccumEmitCount( void )
	{
		return m_nAccumEmitCount;
	}
    */

	virtual void Reset( void );

protected:
    //�����·�������
    virtual void    GenEmissionDirection(Vector3Df& destVector);

    /** ���������ٶ�.
    @param destVector The vector to scale by a randomly generated scale between min and max speed.
    Assumed normalised already, and likely already oriented in the right direction.
    */
    virtual void    GenEmissionVelocity(Vector3Df& destVector);

    /** Internal utility method for generating a time-to-live for a particle. */
    //�������ӵ�����ʱ��
    virtual float   GenEmissionTTL(void);

    /** Internal utility method for generating a colour for a particle. */
    //�������ӵĳ�ʼ��ɫ
    virtual void    GenEmissionColour(CColorValue& destColour);

    //��ʼ���ظ�ʱ��
    virtual void    InitDurationRepeat();

    //��þ�������
    virtual AIRString GetType() = 0;

protected:
    Vector3Df       m_position;             //λ�ã����������ϵͳ��λ�ã��������ӿռ��е�λ��
    Vector3Df       m_direction;            //���䷽��
    Vector3Df       m_vUp;                  //��ֱ�ڷ��䷽�������
    float           m_fAngle;               //����Ƕ�
	float			m_fVertAngle;
    float           m_fEmissionRate;        //ÿ��Ҫ�������������
    float           m_fMinSpeed;            //���ӵ���С�����ٶ�
    float           m_fMaxSpeed;            //���ӵ�������ٶ�
    float           m_fMinTTL;              //���ӵ���С���ʱ��
    float           m_fMaxTTL;              //���ӵ������ʱ��
    float           m_fTimeStart;           //��ʼʱ��
    float           m_fDurationMin;         //��̳���ʱ��
    float           m_fDurationMax;         //������ʱ��
    float           m_fDurationRemain;      //ʣ��ʱ��
    float           m_fRepeatDelayMin;      //2�η������Сʱ����
    float           m_fRepeatDelayMax;      //2�η�������ʱ����
    float           m_fRepeatDelayRemain;   //Repeat delay left
    float           m_fRemainder;           //Fractions of particles wanted to be emitted last time
    CColorValue     m_clrRangeStart;        //��������ʱ�ĳ�ʼ��ɫֵ  һ�������ɫ��Χ
    CColorValue     m_clrRangeEnd;          //��������ʱ�Ľ�����ɫֵ
    bool            m_bEmitted;             //�Ƿ���������������
    bool            m_bEnable;              //�Ƿ����
    bool            m_bLimitInPlane;        //�Ƿ�����������ͬ���䷽��ֱ��ƽ����
    //Vector3Df       m_vParticleSpacePos;
    AIRString     m_strType;              //����������
    AIRString     m_strName;              //����������
    AIRString     m_strEmittedEmitter;    //������ķ���������
    float           m_fInitScaleMin;        //���ӷ���ʱ�ĳ�ʼ��С����Сֵ
    float           m_fInitScaleMax;        //���ӷ���ʱ�ĳ�ʼ��С�����ֵ

    float           m_fDelayEmitTime;       //�����ӳٷ����ʱ��

	//u32				m_nAccumEmitCount;		//�������������������ۻ��������,��������������������ۻ�������ֵ��ʱ��,�Ͳ��ٷ���������, Ϊ0��ʾ������
	//u32				m_nCurAccumCount;		//��ǰ���������ۻ�����������
};
