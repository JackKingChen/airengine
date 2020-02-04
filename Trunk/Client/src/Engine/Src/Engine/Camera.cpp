#include "stdafx.h"
#include "Camera.h"
#include "Engine.h"
//#include <dinput.h>


CCamera::CCamera(void) :
m_bUserClipEnable(true)
{
    m_vLook = Vector3Df(0.0, 0.0, 1.0);
    m_vUp = Vector3Df(0.0, 1.0, 0.0);
    m_vRight = Vector3Df(1.0, 0.0, 0.0);
    m_vPosition = Vector3Df(0.0, 0.0, 0.0);
    m_vLookAt = Vector3Df(0.0, 0.0, 100.0);
    m_fMoveSpeed = 1.0f;
    m_fRotationSpeed = 0.2f;
	m_bNeedAutoUpdate = true;
	m_UserClipViewFar = 1000.0f;
}

CCamera::CCamera(const Vector3Df& vPosition, const Vector3Df& vLookAt) : 
m_vPosition(vPosition), 
m_vLookAt(vLookAt), 
m_vUp(0, 1, 0)
, m_bUserClipEnable(true)
{
    m_vLook = m_vLookAt - m_vPosition;
    m_fDistance = m_vLook.GetLength();
    m_vLook.Normalize();
    m_matView.BuildCameraLookAtMatrixLH(m_vPosition, m_vLookAt, m_vUp);
	m_bNeedAutoUpdate = true;
	m_UserClipViewFar = 1000.0f;
}

CCamera::CCamera(
				 const Vector3Df& up, 
				 const Vector3Df& right, 
				 const Vector3Df& look, 
				 const Vector3Df& position, 
                 f32 fNear, 
				 f32 fFar, 
				 f32 fFov, 
				 f32 fWidth, 
				 f32 fHeight) : 
m_vUp(up), 
m_vRight(right), 
m_vLook(look), 
m_vPosition(position),
m_fNear(fNear), 
m_fFar(fFar), 
m_fFOV(fFov), 
m_fViewWidth(fWidth), 
m_fViewHeight(fHeight), 
m_fMoveSpeed(1.0f), 
m_fRotationSpeed(1.0f),
m_UserClipViewFar(fFar)
, m_bUserClipEnable(true)
{
#ifdef LEFT_HAND_COORDINATE
    m_matView.BuildCameraLookAtMatrixLHByDirection(m_vLook, m_vRight, m_vUp, m_vPosition);
    m_matProject.BuildProjectionMatrixPerspectiveFovLH(m_fFOV, m_fViewWidth / m_fViewHeight, m_fNear, m_fFar);
#else
    m_matView.BuildCameraLookAtMatrixRHByDirection(m_vLook, m_vRight, m_vUp, m_vPosition);
    m_matProject.BuildProjectionMatrixPerspectiveFovRH(m_fFOV, m_fViewWidth / m_fViewHeight, m_fNear, m_fFar);
#endif
    
    m_frustum.SetPerspective(m_fNear, m_fFar, m_fFOV, m_fViewWidth, m_fViewHeight);
    m_frustum.CalculateFrustum(m_matView);
	m_bNeedAutoUpdate = true;
}

CCamera::CCamera(const Vector3Df& vPosition, const Vector3Df& vTarget, f32 fNear, f32 fFar) : 
m_vPosition(vPosition), 
m_vLookAt(vTarget), 
m_fNear(fNear), 
m_fFar(fFar), 
m_fMoveSpeed(3.0), 
m_fRotationSpeed(0.5),
m_UserClipViewFar(fFar)
, m_bUserClipEnable(true)
{
    m_vUp = Vector3Df(0.0, 1.0, 0.0);
    m_vLook = m_vLookAt - m_vPosition;
    //m_vLook.Normalize();
    //m_vRight = m_vUp.CrossProduct(m_vLook);
    //m_matView.BuildCameraLookAtMatrixLHByDirection(m_vLook, m_vRight, m_vUp, m_vPosition);
    BuildViewMatrix();
    //m_frustum.SetPerspectiveLH();
    Test();
	m_bNeedAutoUpdate = true;
}

CCamera::~CCamera()
{

}

bool CCamera::IsVisable(const AABBBox3Df& box) const
{
    return m_frustum.IsAABBIn(box);
}

bool CCamera::IsVisable(const Vector3Df& center, f32 fRadius) const
{
    return m_frustum.IsSphereIn(center, fRadius);
}

void CCamera::BuildReflectCamara( const Plane3Df& plane, CCamera* pDstCamera )
{
	if ( !pDstCamera )
	{
		return;
	}
	float dist = plane.GetDistanceTo( m_vPosition );
	Vector3Df vPosReflect = m_vPosition - plane.Normal * 2 * dist;
	dist = plane.GetDistanceTo( m_vLookAt );
	Vector3Df vLookAtReflect = m_vLookAt - plane.Normal * 2 * dist;
	pDstCamera->SetPosition( vPosReflect );
	pDstCamera->SetLookAt( vLookAtReflect );
	pDstCamera->SetUp( Vector3Df::NEGATIVE_UNIT_Y );
	pDstCamera->RecalculateByPositionAndLookAt();
}


void CCamera::RecalculateByPositionAndLookAt( void )
{
	m_vLook = m_vLookAt - m_vPosition;
	m_fDistance = m_vLook.GetLength();
	m_vLook.Normalize();
	m_vRight = m_vUp.CrossProduct(m_vLook);
	m_vRight.Normalize();
	m_vUp.Normalize();
	BuildViewMatrixImpl();
    //BuildViewMatrix();
	//IRenderer* pRenderer = ENGINE_INST->GetRenderer();
	//pRenderer->SetViewTransform(&m_matView);
	m_frustum.CalculateFrustum(m_matView);

	//UpdateUserClipFrustum();
}


void CCamera::Update()
{
	if ( !m_bNeedAutoUpdate )
	{
		return;
	}

    m_vPosition = m_vLookAt - m_vLook * m_fDistance;
    //OutputDebugString(GetString("update camera position:%f, %f, %f\n", m_vPosition.x, m_vPosition.y, m_vPosition.z));

    BuildViewMatrix();
    
    //注意，不应该在这里设置viewtransform，应该在场景渲染前设置
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    pRenderer->SetViewTransform(&m_matView);
    m_frustum.CalculateFrustum(m_matView);  

    if (m_bUserClipEnable)
    {
        UpdateUserClipFrustum();
    }
	
}

void CCamera::UpdateBillBoard()
{
    Matrix4f matView;
    GetViewMatrix(&matView);

    matView.Inverse();

    m_matBillboard[0] = matView[0];
    m_matBillboard[2] = matView[2];
    m_matBillboard[8] = matView[8];
    m_matBillboard[10] = matView[10];
}

void CCamera::GetBillboardTransform(Matrix4f& matWorld)
{
    matWorld *= m_matBillboard;
}

void CCamera::Walk(float fSpeed)
{
    m_vPosition += m_vLook * fSpeed;
    m_vLookAt += m_vLook * fSpeed;
}

void CCamera::Strafe(float fSpeed)
{
    m_vPosition += m_vRight * fSpeed;
    m_vLookAt += m_vRight * fSpeed;
}

void CCamera::RaiseLow(f32 fSpeed)
{
    m_vPosition += m_vUp * fSpeed;
    m_vLookAt += m_vUp * fSpeed;
}

void CCamera::Fly(float fSpeed)
{
    m_vPosition += m_vUp * fSpeed;
    m_vLookAt += m_vRight * fSpeed;
}

void CCamera::GetPosition(Vector3Df& vPos) const
{
    vPos = m_vPosition;
}

void CCamera::SetPosition(const Vector3Df& vPos)
{
    m_vPosition = vPos;
    m_vLook = m_vLookAt - m_vPosition;
    m_fDistance = m_vLook.GetLength();
    m_vLook.Normalize();
}

void CCamera::SetViewport(f32 fNear, f32 fFar, f32 fFov, f32 fWidth, f32 fHeight, bool bPerspective)
{
    m_fNear = fNear;
    m_fFar = fFar;
    m_fFOV = fFov;
    m_fViewWidth = fWidth;
    m_fViewHeight = fHeight;

    if (bPerspective)
    {
#ifdef LEFT_HAND_COORDINATE
        m_matProject.BuildProjectionMatrixPerspectiveFovLH(fFov, fWidth / fHeight, fNear, fFar);
#else
        m_matProject.BuildProjectionMatrixPerspectiveFovRH(fFov, fWidth / fHeight, fNear, fFar);
#endif
        
        m_frustum.SetPerspective(fNear, fFar, fFov, fWidth, fHeight);
    }
    else 
    {
#ifdef LEFT_HAND_COORDINATE
        m_matProject.BuildProjectionMatrixOrthoLH(fWidth, fHeight, fNear, fFar);
#else
        m_matProject.BuildProjectionMatrixOrthoRH(fWidth, fHeight, fNear, fFar);
#endif
        
        m_frustum.SetOrtho(fWidth, fHeight, fNear, fFar);
    }

    m_bPerspective = bPerspective;
}

void CCamera::SetFarPlane(const f32 fFarPlane)
{
    m_fFar = fFarPlane;
    BuildProjectionMatrix();
}

void CCamera::SetNearPlane(const f32 fNearPlane)
{
    m_fNear = fNearPlane;
    BuildProjectionMatrix();
}

void CCamera::SetViewport(f32 fWidth, f32 fHeight, bool bPerspective /*= true*/)
{
	SetViewport( m_fNear, m_fFar, m_fFOV, fWidth, fHeight, bPerspective );
}



void CCamera::Roll(f32 fAngle)
{
    Quaternion qRotate;
    qRotate.FromAngleAxis(fAngle, Vector3Df::UNIT_Z);
    //m_vPosition = qRotate * m_vPosition;
    m_vLookAt = qRotate * m_vLookAt;
}

void CCamera::Pitch(f32 fDegree)
{
    Quaternion qRotate;
    Vector3Df vRight;
    //vRight = m_vLook.CrossProduct(Vector3Df)
    qRotate.FromAngleAxis(fDegree, m_vRight);
    //m_vPosition = qRotate * m_vPosition;
    m_vLook = qRotate * m_vLook;
    m_vLook.Normalize();
    m_vLookAt = m_vPosition + m_vLook * m_fDistance;
}

void CCamera::Yaw(f32 fDegree)
{
    Quaternion qRotate;
    qRotate.FromAngleAxis(fDegree, Vector3Df::UNIT_Y);
    //m_vPosition = qRotate * m_vPosition;
    m_vLook = qRotate * m_vLook;
    m_vLook.Normalize();
    m_vLookAt = m_vPosition + m_vLook * m_fDistance;
}

void CCamera::PitchYawRoll(f32 fAnglePitch, f32 fAngleYaw, f32 fAngleRoll)
{

}

void CCamera::GetDirection(Vector3Df& up, Vector3Df& right, Vector3Df& look) const
{
    up = m_vUp;
    right = m_vRight;
    look = m_vLook;
}

//实际上应该在这里实时计算比较准确
void CCamera::GetViewMatrix(Matrix4f* mat) const
{
    *mat = m_matView;
}

void CCamera::GetProjectionMatrix(Matrix4f* mat) const
{
    *mat = m_matProject;
}



/*
void CCamera::SetDirection(const Vector3Df& up, const Vector3Df& right, const Vector3Df& look)
{
    m_vUp = up;
    m_vRight = right;
    m_vLook = look;
}
*/

void CCamera::Reset(const Vector3Df& up, const Vector3Df& right, const Vector3Df& look, const Vector3Df& position, 
                    f32 fNear, f32 fFar, f32 fFov, f32 fWidth, f32 fHeight)
{
    m_vUp = up;
    m_vRight = right;
    m_vLook = look;
    m_vPosition = position;
    m_fNear = fNear;
    m_fFar = fFar;
    m_fFOV = fFov;
    m_fViewWidth = fWidth;
    m_fViewHeight = fHeight;
//#ifdef LEFT_HAND_COORDINATE
//    m_matView.BuildCameraLookAtMatrixLHByDirection(m_vLook, m_vRight, m_vUp, m_vPosition);
//#else
//    m_matView.BuildCameraLookAtMatrixRHByDirection(m_vLook, m_vRight, m_vUp, m_vPosition);
//#endif
    BuildViewMatrix();
    
    m_frustum.SetPerspective(m_fNear, m_fFar, m_fFOV, m_fViewWidth, m_fViewHeight);
    m_frustum.CalculateFrustum(m_matView);

	UpdateUserClipFrustum();
}

void CCamera::Test()
{
    Vector3Df up(0.0f, 1.000003f, 0.0f);
    Vector3Df right(-0.554465f, 0.0f, 0.832207f);
    Vector3Df look(-0.832208f, 0.0f, -0.554466f);

    Matrix4f mat;
    mat.BuildRotationMatrixYDegrees(0.5f);

    mat.BuildRotationMatrixByAxisDegrees(right, 0.5f);
    //D3DXMatrixRotationX(&mat, fAngle);

    mat.TransformCoord(up, up);
    mat.TransformCoord(look, look);
    //mat.TransformVect(up, up);
    //mat.TransformVect(look, look);

    look.Normalize();
    up = look.CrossProduct(right);
    up.Normalize();
    right = up.CrossProduct(look);
    right.Normalize();

    int a = 0;
}

Vector3Df CCamera::GetPickRay(int nScreenX, int nScreenY)
{
    Vector3Df vOut;
    float dx,dy;
    //D3DMATRIX invMatrix,viewMatrix;
    float fApsect = m_fViewWidth / m_fViewHeight;
    dx = tanf(m_fFOV * 0.5f) * (nScreenX * 0.5f / m_fViewWidth - 1.0f) / fApsect;
    dy = tanf(m_fFOV * 0.5f) * (1.0f - nScreenY * 0.5f / m_fViewHeight);

    //获取世界矩阵和观察矩阵的逆
    //lpDevice->GetTransform(D3DTRANSFORMSTATE_VIEW,&viewMatrix);
    //D3DMath_MatrixInvert(invMatrix,viewMatrix);
    Matrix4f matInv = m_matView;
    matInv.Inverse();

    //通过与逆矩阵的相乘我们得到空间中两个点的实际坐标
    Vector3Df vP1(dx * m_fNear, dy * m_fNear, m_fNear);
    Vector3Df vP2(dx * m_fFar, dy * m_fFar, m_fFar);
    matInv.TransformCoord(vP1, vP1);
    matInv.TransformCoord(vP2, vP2);
    vOut = vP1 - vP2;
    vOut.Normalize();
    return vOut;
}

void CCamera::GetRay(int nScreenX, int nScreenY, Vector3Df& vOutP1, Vector3Df& vOutP2)
{
    Matrix4f matInv = m_matView;
    matInv.Inverse();
    if (m_bPerspective)
    {
        float dx,dy;
        //D3DMATRIX invMatrix,viewMatrix;
        float fApsect = m_fViewWidth / m_fViewHeight;
        dx = tanf(m_fFOV * 0.5f) * (2.0f * nScreenX - m_fViewWidth) / m_fViewHeight;
        dy = tanf(m_fFOV * 0.5f) * (1.0f - 2.0f * nScreenY / m_fViewHeight);

        //获取世界矩阵和观察矩阵的逆
        //lpDevice->GetTransform(D3DTRANSFORMSTATE_VIEW,&viewMatrix);
        //D3DMath_MatrixInvert(invMatrix,viewMatrix);
        
        //通过与逆矩阵的相乘我们得到空间中两个点的实际坐标
        Vector3Df vP1(dx * m_fNear, dy * m_fNear, m_fNear);
        Vector3Df vP2(dx * m_fFar, dy * m_fFar, m_fFar);
        matInv.TransformCoord(vP1, vOutP1);
        matInv.TransformCoord(vP2, vOutP2);
    }
    else
    {
        Vector3Df vP1(nScreenX -  m_fViewWidth / 2, m_fViewHeight / 2 - nScreenY, m_fNear);
        Vector3Df vP2(nScreenX -  m_fViewWidth / 2, m_fViewHeight / 2 - nScreenY, m_fFar);
        matInv.TransformCoord(vP1, vOutP1);
        matInv.TransformCoord(vP2, vOutP2);
    }
}


void CCamera::GetRay(int nScreenX, int nScreenY, AIR::Ray& ray )
{
	Vector3Df vn, vf, vdir;
	GetRay(nScreenX, nScreenY, vn, vf);
	ray.m_Origin = vn;
	vdir = vf-vn;
	ray.m_Range = vdir.GetLength();
	ray.m_Dir = vdir.Normalize();
}


Vector3Df CCamera::GetXZPlanePosition( int nScreenX, int nScreenY )
{
	Plane3Df p( Vector3Df(0.0f,0.0f,0.0f), Vector3Df(0.0f, 1.0f, 0.0f) );
	return GetPlanePosition( nScreenX, nScreenY, p );
}


Vector3Df CCamera::GetPlanePosition( int nScreenX, int nScreenY, const Plane3Df& plane )
{
	Vector3Df vStart, vEnd, vRay, vInterset;
	GetRay( nScreenX, nScreenY, vStart, vEnd );
	vRay = vEnd - vStart;
	vRay.Normalize();
	plane.GetIntersectionWithLine( vStart, vRay, vInterset );
	return vInterset;
}


void CCamera::Translate3Dto2D(const Vector3Df& vPos, int& nScreenX, int& nScreenY) const
{
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();

    Matrix4f matWVP = m_matView * m_matProject;

    float fX,fY,fZ,fW;

    fX = (matWVP[0] * vPos.x) + (matWVP[4] * vPos.y) + (matWVP[8] * vPos.z) + matWVP[12];

    fY = (matWVP[1] * vPos.x) + (matWVP[5] * vPos.y) + (matWVP[9] * vPos.z) + matWVP[13];

    fZ = (matWVP[2] * vPos.x) + (matWVP[6] * vPos.y) + (matWVP[10] * vPos.z) + matWVP[14];

    fW = (matWVP[3] * vPos.x) + (matWVP[7] * vPos.y) + (matWVP[11] * vPos.z) + matWVP[15];

    float fWpInv = 1.0f / fW;

    //视口大小一半
    float fClip_x = (float)((int)m_fViewWidth  >> 1);
    float fClip_y = (float)((int)m_fViewHeight >> 1);

    //投影坐标范围为[-1,1]要变成[0,2]并且屏幕坐标y方向向下,需要反转

    nScreenX = (u32)( (1.0f + (fX * fWpInv)) * fClip_x );
    nScreenY = (u32)( ((fY * fWpInv) - 1.0f ) * (-fClip_y) );
}

void CCamera::SetLookAt(const Vector3Df& vPos)
{
    m_vLookAt = vPos;
    m_vLook = m_vLookAt - m_vPosition;
    m_fDistance = m_vLook.GetLength();
    m_vLook.Normalize();
}

void CCamera::GetRight(Vector3Df& vRight)
{
    vRight = m_vRight;
}

void CCamera::GetUp(Vector3Df& vUp)
{
    vUp = m_vUp;
}


void CCamera::UpdateUserClipFrustum( void )
{
	/*if (m_bPerspective)
	{
		m_UserClipProject.BuildProjectionMatrixPerspectiveFovLH( m_fFOV, m_fViewWidth / m_fViewHeight, m_fNear, m_UserClipViewFar );
		m_frustum.SetPerspectiveLH( m_fNear, m_UserClipViewFar, m_fFOV, m_fViewWidth, m_fViewHeight );
	}
	else 
	{
		m_UserClipProject.BuildProjectionMatrixOrthoLH( m_fViewWidth, m_fViewHeight, m_fNear, m_UserClipViewFar );
		m_frustum.SetOrthoLH( m_fViewWidth, m_fViewHeight, m_fNear, m_UserClipViewFar );
	}*/
    BuildUserClipProjectionMatrix();
	m_frustum.CalculateFrustum( m_matView );
}

void CCamera::Rotate(f32 fVertRadian, f32 fHorzRadian )
{
    /*
    Quaternion qRotateH;
    qRotateH.FromAngleAxis( fHorzRadian, Vector3Df::UNIT_Y );
    // 水平旋转摄像机视向量角度 fHorzRadian
    m_vLook = qRotateH * m_vLook;
    // 计算摄像机的右手向量
    m_vRight = Vector3Df::UNIT_Y.CrossProduct(m_vLook);
    m_vRight.Normalize();

    Quaternion qRotateV;

    float fMinAngle = DEGTORAD * 10.0f;
    float fMaxAngle = PI - fMinAngle;
    float fPreAngle = m_fAngel - fVertRadian;
    if ( fPreAngle > fMaxAngle )
    {
        fVertRadian = m_fAngel - fMaxAngle;
    }
    else if ( fPreAngle < fMinAngle )
    {
        fVertRadian = m_fAngel - fMinAngle;
    }
    // 垂直旋转摄像机视向量角度 fVertRadian
    qRotateV.FromAngleAxis( fVertRadian, m_vRight );
    m_vLook = qRotateV * m_vLook;
    m_vLook.Normalize();
    // 通过新的视向量计算摄像机的坐标
    m_vPosition = m_vLookAt - m_vLook * m_fDistance;
    // 计算摄像机的上方向向量
    m_vUp = m_vLook.CrossProduct(m_vRight);
    //m_vUp = Vector3Df::UNIT_Y;
    // 计算视图矩阵
    m_matView.BuildCameraLookAtMatrixLH(m_vPosition, m_vLookAt, m_vUp);
    // 计算视锥体
    m_frustum.CalculateFrustum(m_matView);
    */
}

void CCamera::BuildProjectionMatrix()
{
    if (m_bPerspective)
    {
#ifdef LEFT_HAND_COORDINATE
        m_matProject.BuildProjectionMatrixPerspectiveFovLH(m_fFOV, m_fViewWidth / m_fViewHeight, m_fNear, m_fFar);
#else
        m_matProject.BuildProjectionMatrixPerspectiveFovRH(m_fFOV, m_fViewWidth / m_fViewHeight, m_fNear, m_fFar);
#endif

        m_frustum.SetPerspective(m_fNear, m_fFar, m_fFOV, m_fViewWidth, m_fViewHeight);
    }
    else 
    {
#ifdef LEFT_HAND_COORDINATE
        m_matProject.BuildProjectionMatrixOrthoLH(m_fViewWidth, m_fViewHeight, m_fNear, m_fFar);
#else
        m_matProject.BuildProjectionMatrixOrthoRH(m_fViewWidth, m_fViewHeight, m_fNear, m_fFar);
#endif

        m_frustum.SetOrtho(m_fViewWidth, m_fViewHeight, m_fNear, m_fFar);
    }

}

void CCamera::BuildUserClipProjectionMatrix()
{
    if (m_bPerspective)
    {
#ifdef LEFT_HAND_COORDINATE
        m_UserClipProject.BuildProjectionMatrixPerspectiveFovLH( m_fFOV, m_fViewWidth / m_fViewHeight, m_fNear, m_UserClipViewFar );
#else
        m_UserClipProject.BuildProjectionMatrixPerspectiveFovRH( m_fFOV, m_fViewWidth / m_fViewHeight, m_fNear, m_UserClipViewFar );
#endif
        
        m_frustum.SetPerspective( m_fNear, m_UserClipViewFar, m_fFOV, m_fViewWidth, m_fViewHeight );
    }
    else 
    {
#ifdef LEFT_HAND_COORDINATE
        m_UserClipProject.BuildProjectionMatrixOrthoLH( m_fViewWidth, m_fViewHeight, m_fNear, m_UserClipViewFar );
#else
        m_UserClipProject.BuildProjectionMatrixOrthoRH( m_fViewWidth, m_fViewHeight, m_fNear, m_UserClipViewFar );
#endif
        m_frustum.SetOrtho( m_fViewWidth, m_fViewHeight, m_fNear, m_UserClipViewFar );
    }
}

void CCamera::BuildViewMatrix()
{
    
    m_vLook.Normalize();

    if (m_vLook == Vector3Df::NEGATIVE_UNIT_Y)
    {
        m_vRight = Vector3Df::UNIT_X;
        m_vUp = Vector3Df::UNIT_Z;
    }
    else
    {
        m_vRight = Vector3Df::UNIT_Y.CrossProduct(m_vLook);
        m_vRight.Normalize();
        m_vUp = m_vLook.CrossProduct(m_vRight);
    }

    BuildViewMatrixImpl();
}

void CCamera::BuildViewMatrixImpl()
{
#ifdef LEFT_HAND_COORDINATE
    m_matView.BuildCameraLookAtMatrixLH(m_vPosition, m_vLookAt, m_vUp);
#else
    m_matView.BuildCameraLookAtMatrixRH(m_vPosition, m_vLookAt, m_vUp);
#endif
}

bool CCamera::IsInDistance(const Vector3Df& worldPos, f32 fRadius) const
{
    return ((m_vPosition - worldPos).GetLength() - fRadius) < m_UserClipViewFar;
}

bool CCamera::IsInDistance(const Vector3Df& worldPos, f32 fRadius, f32 fDistanceIn)
{
    return ((m_vPosition - worldPos).GetLength() - fRadius) < fDistanceIn;
}

void CCamera::CaculateFrustum()
{
    m_frustum.CalculateFrustum(m_matView);
}
