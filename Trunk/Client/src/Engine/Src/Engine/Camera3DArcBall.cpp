#include "stdafx.h"
//#include "stdafx.h"
#include "Camera3DArcBall.h"
#include "Engine.h"

// ctor
Camera3DArcBall::Camera3DArcBall(const Vector3Df& vPos, const Vector3Df& vLookAt) : 
CThirdPersonCamera( vPos, vLookAt )
{
    m_vLookAt = vLookAt;
    m_vPosition = vPos;
    m_vLook = m_vLookAt - m_vPosition;
    m_fDistance = m_vLook.GetLength();
    m_vLook.Normalize();
    // �������������������
    m_vRight = Vector3Df::UNIT_Y.CrossProduct(m_vLook);
    m_vRight.Normalize();
    // ������������Ϸ�������
    m_vUp = m_vLook.CrossProduct(m_vRight);
    // ������ͼ����
    m_matView.BuildCameraLookAtMatrixLH(m_vPosition, m_vLookAt, m_vUp);
    // ������׶��
    m_frustum.CalculateFrustum(m_matView);

    m_fCurMoveNFSpeed = 0.0f;
    m_fMaxMoveNFSpeed = 100.0f;
    m_fDelta = 5.0f;
}


Camera3DArcBall::Camera3DArcBall(const Vector3Df& vPos, const Vector3Df& vLookAt, f32 fMaxDistance, f32 fMinDistance, CSceneNode* pTarget) :
CThirdPersonCamera(vPos, vLookAt, fMaxDistance, fMinDistance, pTarget )
{
    m_vLookAt = vLookAt;
    m_vPosition = vPos;
    m_vLook = m_vLookAt - m_vPosition;
    m_fDistance = m_vLook.GetLength();
    m_vLook.Normalize();
    m_fDistance = m_fDistance < m_fMinDistance ? m_fMinDistance : (m_fDistance > m_fMaxDistance ? m_fMaxDistance : m_fDistance);
    m_vPosition = m_vLookAt - m_vLook * m_fDistance;
    //// �������������������
    //m_vRight = Vector3Df::UNIT_Y.CrossProduct(m_vLook);
    //m_vRight.Normalize();
    //// ������������Ϸ�������
    //m_vUp = m_vLook.CrossProduct(m_vRight);
    //// ������ͼ����
    //m_matView.BuildCameraLookAtMatrixLH(m_vPosition, m_vLookAt, m_vUp);
    BuildViewMatrix();
    // ������׶��
    m_frustum.CalculateFrustum(m_matView);

    m_fCurMoveNFSpeed = 0.0f;
    m_fMaxMoveNFSpeed = 1000.0f;
    m_fDelta = 5.0f;
}

// dtor
Camera3DArcBall::~Camera3DArcBall( void )
{

}

void    Camera3DArcBall::Update( void )
{
	if ( !m_bNeedAutoUpdate )
	{
		return;
	}
    if ( m_fCurMoveNFSpeed == 0.0f )
    {
        //return;
    }
    // �����ǰ���ƶ��ٶ�Ϊ����,�������������ƶ�
    if ( m_fCurMoveNFSpeed > 0.0f )
    {
        m_fCurMoveNFSpeed -= m_fDelta;
        if ( m_fCurMoveNFSpeed < 0.0f )
        {
            m_fCurMoveNFSpeed = 0.0f;
            return;
        }
    }
    // �����ǰ���ƶ��ٶ�Ϊ����,�����������ǰ�ƶ�
    else if ( m_fCurMoveNFSpeed < 0.0f )
    {
        m_fCurMoveNFSpeed += m_fDelta;
        if ( m_fCurMoveNFSpeed > 0.0f )
        {
            m_fCurMoveNFSpeed = 0.0f;
            return;
        }
    }

    // ��õ�ǰ֡��������ƶ�����
    float delta = m_fCurMoveNFSpeed * ENGINE_INST->GetTimeSinceLastFrame();

    // �������������ӵ�ľ���
    m_fDistance += delta;
    // �����������ӵ�ﵽ��С����
    if ( m_fDistance < m_fMinDistance )
    {
        // ���õ�ǰ�ٶ�Ϊ0
        m_fDistance = m_fMinDistance;
        m_fCurMoveNFSpeed = 0.0f;
    }
    // �����������ӵ�ﵽ������
    else if ( m_fDistance > m_fMaxDistance )
    {
        // ���õ�ǰ�ٶ�Ϊ0
        m_fDistance = m_fMaxDistance;
        m_fCurMoveNFSpeed = 0.0f;
    }
    //m_vLook.Normalize();
    m_vPosition = m_vLookAt - m_vLook * m_fDistance;
    //// �������������������
    //m_vRight = Vector3Df::UNIT_Y.CrossProduct(m_vLook);
    //m_vRight.Normalize();
    //// ������������Ϸ�������
    //m_vUp = m_vLook.CrossProduct(m_vRight);
    //// ������ͼ����
    //m_matView.BuildCameraLookAtMatrixLH(m_vPosition, m_vLookAt, m_vUp);
    BuildViewMatrix();
    // ������׶��
    m_frustum.CalculateFrustum(m_matView);

    Vector3Df v = Vector3Df::NEGATIVE_UNIT_Y;
    m_fAngel = v.GetRadianAngleWith( m_vLook );

	//UpdateUserClipFrustum();
}


void	Camera3DArcBall::Rotate( f32 fVertRadian, f32 fHorzRadian )
{
    Quaternion qRotateH;
    qRotateH.FromAngleAxis( fHorzRadian, Vector3Df::UNIT_Y );
    // ˮƽ��ת������������Ƕ� fHorzRadian
    m_vLook = qRotateH * m_vLook;
    // �������������������
    m_vRight = Vector3Df::UNIT_Y.CrossProduct(m_vLook);
    m_vRight.Normalize();

    Quaternion qRotateV;
    //if ( m_fAngel - fVertRadian > (DEGTORAD * 80.0f) )
    //{
    //    fVertRadian = 0.0f;
    //}
    //else if ( m_fAngel - fVertRadian < -(DEGTORAD * 80.0f) )
    //{
    //    fVertRadian = 0.0f;
    //}
	if ( m_fAngel - fVertRadian < (DEGTORAD * 10.0f) )
	{
		fVertRadian = 0.0f;
	}
	else if ( m_fAngel - fVertRadian > (DEGTORAD * 170.0f) )
	{
		fVertRadian = 0.0f;
	}
    // ��ֱ��ת������������Ƕ� fVertRadian
    qRotateV.FromAngleAxis( fVertRadian, m_vRight );
    m_vLook = qRotateV * m_vLook;
    m_vLook.Normalize();
    // ͨ���µ����������������������
    m_vPosition = m_vLookAt - m_vLook * m_fDistance;
    // ������������Ϸ�������
    m_vUp = m_vLook.CrossProduct(m_vRight);
    // ������ͼ����
    BuildViewMatrixImpl();
    // ������׶��
    m_frustum.CalculateFrustum(m_matView);
}

void	Camera3DArcBall::MoveUpDown(bool bUp )
{
    float fDeltaY = 1.0f;
    if ( !bUp )
    {
        fDeltaY = -fDeltaY;
    }
    m_vLookAt.y += fDeltaY;
    m_vPosition.y += fDeltaY;
    // �������������������
    m_vRight = Vector3Df::UNIT_Y.CrossProduct(m_vLook);
    m_vRight.Normalize();
    // ������������Ϸ�������
    m_vUp = m_vLook.CrossProduct(m_vRight);
    // ������ͼ����
    BuildViewMatrixImpl();
    // ������׶��
    m_frustum.CalculateFrustum(m_matView);
}

void	Camera3DArcBall::MoveNearFar( f32 fdst)
{
    m_vLook.Normalize();
    m_fDistance += fdst;
    m_fDistance = m_fDistance < m_fMinDistance ? m_fMinDistance : (m_fDistance > m_fMaxDistance ? m_fMaxDistance : m_fDistance);
    m_vPosition = m_vLookAt - m_vLook * m_fDistance;
    // �������������������
    m_vRight = Vector3Df::UNIT_Y.CrossProduct(m_vLook);
    m_vRight.Normalize();
    // ������������Ϸ�������
    m_vUp = m_vLook.CrossProduct(m_vRight);
    // ������ͼ����
    //m_matView.BuildCameraLookAtMatrixLH(m_vPosition, m_vLookAt, m_vUp);
    //// ������׶��
    //m_frustum.CalculateFrustum(m_matView);
}


void    Camera3DArcBall::AddNFAccelerate( bool bForward )
{
    // ����������ǰ�ƶ�,m_fCurMoveNFSpeedΪ��
    // ������������ƶ�,m_fCurMoveNFSpeedΪ��
    // �������ǰ�ƶ�
    if ( bForward )
    {
        // ������ǰ�ƶ����ٶ�����
        m_fCurMoveNFSpeed -= 20 * m_fDelta;
        if ( m_fCurMoveNFSpeed < -m_fMaxMoveNFSpeed )
        {
            m_fCurMoveNFSpeed = -m_fMaxMoveNFSpeed;
        }
    }
    // ���������ƶ�
    else
    {
        // ��������ƶ����ٶ�����
        m_fCurMoveNFSpeed += 20 * m_fDelta;
        if ( m_fCurMoveNFSpeed > m_fMaxMoveNFSpeed )
        {
            m_fCurMoveNFSpeed = m_fMaxMoveNFSpeed;
        }
    }
}

void	Camera3DArcBall::ResetCameraToLookAtCenterPoint( void )
{
	m_vLookAt = Vector3Df::ZERO;
	m_vPosition = m_vLookAt - m_vLook * m_fDistance;
	// �������������������
	m_vRight = Vector3Df::UNIT_Y.CrossProduct(m_vLook);
	m_vRight.Normalize();
	// ������������Ϸ�������
	m_vUp = m_vLook.CrossProduct(m_vRight);
	// ������ͼ����
	BuildViewMatrixImpl();
	// ������׶��
	m_frustum.CalculateFrustum(m_matView);

	//Vector3Df v = Vector3Df::NEGATIVE_UNIT_Y;
	//m_fAngel = v.GetRadianAngleWith( m_vLook );
}