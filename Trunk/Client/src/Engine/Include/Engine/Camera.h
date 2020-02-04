// ***************************************************************
//  Camera   version:  1.0   ��  date: 01/01/2010
//  -------------------------------------------------------------
//  Author    liangairan
//  -------------------------------------------------------------
//  Copyright (C) 2010 - All Rights Reserved
// ***************************************************************
// �������
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

    //�����������λ��
    void SetPosition(const Vector3Df& vPos);

	//����������λ��
    void GetPosition(Vector3Df& vPos) const;

	//����������λ��
    const Vector3Df& GetPosition( void ) const
    {
        return m_vPosition;
    }

    //����������ӵ��λ��
    void SetLookAt(const Vector3Df& vPos);

	//���������
    const Vector3Df& GetLookDir() const
    {
        return m_vLook;
    }

	//����ӵ�����
    const Vector3Df& GetLookAt() const
    {
        return m_vLookAt;
    }

    //�����������viewport�Ĳ���
    //@fNear    ������
    //@fFar     Զ����
    //@fFOV     ��׶��Ƕȣ���λ����
    //@fWidth   viewport�Ŀ�
    //@fHeight  viewport�ĸ�
    //@bPerspective  �Ƿ�Ϊ͸��ͶӰ
    void SetViewport(f32 fNear, f32 fFar, f32 fFov, f32 fWidth, f32 fHeight, bool bPerspective = true);

	//�����������viewport����
	void SetViewport(f32 fWidth, f32 fHeight, bool bPerspective = true);

    //����������������
	//up - ������Ϸ�������
	//right - ������ҷ�������
	//look - �����������
    void GetDirection(Vector3Df& up, Vector3Df& right, Vector3Df& look) const;

    //�����������ҷ���
    void GetRight(Vector3Df& vRight);

	//�����������ҷ���
	const Vector3Df& GetRight( void ) const
	{
		return m_vRight;
	}

	//����������ҷ�������
	void SetRight(const Vector3Df& vRight)
	{
		m_vRight = vRight;
	}

    //�����������Ϸ���
    void GetUp(Vector3Df& vUp);

	//�����������Ϸ���
	const Vector3Df& GetUp( void ) const
	{
		return m_vUp;
	}

	//������������Ϸ���
	void SetUp(const Vector3Df& vUp)
	{
		m_vUp = vUp;
	}

    //�������������
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

	//������������ͼ����
    void GetViewMatrix(Matrix4f* mat) const;

	//����������ͶӰ����
    void GetProjectionMatrix(Matrix4f* mat) const;

	//������������ͼ����
    inline const Matrix4f& GetViewMatrix() const
    {
        return m_matView;
    }

	//����������ͶӰ����
    inline const Matrix4f& GetProjectionMatrix() const
    {
        return m_matProject;
    }

    //���������
    virtual void Update();

	//--------------------------------------------------
	//! add by hjj 2010-6-4
	//�������������(������ʱ�õ�)
	void BuildReflectCamara( const Plane3Df& plane, CCamera* pDstCamera );

	//�����������λ���Լ��ӵ�֮��,����������Ĳ���
	void RecalculateByPositionAndLookAt( void );

	//�����Ƿ���Ҫ���������
	inline void SetNeedAutoUpdate( bool bNeedAutoUpdate )
	{
		m_bNeedAutoUpdate = bNeedAutoUpdate;
	}

	//��õ�ǰ�Ƿ���Ҫ���������
	inline bool GetNeedAutoUpdate( void )
	{
		return m_bNeedAutoUpdate;
	}
	//--------------------------------------------------

    //��ý�����
    inline f32 GetViewNear() const 
	{ 
		return m_fNear; 
	}

    //���Զ����
    inline f32 GetViewFar() const 
	{ 
		return m_fFar; 
	}

	//����ӿڵĿ��
    inline f32 GetViewWidth() const 
	{ 
		return m_fViewWidth; 
	}

	//����ӿڵĸ߶�
    inline f32 GetViewHeight() const 
	{ 
		return m_fViewHeight; 
	}

	//��ù۲�Ƕ�
    inline f32 GetViewFOV() const 
	{ 
		return m_fFOV; 
	}

	//����ĳ�㵽������ľ���
    inline f32 GetDistanceFrom(const Vector3Df& vPos) const 
    {
        Vector3Df vDir = m_vPosition - vPos;
        return vDir.GetLength();
    }

	//����Զ����
    void SetFarPlane(const f32 fFarPlane);

	//���ý�����
    void SetNearPlane(const f32 fNearPlane);

    //�����������ռ������
    Vector3Df GetPickRay(int nScreenX, int nScreenY);

    //vOutP1������������
    //vOutP2��Զ��������
    void GetRay(int nScreenX, int nScreenY, Vector3Df& vOutP1, Vector3Df& vOutP2);

	void GetRay(int nScreenX, int nScreenY, AIR::Ray& ray ); 

	//�ж�aabb��Χ���Ƿ�ɼ�
    virtual bool IsVisable(const AABBBox3Df& box) const;

    //����ü�
    virtual bool IsVisable(const Vector3Df& center, f32 fRadius) const;

    //�жϾ����Ƿ�ɼ�,��Ҫ������2Dͼ�ε��ж�
	inline virtual bool IsVisable(const Rectf& rect) const
	{
		return false;
	}

	//�����׶��
    const CFrustum* GetFrustum() const { return &m_frustum; } 

    //��3D����ת��2D����
    //@vPos     in   3D�ռ�����
    //@nScreenX out  ��Ļ����X
    //@nScreenY out  ��Ļ����Y
    void Translate3Dto2D(const Vector3Df& vPos, int& nScreenX, int& nScreenY) const;

	//�����������ӵ�ľ���
    inline virtual float GetDistance() const 
    {
        return (m_vPosition - m_vLookAt).GetLength();
    }

	//������Ļ����,�������߻��е�XZƽ�������
	Vector3Df GetXZPlanePosition( int nScreenX, int nScreenY );

	Vector3Df GetPlanePosition( int nScreenX, int nScreenY, const Plane3Df& plane );

	//���������X����ת
    void Pitch(f32 fDegree); 

	//���������Y����ת
    void Yaw(f32 fDegree);      

	//���������Z����ת
    void Roll(f32 fDegree);     

	//��ǰ�������
    void Walk(f32 fSpeed);     

	//������������
    void Strafe(f32 fSpeed);   

	//�����½�
    void RaiseLow(f32 fSpeed);

    //��þ���billboard��ľ���
    void GetBillboardTransform(Matrix4f& matWorld);

	//��õ�ǰ�Ƿ�͸��ͶӰ
    inline bool GetIsPerspectProj( void )
    {
        return m_bPerspective;
    }

	//--------------------------------------------------
	//! add by hjj 2010-7-28
	//�����û��ü���Զ����
	inline void SetUserClipViewFar( f32 fUserClipViewFar )
	{
		m_UserClipViewFar = fUserClipViewFar;
		//UpdateUserClipFrustum();
	}

	//����û��ü���Զ����
	inline f32 GetUserClipViewFar( void )
	{
		return m_UserClipViewFar;
	}

    inline void SetUserFrustumEnable(bool bEnable)
    {
        m_bUserClipEnable = bEnable;
    }

	//����û��ü���׶��
	//inline const CFrustum*		GetUserClipFrustum() const 
	//{ 
	//	return &m_UserClipFrustum; 
	//} 
	//--------------------------------------------------

    //************************************
    // Method:    Rotate
    // Returns:   void
    // Qualifier: ��ת�����
    // Parameter: f32 fVertRadian : ��ֱ��ת�Ļ��Ƚ�
    // Parameter: f32 fHorzRadian : ˮƽ��ת�Ļ��Ƚ�
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

    //����ͶӰ����
    virtual void BuildProjectionMatrix();

    void BuildViewMatrix();

    void CaculateFrustum();
    //void SetLookDir(const Vector3Df& dir);
protected:
    //����
    void						Fly(f32 fSpeed);

	//��ת�����
    void						PitchYawRoll(f32 fAnglePitch, f32 fAngleYaw, f32 fAngleRoll);
    
	//���¹�������
    virtual void				UpdateBillBoard();

	//�����û��ü���׶��
	void						UpdateUserClipFrustum( void );

    void BuildViewMatrixImpl();

    //�����Լ�����ͶӰ����
    void BuildUserClipProjectionMatrix();
protected:
    Vector3Df	m_vUp;				//�����Y����
    Vector3Df	m_vRight;			//�����X����
    Vector3Df	m_vLook;			//�����Z����
    Vector3Df	m_vPosition;		//�����λ��
    f32			m_fNear;			//������
    f32			m_fFar;				//Զ����
    f32			m_fFOV;				//��׶�н�
    f32			m_fViewWidth;		//�ӿڿ�
    f32			m_fViewHeight;		//�ӿڸ�
    Vector3Df	m_vLookAt;			//���������λ��
    Quaternion	m_oriention;		//��Ԫ����ת
    f32			m_fMoveSpeed;       //������ƶ��ٶ�
    f32			m_fRotationSpeed;   //��ת�ٶ�
    f32			m_fDistance;		//�ӵ㵽������ľ���    
    bool		m_bPerspective;		//�Ƿ�Ϊ͸��ͶӰ  
    Matrix4f	m_matView;			//��ͼ����
    Matrix4f	m_matProject;		//ͶӰ����
    Matrix4f	m_matBillboard;		//�����������billboard����
    CFrustum	m_frustum;			//Ĭ�ϵ���׶��
	bool		m_bNeedAutoUpdate;	//��ǰ�Ƿ���Ҫ���������

	f32			m_UserClipViewFar;	//�û��޳�Զ�ü���
	Matrix4f	m_UserClipProject;	//�û��޳�ͶӰ����
	CFrustum	m_UserClipFrustum;	//�û��޳���׶��

    bool        m_bUserClipEnable;  //�Ƿ�ʹ��userclip
private:

    

    void Test();
};
