#include "stdafx.h"
#include "ThirdPersonCamera.h"
#include "Quaternion.h"
//#include "SystemMgr.h"

CThirdPersonCamera::CThirdPersonCamera(const Vector3Df& vPos, const Vector3Df& vLookAt) : 
CCamera(vPos, vLookAt)
{
	m_pTarget = NULL;
	m_fMoveSpeed = 5.0f;
	m_bNeedTrack = false;
}

CThirdPersonCamera::CThirdPersonCamera(const Vector3Df& vPos, const Vector3Df& vLookAt, f32 fMaxDistance, f32 fMinDistance, CSceneNode* pTarget) : 
CCamera(vPos, vLookAt)
{
	m_fMaxDistance = fMaxDistance;
	m_fMinDistance = fMinDistance; 
	m_pTarget = pTarget;
    m_vLook = m_vLookAt - m_vPosition;
    m_fDistance = m_vLook.GetLength();
    m_vLook.Normalize();
	m_fMoveSpeed = 5.0f;
	m_bNeedTrack = false;
}

CThirdPersonCamera::~CThirdPersonCamera()
{
    
}

void CThirdPersonCamera::Update()
{
	if ( !m_bNeedAutoUpdate )
	{
		return;
	}
	if ( !m_pTarget )
	{
		return;
	}
	// 计算目标的位置到摄像机视点的向量
	Vector3Df vTranslate = m_pTarget->GetPosition() - m_vLookAt;
	// 设置摄像机的视点到目标的位置处
	m_vLookAt = m_pTarget->GetPosition();
	// 移动摄像机的坐标
	m_vPosition += vTranslate;
	// 计算视点到摄像机坐标的向量
	m_vLook = m_vLookAt - m_vPosition;
	// 计算视点到摄像机的距离
	m_fDistance = m_vLook.GetLength();
	m_vLook.Normalize();
	// 设置摄像机的位置,使得摄像机到视点的距离在m_fMinDistance到m_fMaxDistance之间
	if (m_fDistance > m_fMaxDistance)
	{
		m_vPosition = m_vLookAt - m_vLook * m_fMaxDistance;
		m_fDistance = m_fMaxDistance;
	}
	else if (m_fDistance < m_fMinDistance)
	{
		m_vPosition = m_vLookAt - m_vLook * m_fMinDistance;
		m_fDistance = m_fMinDistance;
	}
	// 计算摄像机的右手向量
	m_vRight = Vector3Df::UNIT_Y.CrossProduct(m_vLook);
	m_vRight.Normalize();
	// 计算摄像机的上方向向量
	m_vUp = m_vLook.CrossProduct(m_vRight);
	// 计算视图矩阵
	m_matView.BuildCameraLookAtMatrixLH(m_vPosition, m_vLookAt, m_vUp);
	// 计算视锥体
	m_frustum.CalculateFrustum(m_matView);

	UpdateUserClipFrustum();
}

void CThirdPersonCamera::Rotate(const f32 fRadius)
{
    Quaternion qRotate;
    qRotate.FromAngleAxis(fRadius, Vector3Df::UNIT_Y);
	//m_vPosition = qRotate * m_vPosition;
	// 旋转摄像机到视点的向量
	m_vLook = qRotate * m_vLook;
	// 通过新的视向量计算摄像机的坐标
	m_vPosition = m_vLookAt - m_vLook * m_fDistance;
}


void CThirdPersonCamera::MoveUpDown(const f32 fDy)
{
	m_vPosition += Vector3Df(0.0f, fDy, 0.0f);
}


void CThirdPersonCamera::MoveNearFar(const f32 fdst)
{
	m_fDistance += fdst;
	m_fDistance = m_fDistance < m_fMinDistance ? m_fMinDistance : m_fDistance;
	m_fDistance = m_fDistance > m_fMaxDistance ? m_fMaxDistance : m_fDistance;
	m_vPosition = m_vLookAt - m_vLook * m_fDistance;
}


void CThirdPersonCamera::Walk(f32 fSpeed)
{
    m_vPosition += m_vLook * fSpeed;
    if ((m_vPosition - m_vLookAt).GetLength() > m_fMaxDistance)
    {
        m_vPosition = m_vLookAt - m_vLook * m_fMaxDistance;
    }
    else if ((m_vPosition - m_vLookAt).GetLength() < m_fMinDistance )
    {
        m_vPosition = m_vLookAt - m_vLook * m_fMinDistance;
    }
}

float CThirdPersonCamera::GetDistance() const 
{
	return m_fDistance;
}

void CThirdPersonCamera::SetMaxDistance(f32 fMaxDistance)
{
    m_fMaxDistance = fMaxDistance;
}

void CThirdPersonCamera::SetMinDistance(f32 fMinDistance)
{
    m_fMinDistance = fMinDistance;
}

void CThirdPersonCamera::SetTarget(CSceneNode* pTarget)
{
    m_pTarget = pTarget;
}


void CThirdPersonCamera::SetMoveSpeed( float speed )
{
	m_fMoveSpeed = speed;
}

float CThirdPersonCamera::GetMoveSpeed( void )
{
	return m_fMoveSpeed;
}

void CThirdPersonCamera::SetNeedTrack( bool bTrack )
{
	m_bNeedTrack = bTrack;
}

bool CThirdPersonCamera::GetNeedTrack( void )
{
	return m_bNeedTrack;
}