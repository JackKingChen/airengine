// ***************************************************************
//  Camera   version:  1.0   ·  date: 01/01/2010
//  -------------------------------------------------------------
//  Author    liangairan
//  -------------------------------------------------------------
//  Copyright (C) 2010 - All Rights Reserved
// ***************************************************************
// 摄像机类
// ***************************************************************
#pragma once 
#include "Common.h"
#include "Vector3D.h"
#include "Matrix4.h"
#include "Dimension2D.h"
#include "Frustum.h"
#include "EngineConfig.h"
#include "Quaternion.h"
#include "Ray.h"
#include "MemoryAlloter.h"

class EngineExport CCamera// : public EngineAllocator(CCamera)
{
	EngineAllocator(CCamera)
public:
    CCamera();

    CCamera(const Vector3Df& vPosition, const Vector3Df& vLookAt);

    CCamera(const Vector3Df& vPosition, const Vector3Df& vTarget, f32 fNear, f32 fFar);

    CCamera(
		const Vector3Df& up, 
		const Vector3Df& right, 
		const Vector3Df& look, 
		const Vector3Df& position, 
        f32 fNear, 
		f32 fFar, 
		f32 fFov, 
		f32 fWidth, 
		f32 fHeight);

    virtual ~CCamera();

    //设置摄像机的位置
    void SetPosition(const Vector3Df& vPos);

	//获得摄像机的位置
    void GetPosition(Vector3Df& vPos) const;

	//获得摄像机的位置
    const Vector3Df& GetPosition( void ) const
    {
        return m_vPosition;
    }

    //设置摄像机视点的位置
    void SetLookAt(const Vector3Df& vPos);

	//获得视向量
    const Vector3Df& GetLookDir() const
    {
        return m_vLook;
    }

	//获得视点坐标
    const Vector3Df& GetLookAt() const
    {
        return m_vLookAt;
    }

    //设置摄像机的viewport的参数
    //@fNear    近截面
    //@fFar     远截面
    //@fFOV     视锥体角度，单位弧度
    //@fWidth   viewport的宽
    //@fHeight  viewport的高
    //@bPerspective  是否为透视投影
    void SetViewport(f32 fNear, f32 fFar, f32 fFov, f32 fWidth, f32 fHeight, bool bPerspective = true);

	//设置摄像机的viewport参数
	void SetViewport(f32 fWidth, f32 fHeight, bool bPerspective = true);

    //获得摄像机方向向量
	//up - 摄像机上方向向量
	//right - 摄像机右方向向量
	//look - 摄像机视向量
    void GetDirection(Vector3Df& up, Vector3Df& right, Vector3Df& look) const;

    //获得摄像机的右方向
    void GetRight(Vector3Df& vRight);

	//获得摄像机的右方向
	const Vector3Df& GetRight( void ) const
	{
		return m_vRight;
	}

	//设置摄像机右方向向量
	void SetRight(const Vector3Df& vRight)
	{
		m_vRight = vRight;
	}

    //获得摄像机的上方向
    void GetUp(Vector3Df& vUp);

	//获得摄像机的上方向
	const Vector3Df& GetUp( void ) const
	{
		return m_vUp;
	}

	//设置摄像机的上方向
	void SetUp(const Vector3Df& vUp)
	{
		m_vUp = vUp;
	}

    //重新设置摄像机
    virtual void Reset(
		const Vector3Df& up, 
		const Vector3Df& right, 
		const Vector3Df& look, 
		const Vector3Df& position, 
        f32 fNear, 
		f32 fFar, 
		f32 fFov, 
		f32 fWidth, 
		f32 fHeight);

	//获得摄像机的视图矩阵
    void GetViewMatrix(Matrix4f* mat) const;

	//获得摄像机的投影矩阵
    void GetProjectionMatrix(Matrix4f* mat) const;

	//获得摄像机的视图矩阵
    inline const Matrix4f& GetViewMatrix() const
    {
        return m_matView;
    }

	//获得摄像机的投影矩阵
    inline const Matrix4f& GetProjectionMatrix() const
    {
        return m_matProject;
    }

    //更新摄像机
    virtual void Update();

	//--------------------------------------------------
	//! add by hjj 2010-6-4
	//建立反射摄像机(做反射时用到)
	void BuildReflectCamara( const Plane3Df& plane, CCamera* pDstCamera );

	//设置摄像机的位置以及视点之后,更新摄像机的参数
	void RecalculateByPositionAndLookAt( void );

	//设置是否需要更新摄像机
	inline void SetNeedAutoUpdate( bool bNeedAutoUpdate )
	{
		m_bNeedAutoUpdate = bNeedAutoUpdate;
	}

	//获得当前是否需要更新摄像机
	inline bool GetNeedAutoUpdate( void )
	{
		return m_bNeedAutoUpdate;
	}
	//--------------------------------------------------

    //获得近截面
    inline f32 GetViewNear() const 
	{ 
		return m_fNear; 
	}

    //获得远截面
    inline f32 GetViewFar() const 
	{ 
		return m_fFar; 
	}

	//获得视口的宽度
    inline f32 GetViewWidth() const 
	{ 
		return m_fViewWidth; 
	}

	//获得视口的高度
    inline f32 GetViewHeight() const 
	{ 
		return m_fViewHeight; 
	}

	//获得观察角度
    inline f32 GetViewFOV() const 
	{ 
		return m_fFOV; 
	}

	//计算某点到摄像机的距离
    inline f32 GetDistanceFrom(const Vector3Df& vPos) const 
    {
        Vector3Df vDir = m_vPosition - vPos;
        return vDir.GetLength();
    }

	//设置远截面
    void SetFarPlane(const f32 fFarPlane);

	//设置近截面
    void SetNearPlane(const f32 fNearPlane);

    //获得世界坐标空间的射线
    Vector3Df GetPickRay(int nScreenX, int nScreenY);

    //vOutP1，近截面坐标
    //vOutP2，远截面坐标
    void GetRay(int nScreenX, int nScreenY, Vector3Df& vOutP1, Vector3Df& vOutP2);

	void GetRay(int nScreenX, int nScreenY, AIR::Ray& ray ); 

	//判断aabb包围盒是否可见
    virtual bool IsVisable(const AABBBox3Df& box) const;

    //球体裁剪
    virtual bool IsVisable(const Vector3Df& center, f32 fRadius) const;

    //判断矩形是否可见,主要是用于2D图形的判断
	inline virtual bool IsVisable(const Rectf& rect) const
	{
		return false;
	}

	//获得视锥体
    const CFrustum* GetFrustum() const { return &m_frustum; } 

    //把3D坐标转成2D坐标
    //@vPos     in   3D空间坐标
    //@nScreenX out  屏幕坐标X
    //@nScreenY out  屏幕坐标Y
    void Translate3Dto2D(const Vector3Df& vPos, int& nScreenX, int& nScreenY) const;

	//获得摄像机到视点的距离
    inline virtual float GetDistance() const 
    {
        return (m_vPosition - m_vLookAt).GetLength();
    }

	//给定屏幕坐标,计算射线击中的XZ平面的坐标
	Vector3Df GetXZPlanePosition( int nScreenX, int nScreenY );

	Vector3Df GetPlanePosition( int nScreenX, int nScreenY, const Plane3Df& plane );

	//绕摄像机的X轴旋转
    void Pitch(f32 fDegree); 

	//绕摄像机的Y轴旋转
    void Yaw(f32 fDegree);      

	//绕摄像机的Z轴旋转
    void Roll(f32 fDegree);     

	//向前向后行走
    void Walk(f32 fSpeed);     

	//向左向右行走
    void Strafe(f32 fSpeed);   

	//上升下降
    void RaiseLow(f32 fSpeed);

    //获得经过billboard后的矩阵
    void GetBillboardTransform(Matrix4f& matWorld);

	//获得当前是否透视投影
    inline bool GetIsPerspectProj( void )
    {
        return m_bPerspective;
    }

	//--------------------------------------------------
	//! add by hjj 2010-7-28
	//设置用户裁剪的远截面
	inline void SetUserClipViewFar( f32 fUserClipViewFar )
	{
		m_UserClipViewFar = fUserClipViewFar;
		//UpdateUserClipFrustum();
	}

	//获得用户裁剪的远截面
	inline f32 GetUserClipViewFar( void )
	{
		return m_UserClipViewFar;
	}

    inline void SetUserFrustumEnable(bool bEnable)
    {
        m_bUserClipEnable = bEnable;
    }

	//获得用户裁剪视锥体
	//inline const CFrustum*		GetUserClipFrustum() const 
	//{ 
	//	return &m_UserClipFrustum; 
	//} 
	//--------------------------------------------------

    //************************************
    // Method:    Rotate
    // Returns:   void
    // Qualifier: 旋转摄像机
    // Parameter: f32 fVertRadian : 垂直旋转的弧度角
    // Parameter: f32 fHorzRadian : 水平旋转的弧度角
    //************************************
    virtual void Rotate( f32 fVertRadian, f32 fHorzRadian );

    bool IsInDistance(const Vector3Df& worldPos, f32 fRadius) const;

    bool IsInDistance(const Vector3Df& worldPos, f32 fRadius, f32 fDistanceIn);

    inline bool IsUserClipEnable() const
    {
        return m_bUserClipEnable;
    }

    void SetViewMatrix(const Matrix4f& matView)
    {
        m_matView = matView;
    }

    void SetProjectionMatrix(const Matrix4f& matProj)
    {
        m_matProject = matProj;
    }

    //创建投影矩阵
    virtual void BuildProjectionMatrix();

    void BuildViewMatrix();

    void CaculateFrustum();
    //void SetLookDir(const Vector3Df& dir);
protected:
    //飞行
    void						Fly(f32 fSpeed);

	//旋转摄像机
    void						PitchYawRoll(f32 fAnglePitch, f32 fAngleYaw, f32 fAngleRoll);
    
	//更新公告板矩阵
    virtual void				UpdateBillBoard();

	//更新用户裁剪视锥体
	void						UpdateUserClipFrustum( void );

    void BuildViewMatrixImpl();

    //创建自己定义投影矩阵
    void BuildUserClipProjectionMatrix();
protected:
    Vector3Df	m_vUp;				//摄像机Y方向
    Vector3Df	m_vRight;			//摄像机X方向
    Vector3Df	m_vLook;			//摄像机Z方向
    Vector3Df	m_vPosition;		//摄像机位置
    f32			m_fNear;			//近截面
    f32			m_fFar;				//远截面
    f32			m_fFOV;				//视锥夹角
    f32			m_fViewWidth;		//视口宽
    f32			m_fViewHeight;		//视口高
    Vector3Df	m_vLookAt;			//摄像机看的位置
    Quaternion	m_oriention;		//四元数旋转
    f32			m_fMoveSpeed;       //照相机移动速度
    f32			m_fRotationSpeed;   //旋转速度
    f32			m_fDistance;		//视点到摄像机的距离    
    bool		m_bPerspective;		//是否为透视投影  
    Matrix4f	m_matView;			//视图矩阵
    Matrix4f	m_matProject;		//投影矩阵
    Matrix4f	m_matBillboard;		//面向摄像机的billboard矩阵
    CFrustum	m_frustum;			//默认的视锥体
	bool		m_bNeedAutoUpdate;	//当前是否需要更新摄像机

	f32			m_UserClipViewFar;	//用户剔除远裁剪面
	Matrix4f	m_UserClipProject;	//用户剔除投影矩阵
	CFrustum	m_UserClipFrustum;	//用户剔除视锥体

    bool        m_bUserClipEnable;  //是否使用userclip
private:

    

    void Test();
};
