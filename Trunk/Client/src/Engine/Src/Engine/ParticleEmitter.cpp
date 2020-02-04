#include "stdafx.h"
#include "ParticleEmitter.h"
#include "MathUtil.h"
#include "StringUtil.h"

CParticleEmitter::CParticleEmitter() : m_fAngle(0), m_fEmissionRate(0), m_fMinSpeed(1), m_fMaxSpeed(1), 
m_fMinTTL(5),
m_fMaxTTL(5),
m_fTimeStart(0),
m_fDurationMin(0),
m_fDurationMax(0),
m_fDurationRemain(0),
m_fRepeatDelayMin(0),
m_fRepeatDelayMax(0),
m_fRepeatDelayRemain(0),
m_fRemainder(0),
m_bEmitted(false),
m_bEnable(true),
m_fInitScaleMin(1.0),
m_fInitScaleMax(1.0),
m_fDelayEmitTime(0.0f)
{
    SetDirection(Vector3Df::UNIT_Y);
    m_bLimitInPlane = false;
	m_particleType = PARTICLE_EMITTER;
	SetVertAngle( 0.0f );
	//m_nAccumEmitCount = m_nCurAccumCount = 0;
}


CParticleEmitter::~CParticleEmitter()
{

}

void CParticleEmitter::SetDirection(const Vector3Df& vDirection)
{
    m_direction = vDirection;
    m_direction.Normalize();

    m_vUp = m_direction.Perpendicular();
    m_vUp.Normalize();
}

const Vector3Df& CParticleEmitter::GetDirection() const
{
    return m_direction;
}

void CParticleEmitter::SetPosition(const Vector3Df& vPosition)
{
    m_position = vPosition;
}

void CParticleEmitter::GetPosition(Vector3Df& vPosition)
{
    vPosition = m_position;
}

Vector3Df& CParticleEmitter::GetPosition( void )
{
    return m_position;
}

void CParticleEmitter::SetAngle(float fAngle)
{
    m_fAngle = fAngle;
    AIR::DegreeToRadius(m_fAngle);
}

void CParticleEmitter::SetTimeToLive(float fTime)
{
    m_fMinTTL = m_fMaxTTL = fTime;
}

void CParticleEmitter::SetMinTimeToLive(float fTime)
{
    m_fMinTTL = fTime;
}

void CParticleEmitter::SetMaxTimeToLive(float fTime)
{
    m_fMaxTTL = fTime;
}

void CParticleEmitter::SetParticleColor(const CColorValue& color)
{
    m_clrRangeStart = m_clrRangeEnd = color;
}

void CParticleEmitter::SetColorRangeStart(const CColorValue& color)
{
    m_clrRangeStart = color;
}

void CParticleEmitter::SetColorRangeEnd(const CColorValue& color)
{
    m_clrRangeEnd = color;
}

int CParticleEmitter::GenParticleCount(float fElapse)
{
    unsigned short nRequest = 0;

	// 如果发射器的最大发射累积量是有限制的并且当前已经达到最大发射累积量
	// 就不再发射了
	//if ( m_nCurAccumCount >= m_nAccumEmitCount && m_nAccumEmitCount > 0 )
	//{
	//	return 0;
	//}

    //如果当前发射器可用
    if (m_bEnable)
    {
        //计算当前帧可以产生的粒子数
        m_fRemainder += fElapse * m_fEmissionRate;
        nRequest = (unsigned short)(m_fRemainder + 0.5f);
        m_fRemainder -= nRequest;

        //检查是否过期
        if (m_fDurationMax > 0)
        {
            //计算持续发射的剩余时间
            m_fDurationRemain -= fElapse;
            //如果持续发射的剩余时间为0,就设置发射器不能用
            if (m_fDurationRemain <= 0)
            {
                SetEnable(false);
            }
        }
    }
	//当前发射器不可用
    else
    {
		//2次发射的最大时间间隔不为0
        if (m_fRepeatDelayMax)
        {
            m_fRepeatDelayRemain -= fElapse;
            if (m_fRepeatDelayRemain <= 0)
            {
                SetEnable(true);
            }
        }
        if (m_fTimeStart)
        {
            m_fTimeStart -= fElapse;
            if (m_fTimeStart <= 0)
            {
                SetEnable(true);
                m_fTimeStart = 0;
            }
        }
    }

	// 如果发射器的最大发射累积量是有限制的
    /*
	if ( m_nAccumEmitCount > 0 )
	{
		// 如果即将超出限定量
		if ( m_nCurAccumCount + nRequest > m_nAccumEmitCount )
		{
			// 控制发射数量
			nRequest = m_nAccumEmitCount - m_nCurAccumCount;
			m_nCurAccumCount = m_nAccumEmitCount;
		}
		else
		{
			m_nCurAccumCount += nRequest;
		}
	}
    */

    return nRequest;
}

void CParticleEmitter::GenEmissionDirection(Vector3Df& destVector)
{
    if ( m_bLimitInPlane )
    {
        Quaternion q;
        destVector = m_direction.Perpendicular();
        q.FromAngleAxis( AIR::RangeRandom( -AIR::PI, AIR::PI ), m_direction );
        destVector = q * destVector;
    }
    else
    {
        ////if (m_fAngle != 0)
        ////{
        ////    // Randomise angle
        ////    float angle = AIR::UnitRandom() * m_fAngle;
        ////    // Randomise direction
        ////    destVector = CMathUtil::RandomDeviant(angle, m_direction, m_vUp);
        ////}
        ////else
        ////{
        ////    // Constant angle
        ////    destVector = m_direction;
        ////}
		destVector = CMathUtil::RandomDeviant( m_fAngle, m_fVertAngle, m_direction, m_vUp );
    }

}

void CParticleEmitter::GenEmissionVelocity(Vector3Df& destVector)
{
    float scalar;
    if (m_fMinSpeed != m_fMaxSpeed)
    {
        scalar = m_fMinSpeed + (AIR::UnitRandom() * (m_fMaxSpeed - m_fMinSpeed));
    }
    else
    {
        scalar = m_fMinSpeed;
    }

    destVector *= scalar;
}

float CParticleEmitter::GenEmissionTTL(void)
{
    if (m_fMaxTTL != m_fMinTTL)
    {
        return m_fMinTTL + (AIR::UnitRandom() * (m_fMaxTTL - m_fMinTTL));
    }
    else
    {
        return m_fMinTTL;
    }
}

void CParticleEmitter::GenEmissionColour(CColorValue& destColour)
{
    if (m_clrRangeStart != m_clrRangeEnd)
    {
        // Randomise
        //Real t = Math::UnitRandom();
        destColour.r = m_clrRangeStart.r + AIR::UnitRandom() * (m_clrRangeEnd.r - m_clrRangeStart.r);
        destColour.g = m_clrRangeStart.g + AIR::UnitRandom() * (m_clrRangeEnd.g - m_clrRangeStart.g);
        destColour.b = m_clrRangeStart.b + AIR::UnitRandom() * (m_clrRangeEnd.b - m_clrRangeStart.b);
        destColour.a = m_clrRangeStart.a + AIR::UnitRandom() * (m_clrRangeEnd.a - m_clrRangeStart.a);
    }
    else
    {
        destColour = m_clrRangeStart;
    }
}

void CParticleEmitter::SetDuration(float fTime)
{
    m_fDurationMin = m_fDurationMax = fTime;
    InitDurationRepeat();
}

void CParticleEmitter::SetMinDuration(float fTime)
{
    m_fDurationMin = fTime;
    InitDurationRepeat();
}

void CParticleEmitter::SetMaxDuration(float fTime)
{
    m_fDurationMax = fTime;
    InitDurationRepeat();
}

float CParticleEmitter::GetDuration() const
{
    return m_fDurationMin;
}

float CParticleEmitter::GetMinDuration() const
{
    return m_fDurationMin;
}

float CParticleEmitter::GetMaxDuration() const
{
    return m_fDurationMax;
}

void CParticleEmitter::SetRepeatDelay(float fTime)
{
    m_fRepeatDelayMin = m_fRepeatDelayMax = fTime;
    InitDurationRepeat();
}

float CParticleEmitter::GetAngle() const
{
    return m_fAngle;
}

float CParticleEmitter::GetEmissionRate() const
{
    return m_fEmissionRate;
}

float CParticleEmitter::GetMinSpeed() const
{
    return m_fMinSpeed;
}

float CParticleEmitter::GetMaxSpeed() const
{
    return m_fMaxSpeed;
}

float CParticleEmitter::GetTimeToLive() const
{
    return m_fMinTTL;
}

float CParticleEmitter::GetMinTimeToLive() const
{
    return m_fMinTTL;
}

float CParticleEmitter::GetMaxTimeToLive() const
{
    return m_fMaxTTL;
}

void CParticleEmitter::SetRepeatDelayMin(float fTime)
{
    m_fRepeatDelayMin = fTime;
    InitDurationRepeat();
}

float CParticleEmitter::GetStartTime() const
{
    return m_fTimeStart;
}

void CParticleEmitter::SetRepeatDelayMax(float fTime)
{
    m_fRepeatDelayMax = fTime;
    InitDurationRepeat();
}

void CParticleEmitter::SetEmissionRate(float fRate)
{
    m_fEmissionRate = fRate;
}

void CParticleEmitter::SetMinSpeed(float fSpeed)
{
    m_fMinSpeed = fSpeed;
}

void CParticleEmitter::SetMaxSpeed(float fSpeed)
{
    m_fMaxSpeed = fSpeed;
}

void CParticleEmitter::SetStartTime(float fTime)
{
    m_fTimeStart = fTime;
}

float CParticleEmitter::GetRepeatDelay() const
{
    return m_fRepeatDelayMin;
}

float CParticleEmitter::GetRepeatDelayMin() const
{
    return m_fRepeatDelayMin;
}

float CParticleEmitter::GetRepeatDelayMax() const
{
    return m_fRepeatDelayMax;
}

bool CParticleEmitter::IsEmitted() const
{
    return m_bEmitted;
}

void CParticleEmitter::SetEmitted(bool bEmitted)
{
    m_bEmitted = bEmitted;
}

void CParticleEmitter::SetEnable(bool bEnable)
{
    m_bEnable = bEnable;
    InitDurationRepeat();
}

bool CParticleEmitter::IsEnable()
{
    return m_bEnable;
}

void CParticleEmitter::InitDurationRepeat()
{
    if (m_bEnable)
    {
        if (m_fDurationMin == m_fDurationMax)
        {
            m_fDurationRemain = m_fDurationMin;
        }
        else
        {
            m_fDurationRemain = AIR::RangeRandom(m_fDurationMin, m_fDurationMax);
        }
    }
    else
    {
        // Reset repeat
        if (m_fRepeatDelayMin == m_fRepeatDelayMax)
        {
            m_fRepeatDelayRemain = m_fRepeatDelayMin;
        }
        else
        {
            m_fRepeatDelayRemain = AIR::RangeRandom(m_fRepeatDelayMax, m_fRepeatDelayMin);
        }

    }
}

void CParticleEmitter::GetEmissionColor(CColorValue& colorValue)
{
    colorValue = m_clrRangeStart;
}

void CParticleEmitter::GetColorRangeStart(CColorValue& colorValue)
{
    colorValue = m_clrRangeStart;
}

void CParticleEmitter::GetColorRangeEnd(CColorValue& colorValue)
{
    colorValue = m_clrRangeEnd;
}


bool CParticleEmitter::LoadFromXML(CMarkupSTL& xml)
{
    AIRString strValue("");

    m_strName = xml.GetAttrib("name").c_str();

    m_strEmittedEmitter = xml.GetAttrib("emitted_emitter").c_str();
    
    strValue = xml.GetAttrib("angle").c_str();
    float fAngle = atof(strValue.c_str())/* * AIR::DEGTORAD*/;
    SetAngle(fAngle);

	strValue = xml.GetAttrib("vert_angle").c_str();
	if ( strValue.length() > 0 )
	{
		fAngle = atof(strValue.c_str());
		SetVertAngle( fAngle );
	}

    strValue = xml.GetAttrib("time_to_live").c_str();
    SetTimeToLive(atof(strValue.c_str()));

	strValue = xml.GetAttrib( "time_to_live_min" ).c_str();
	if ( strValue.length() > 0 )
	{
		SetMinTimeToLive( atof(strValue.c_str()) );
	}

	strValue = xml.GetAttrib( "time_to_live_max" ).c_str();
	if ( strValue.length() > 0 )
	{
		SetMaxTimeToLive( atof(strValue.c_str()) );
	}

    strValue = xml.GetAttrib("direction").c_str();
    Vector3Df direction;
    sscanf(strValue.c_str(), "(%f,%f,%f)", &direction.x, &direction.y, &direction.z);
    SetDirection(direction);

    strValue = xml.GetAttrib("limit_in_plane").c_str();
    if ( strValue.size() > 0 )
    {
        CStringUtil::ToLowerCase( strValue );
        if ( strValue == "true" )
        {
            m_bLimitInPlane = true;
        }
        else
        {
            m_bLimitInPlane = false;
        }
    }

    strValue = xml.GetAttrib("velocity_min").c_str();
    float fSpeedMin = atof(strValue.c_str());
    SetMinSpeed(fSpeedMin);

    strValue = xml.GetAttrib("velocity_max").c_str();
    float fSpeedMax = atof(strValue.c_str());
    SetMaxSpeed(fSpeedMax);

    strValue = xml.GetAttrib("emission_rate").c_str();
    float fEmissionRate = atof(strValue.c_str());
    SetEmissionRate(fEmissionRate);

    strValue = xml.GetAttrib("color_range_start").c_str();
    float a, r, g, b;
    if (strValue.length() > 0)
    {
        sscanf(strValue.c_str(), "(%f, %f, %f, %f)", &a, &r, &g, &b);
        SetColorRangeStart(CColorValue(a, r, g, b));
    }

    strValue = xml.GetAttrib("color_range_end").c_str();
    if (strValue.length() > 0)
    {
        sscanf(strValue.c_str(), "(%f, %f, %f, %f)", &a, &r, &g, &b);
        SetColorRangeEnd(CColorValue(a, r, g, b));
    }

    strValue = xml.GetAttrib("position").c_str();
    Vector3Df position;
    sscanf(strValue.c_str(), "(%f,%f,%f)", &position.x, &position.y, &position.z);
    SetPosition(position);
    //SetParticleSpacePos( position );

    strValue = xml.GetAttrib("duration").c_str();
    if (strValue.length() > 0)
    {
        SetDuration(atof(strValue.c_str()));
    }

    strValue = xml.GetAttrib("duration_min").c_str();
    if (strValue.length() > 0)
    {
        SetMinDuration(atof(strValue.c_str()));
    }

    strValue = xml.GetAttrib("duration_max").c_str();
    if (strValue.length() > 0)
    {
        float fDurationMax = atof(strValue.c_str());
        if (fDurationMax < m_fDurationMin)
        {
            fDurationMax = m_fDurationMin;
        }
        SetMaxDuration(fDurationMax);
    }

    strValue = xml.GetAttrib("repeat_delay").c_str();
    if (strValue.length() > 0)
    {
        SetRepeatDelay(atof(strValue.c_str()));
    }

    strValue = xml.GetAttrib("repeat_delay_min").c_str();
    if (strValue.length() > 0)
    {
        SetRepeatDelayMin(atof(strValue.c_str()));
    }

    strValue = xml.GetAttrib("repeat_delay_max").c_str();
    if (strValue.length() > 0)
    {
        float repeatDelayMax = atof(strValue.c_str());
        if (repeatDelayMax < m_fRepeatDelayMin)
        {
            repeatDelayMax = m_fRepeatDelayMin;
        }
        SetRepeatDelayMax(repeatDelayMax);
    }

    strValue = xml.GetAttrib("init_scale_min").c_str();
    if ( strValue.length() > 0)
    {
        m_fInitScaleMin = atof(strValue.c_str());
    }

    strValue = xml.GetAttrib("init_scale_max").c_str();
    if ( strValue.length() > 0)
    {
        m_fInitScaleMax = atof(strValue.c_str());
    }

    strValue = xml.GetAttrib("delay_emit_time").c_str();
    if ( strValue.length() > 0 )
    {
        m_fDelayEmitTime = atof(strValue.c_str());
    }

	strValue = xml.GetAttrib("accum_emit_count").c_str();
	if ( strValue.length() > 0 )
	{
		//m_nAccumEmitCount = atoi(strValue.c_str());
	}

    return true;
}

bool CParticleEmitter::SaveToXMLNode(CMarkupSTL& xml)
{
    xml.AddAttrib("type", m_strType.c_str());
    xml.AddAttrib("name", m_strName.c_str());

    if (m_strEmittedEmitter != CStringUtil::BLANK)
    {
        xml.AddAttrib("emitted_emitter", m_strEmittedEmitter.c_str());
    }
    char szTmp[256] = { 0 };
    float fAngle = m_fAngle;
    AIR::RadiusToDegree(fAngle);
    sprintf(szTmp, "%f", fAngle);
    xml.AddAttrib("angle", szTmp);

	fAngle = m_fVertAngle;
	AIR::RadiusToDegree(fAngle);
	snprintf( szTmp, 255, "%f", fAngle );
	xml.AddAttrib("vert_angle", szTmp);

    sprintf(szTmp, "%f", m_fMinTTL);
    xml.AddAttrib("time_to_live", szTmp);

    sprintf( szTmp, "%f", m_fMinTTL );
    xml.AddAttrib("time_to_live_min", szTmp);
    sprintf(szTmp, "%f", m_fMaxTTL);
    xml.AddAttrib("time_to_live_max", szTmp);

    sprintf(szTmp, "(%f,%f,%f)", m_direction.x, m_direction.y, m_direction.z);
    xml.AddAttrib( "direction", szTmp );

    sprintf(szTmp, "(%f,%f,%f)", m_position.x, m_position.y, m_position.z);
    xml.AddAttrib( "position", szTmp );

    if ( m_bLimitInPlane )
    {
        xml.AddAttrib("limit_in_plane", "true");
    }
    else
    {
        xml.AddAttrib("limit_in_plane", "false");
    }

    sprintf(szTmp, "%f", m_fMinSpeed);
    xml.AddAttrib("velocity_min", szTmp);
    sprintf(szTmp, "%f", m_fMaxSpeed);
    xml.AddAttrib("velocity_max", szTmp);

	sprintf(szTmp, "%f", m_fEmissionRate);
    xml.AddAttrib("emission_rate", szTmp);

    sprintf(szTmp, "(%f, %f, %f, %f)", m_clrRangeStart.a, m_clrRangeStart.r,m_clrRangeStart.g, m_clrRangeStart.b);
    xml.AddAttrib("color_range_start", szTmp);

    sprintf(szTmp, "(%f, %f, %f, %f)", m_clrRangeEnd.a, m_clrRangeEnd.r,m_clrRangeEnd.g, m_clrRangeEnd.b);
    xml.AddAttrib("color_range_end", szTmp);

    sprintf(szTmp, "%f", m_fDurationMin);
    xml.AddAttrib("duration", szTmp);

    if (m_fDurationMin != m_fDurationMax)
    {
        xml.AddAttrib("duration_min", szTmp);
        sprintf(szTmp, "%f", m_fDurationMax);
        xml.AddAttrib("duration_max", szTmp);
    }

    sprintf(szTmp, "%f", m_fRepeatDelayMin);
    xml.AddAttrib("repeat_delay", szTmp);
    if (m_fRepeatDelayMin != m_fRepeatDelayMax)
    {
        xml.AddAttrib("repeat_delay_min", szTmp);
        sprintf(szTmp, "%f", m_fRepeatDelayMax);
        xml.AddAttrib("repeat_delay_max", szTmp);
    }

    sprintf(szTmp, "%f", m_fInitScaleMin);
    xml.AddAttrib("init_scale_min", szTmp);

    sprintf(szTmp, "%f", m_fInitScaleMax);
    xml.AddAttrib("init_scale_max", szTmp);

    sprintf(szTmp, "%f", m_fDelayEmitTime);
    xml.AddAttrib("delay_emit_time", szTmp);

	//xml.AddAttrib( "accum_emit_count", int(m_nAccumEmitCount) );

    return true;
}


void    CParticleEmitter::SetIsLimitInPlane( bool b )
{
    m_bLimitInPlane = b;
}


bool    CParticleEmitter::GetIsLimitInPlane( void )
{
    return m_bLimitInPlane;
}


void CParticleEmitter::SetName(const AIRString& strName)
{
    m_strName = strName;
}

void CParticleEmitter::SetEmittedEmitterName(const AIRString& strName)
{
    m_strEmittedEmitter = strName;
}

void CParticleEmitter::Copy(CParticleEmitter* pEmitterClone)
{
    if ( !pEmitterClone )
    {
        return;
    }
    *this = *pEmitterClone;
    m_strType = GetType();
}


void CParticleEmitter::Reset( void )
{
	if (m_fDurationMin == m_fDurationMax)
	{
		m_fDurationRemain = m_fDurationMin;
	}
	else
	{
		m_fDurationRemain = AIR::RangeRandom(m_fDurationMin, m_fDurationMax);
	}

	if (m_fRepeatDelayMin == m_fRepeatDelayMax)
	{
		m_fRepeatDelayRemain = m_fRepeatDelayMin;
	}
	else
	{
		m_fRepeatDelayRemain = AIR::RangeRandom(m_fRepeatDelayMax, m_fRepeatDelayMin);
	}

	//m_nCurAccumCount = 0;

	m_bEnable = true;
}