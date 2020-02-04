/********************************************************************
	created:	2009/11/29
	created:	29:11:2009   11:24
	filename: 	d:\My Codes\Engine\Engine\Include\ParticleEmitter.h
	file path:	d:\My Codes\Engine\Engine\Include
	file base:	ParticleEmitter
	file ext:	h
	author:		liangairan
	
	purpose:	粒子发射器，不同形状的发射器决定了不同的效果
                不同形状的发射器要继承这个类
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

    //传一个粒子进来，根据emitter改变它的属性
    //@pParticle 输出的粒子
    virtual void    InitParitcle(CParticle* pParticle) = 0;

    //每一帧产生的粒子个数
    //@fElapse 上一帧的时间差
    virtual int     GenParticleCount(float fElapse);

    //设置发射器的位置
    virtual void    SetPosition(const Vector3Df& vPosition);

    //获得发射器位置
    virtual void    GetPosition(Vector3Df& vPosition);
    virtual Vector3Df& GetPosition( void );

    //设置发射方向
    virtual void    SetDirection(const Vector3Df& vDirection);

    //获取发射方向
    virtual const Vector3Df&   GetDirection() const;

    //设置发射的偏移角度
    virtual void    SetAngle(float fAngle);

    //获取发射的偏移角度
    virtual float   GetAngle() const;

    //设置每秒发射的数量
    virtual void    SetEmissionRate(float fRate);

    //获得每秒发射粒子数
    virtual float   GetEmissionRate() const;

    //设置最小的发射速度
    virtual void    SetMinSpeed(float fSpeed);

    //获得最小发射速度
    virtual float   GetMinSpeed() const;

    //设置粒子的最大速度
    virtual void    SetMaxSpeed(float fSpeed);

    //获取最大的发射速度
    virtual float   GetMaxSpeed() const;

    //设置存活时间
    virtual void    SetTimeToLive(float fTime);

    //设置最小的存活时间
    virtual void    SetMinTimeToLive(float fTime);

    //设置最大存活时间
    virtual void    SetMaxTimeToLive(float fTime);

    //获取存活时间
    virtual float   GetTimeToLive() const;

    //获取最小的存活时间
    virtual float   GetMinTimeToLive() const;

    //获取最大的存活时间
    virtual float   GetMaxTimeToLive() const;

    //设置粒子产生的初始颜色
    virtual void    SetParticleColor(const CColorValue& color);

    //设置初始粒子的起始颜色
    virtual void    SetColorRangeStart(const CColorValue& color);

    //设置结束颜色
    virtual void    SetColorRangeEnd(const CColorValue& color);

    //设置起始时间
    virtual void    SetStartTime(float fTime);

    //获得起始时间
    virtual float   GetStartTime() const;

    //设置存活时间
    virtual void    SetDuration(float fTime);

    //设置最少存活时间
    virtual void    SetMinDuration(float fTime);

    //设置最大存活时间
    virtual void    SetMaxDuration(float fTime);

    //获取存活时间
    virtual float   GetDuration() const;

    //获取最少存活时间
    virtual float   GetMinDuration() const;

    //获取最大存活时间
    virtual float   GetMaxDuration() const;

    //设置重复时间
    virtual void    SetRepeatDelay(float fTime);

    //设置最小重复时间
    virtual void    SetRepeatDelayMin(float fTime);

    //设置最大重复时间
    virtual void    SetRepeatDelayMax(float fTime);

    //获取重复时间
    virtual float   GetRepeatDelay() const;

    //获取最小的重复时间
    virtual float   GetRepeatDelayMin() const;

    //获取最大的重复时间
    virtual float   GetRepeatDelayMax() const;

    //是否是被发射的发射器  该功能因为太繁，所以在该版本中去掉，以后另外设计新的被发射的发射器
    virtual bool    IsEmitted() const;

    //设置发射
    virtual void    SetEmitted(bool bEmitted);

    //设置是否可用
    virtual void    SetEnable(bool bEnable);

    //获取是否可用
    virtual bool    IsEnable();

    //获取发射颜色
    virtual void    GetEmissionColor(CColorValue& colorValue);

    //获取初始粒子的起始颜色
    virtual void    GetColorRangeStart(CColorValue& colorValue);

    //获取初始粒子的结束颜色
    virtual void    GetColorRangeEnd(CColorValue& colorValue);

    //从xml加载
    virtual bool    LoadFromXML(CMarkupSTL& xml);

    //保存到XML结点
    virtual bool    SaveToXMLNode(CMarkupSTL& xml);

    virtual void    SetIsLimitInPlane( bool b );
    virtual bool    GetIsLimitInPlane( void );

    //virtual void    SetParticleSpacePos( const Vector3Df& pos );
    //virtual void    GetParticleSpacePos( Vector3Df& vPosition );
    //virtual Vector3Df GetParticleSpacePos( void );

    virtual void SetName(const AIRString& strName);

    virtual void SetEmittedEmitterName(const AIRString& strName);

    //获得类型的名字
    virtual const AIRString& GetTypeStr() const
    {
        return m_strType;
    }

    //获得自己的名字
    virtual const AIRString& GetName() const
    {
        return m_strName;
    }

    //获得被发射发射器的名字
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

    // 设置发射器粒子出生后的延迟发射时间
    virtual void SetDelayEmitTime( float time )
    {
        m_fDelayEmitTime = time;
    }

    // 获得发射器粒子出生后的延迟发射时间
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
    //产生新发生方向
    virtual void    GenEmissionDirection(Vector3Df& destVector);

    /** 产生发射速度.
    @param destVector The vector to scale by a randomly generated scale between min and max speed.
    Assumed normalised already, and likely already oriented in the right direction.
    */
    virtual void    GenEmissionVelocity(Vector3Df& destVector);

    /** Internal utility method for generating a time-to-live for a particle. */
    //产生粒子的生存时间
    virtual float   GenEmissionTTL(void);

    /** Internal utility method for generating a colour for a particle. */
    //产生粒子的初始颜色
    virtual void    GenEmissionColour(CColorValue& destColour);

    //初始化重复时间
    virtual void    InitDurationRepeat();

    //获得绝对名字
    virtual AIRString GetType() = 0;

protected:
    Vector3Df       m_position;             //位置，相对于粒子系统的位置，即在粒子空间中的位置
    Vector3Df       m_direction;            //发射方向
    Vector3Df       m_vUp;                  //垂直于发射方向的向量
    float           m_fAngle;               //发射角度
	float			m_fVertAngle;
    float           m_fEmissionRate;        //每秒要发射的粒子数量
    float           m_fMinSpeed;            //粒子的最小发射速度
    float           m_fMaxSpeed;            //粒子的最大发射速度
    float           m_fMinTTL;              //粒子的最小存活时间
    float           m_fMaxTTL;              //粒子的最大存活时间
    float           m_fTimeStart;           //起始时间
    float           m_fDurationMin;         //最短持续时间
    float           m_fDurationMax;         //最大持续时间
    float           m_fDurationRemain;      //剩余时间
    float           m_fRepeatDelayMin;      //2次发射的最小时间间隔
    float           m_fRepeatDelayMax;      //2次发射的最大时间间隔
    float           m_fRepeatDelayRemain;   //Repeat delay left
    float           m_fRemainder;           //Fractions of particles wanted to be emitted last time
    CColorValue     m_clrRangeStart;        //产生粒子时的初始颜色值  一个随机颜色范围
    CColorValue     m_clrRangeEnd;          //产生粒子时的结束颜色值
    bool            m_bEmitted;             //是否被其他发射器发射
    bool            m_bEnable;              //是否可用
    bool            m_bLimitInPlane;        //是否将粒子限制在同发射方向垂直的平面上
    //Vector3Df       m_vParticleSpacePos;
    AIRString     m_strType;              //发射器类型
    AIRString     m_strName;              //发射器名字
    AIRString     m_strEmittedEmitter;    //被发射的发射器名字
    float           m_fInitScaleMin;        //粒子发射时的初始大小的最小值
    float           m_fInitScaleMax;        //粒子发射时的初始大小的最大值

    float           m_fDelayEmitTime;       //粒子延迟发射的时间

	//u32				m_nAccumEmitCount;		//发射器发射粒子数的累积最大数量,当发射器发射的粒子数累积超过该值的时候,就不再发射粒子了, 为0表示不限制
	//u32				m_nCurAccumCount;		//当前发射器的累积发射粒子数
};
