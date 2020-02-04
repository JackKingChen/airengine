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
	// ����Ŀ���λ�õ�������ӵ������
	Vector3Df vTranslate = m_pTarget->GetPosition() - m_vLookAt;
	// ������������ӵ㵽Ŀ���λ�ô�
	m_vLookAt = m_pTarget->GetPosition();
	// �ƶ������������
	m_vPosition += vTranslate;
	// �����ӵ㵽��������������
	m_vLook = m_vLookAt - m_vPosition;
	// �����ӵ㵽������ľ���
	m_fDistance = m_vLook.GetLength();
	m_vLook.Normalize();
	// �����������λ��,ʹ����������ӵ�ľ�����m_fMinDistance��m_fMaxDistance֮��
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
	// �������������������
	m_vRight = Vector3Df::UNIT_Y.CrossProduct(m_vLook);
	m_vRight.Normalize();
	// ������������Ϸ�������
	m_vUp = m_vLook.CrossProduct(m_vRight);
	// ������ͼ����
	m_matView.BuildCameraLookAtMatrixLH(m_vPosition, m_vLookAt, m_vUp);
	// ������׶��
	m_frustum.CalculateFrustum(m_matView);

	UpdateUserClipFrustum();
}

void CThirdPersonCamera::Rotate(const f32 fRadius)
{
    Quaternion qRotate;
    qRotate.FromAngleAxis(fRadius, Vector3Df::UNIT_Y);
	//m_vPosition = qRotate * m_vPosition;
	// ��ת��������ӵ������
	m_vLook = qRotate * m_vLook;
	// ͨ���µ����������������������
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