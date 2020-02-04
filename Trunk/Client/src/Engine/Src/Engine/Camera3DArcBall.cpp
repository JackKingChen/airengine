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
    // 计算摄像机的右手向量
    m_vRight = Vector3Df::UNIT_Y.CrossProduct(m_vLook);
    m_vRight.Normalize();
    // 计算摄像机的上方向向量
    m_vUp = m_vLook.CrossProduct(m_vRight);
    // 计算视图矩阵
    m_matView.BuildCameraLookAtMatrixLH(m_vPosition, m_vLookAt, m_vUp);
    // 计算视锥体
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
    //// 计算摄像机的右手向量
    //m_vRight = Vector3Df::UNIT_Y.CrossProduct(m_vLook);
    //m_vRight.Normalize();
    //// 计算摄像机的上方向向量
    //m_vUp = m_vLook.CrossProduct(m_vRight);
    //// 计算视图矩阵
    //m_matView.BuildCameraLookAtMatrixLH(m_vPosition, m_vLookAt, m_vUp);
    BuildViewMatrix();
    // 计算视锥体
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
    // 如果当前的移动速度为正数,表明摄像机向后移动
    if ( m_fCurMoveNFSpeed > 0.0f )
    {
        m_fCurMoveNFSpeed -= m_fDelta;
        if ( m_fCurMoveNFSpeed < 0.0f )
        {
            m_fCurMoveNFSpeed = 0.0f;
            return;
        }
    }
    // 如果当前的移动速度为负数,表明摄像机向前移动
    else if ( m_fCurMoveNFSpeed < 0.0f )
    {
        m_fCurMoveNFSpeed += m_fDelta;
        if ( m_fCurMoveNFSpeed > 0.0f )
        {
            m_fCurMoveNFSpeed = 0.0f;
            return;
        }
    }

    // 获得当前帧摄像机的移动距离
    float delta = m_fCurMoveNFSpeed * ENGINE_INST->GetTimeSinceLastFrame();

    // 计算出摄像机到视点的距离
    m_fDistance += delta;
    // 如果摄像机到视点达到最小距离
    if ( m_fDistance < m_fMinDistance )
    {
        // 设置当前速度为0
        m_fDistance = m_fMinDistance;
        m_fCurMoveNFSpeed = 0.0f;
    }
    // 如果摄像机到视点达到最大距离
    else if ( m_fDistance > m_fMaxDistance )
    {
        // 设置当前速度为0
        m_fDistance = m_fMaxDistance;
        m_fCurMoveNFSpeed = 0.0f;
    }
    //m_vLook.Normalize();
    m_vPosition = m_vLookAt - m_vLook * m_fDistance;
    //// 计算摄像机的右手向量
    //m_vRight = Vector3Df::UNIT_Y.CrossProduct(m_vLook);
    //m_vRight.Normalize();
    //// 计算摄像机的上方向向量
    //m_vUp = m_vLook.CrossProduct(m_vRight);
    //// 计算视图矩阵
    //m_matView.BuildCameraLookAtMatrixLH(m_vPosition, m_vLookAt, m_vUp);
    BuildViewMatrix();
    // 计算视锥体
    m_frustum.CalculateFrustum(m_matView);

    Vector3Df v = Vector3Df::NEGATIVE_UNIT_Y;
    m_fAngel = v.GetRadianAngleWith( m_vLook );

	//UpdateUserClipFrustum();
}


void	Camera3DArcBall::Rotate( f32 fVertRadian, f32 fHorzRadian )
{
    Quaternion qRotateH;
    qRotateH.FromAngleAxis( fHorzRadian, Vector3Df::UNIT_Y );
    // 水平旋转摄像机视向量角度 fHorzRadian
    m_vLook = qRotateH * m_vLook;
    // 计算摄像机的右手向量
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
    // 垂直旋转摄像机视向量角度 fVertRadian
    qRotateV.FromAngleAxis( fVertRadian, m_vRight );
    m_vLook = qRotateV * m_vLook;
    m_vLook.Normalize();
    // 通过新的视向量计算摄像机的坐标
    m_vPosition = m_vLookAt - m_vLook * m_fDistance;
    // 计算摄像机的上方向向量
    m_vUp = m_vLook.CrossProduct(m_vRight);
    // 计算视图矩阵
    BuildViewMatrixImpl();
    // 计算视锥体
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
    // 计算摄像机的右手向量
    m_vRight = Vector3Df::UNIT_Y.CrossProduct(m_vLook);
    m_vRight.Normalize();
    // 计算摄像机的上方向向量
    m_vUp = m_vLook.CrossProduct(m_vRight);
    // 计算视图矩阵
    BuildViewMatrixImpl();
    // 计算视锥体
    m_frustum.CalculateFrustum(m_matView);
}

void	Camera3DArcBall::MoveNearFar( f32 fdst)
{
    m_vLook.Normalize();
    m_fDistance += fdst;
    m_fDistance = m_fDistance < m_fMinDistance ? m_fMinDistance : (m_fDistance > m_fMaxDistance ? m_fMaxDistance : m_fDistance);
    m_vPosition = m_vLookAt - m_vLook * m_fDistance;
    // 计算摄像机的右手向量
    m_vRight = Vector3Df::UNIT_Y.CrossProduct(m_vLook);
    m_vRight.Normalize();
    // 计算摄像机的上方向向量
    m_vUp = m_vLook.CrossProduct(m_vRight);
    // 计算视图矩阵
    //m_matView.BuildCameraLookAtMatrixLH(m_vPosition, m_vLookAt, m_vUp);
    //// 计算视锥体
    //m_frustum.CalculateFrustum(m_matView);
}


void    Camera3DArcBall::AddNFAccelerate( bool bForward )
{
    // 如果摄像机向前移动,m_fCurMoveNFSpeed为负
    // 如果摄像机向后移动,m_fCurMoveNFSpeed为正
    // 摄像机向前移动
    if ( bForward )
    {
        // 增加向前移动的速度增量
        m_fCurMoveNFSpeed -= 20 * m_fDelta;
        if ( m_fCurMoveNFSpeed < -m_fMaxMoveNFSpeed )
        {
            m_fCurMoveNFSpeed = -m_fMaxMoveNFSpeed;
        }
    }
    // 摄像机向后移动
    else
    {
        // 增加向后移动的速度增量
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
	// 计算摄像机的右手向量
	m_vRight = Vector3Df::UNIT_Y.CrossProduct(m_vLook);
	m_vRight.Normalize();
	// 计算摄像机的上方向向量
	m_vUp = m_vLook.CrossProduct(m_vRight);
	// 计算视图矩阵
	BuildViewMatrixImpl();
	// 计算视锥体
	m_frustum.CalculateFrustum(m_matView);

	//Vector3Df v = Vector3Df::NEGATIVE_UNIT_Y;
	//m_fAngel = v.GetRadianAngleWith( m_vLook );
}