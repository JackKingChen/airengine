/********************************************************************
	created:	2009/10/18
	created:	18:10:2009   15:08
	filename: 	d:\My Codes\3DGraphic\Engine\src\Scene\ThirdPersonCamera.h
	file path:	d:\My Codes\3DGraphic\Engine\src\Scene
	file base:	ThirdPersonCamera
	file ext:	h
	author:		liangairan
	
	purpose:	第三人称固定视觉摄像机
*********************************************************************/

#pragma once

#include "Camera.h"
#include "SceneNode.h"
//#include "Actor.h"

class EngineExport CThirdPersonCamera : public CCamera
{
public:
    CThirdPersonCamera(const Vector3Df& vPos, const Vector3Df& vLookAt);
    CThirdPersonCamera(const Vector3Df& vPos, const Vector3Df& vLookAt, f32 fMaxDistance, f32 fMinDistance, CSceneNode* pTarget);
    virtual ~CThirdPersonCamera();

    //virtual void SetLookat(const Vector3Df& vLookAt);

    //旋转，绕（0，1,0）旋转
    virtual void	Rotate(const f32 fRadius);

	virtual void	MoveUpDown(const f32 fDy);

	virtual void	MoveNearFar(const f32 fdst);

    virtual void	Update();

    virtual void	SetMaxDistance(f32 fMaxDistance);

	virtual float   GetMaxDistance( void ) { return m_fMaxDistance; }

	virtual float   GetMinDistance( void ) { return m_fMinDistance; }

    virtual void	SetMinDistance(f32 fMinDistance);

    virtual void	Walk(f32 fSpeed); 

    virtual float	GetDistance() const;

    virtual void	SetTarget(CSceneNode* pTarget);

	virtual void	SetMoveSpeed( float speed );

	virtual float	GetMoveSpeed( void );

	virtual void	SetNeedTrack( bool bTrack );

	virtual bool	GetNeedTrack( void );

protected:
    float m_fDistance;    //焦点与摄像机的距离
    float m_fMaxDistance;
    float m_fMinDistance;

    CSceneNode* m_pTarget;

	float m_fMoveSpeed;
	bool  m_bNeedTrack;
	float m_fTrackDistance;
};

