#pragma once

#ifndef __Camera3DArcBall_H__
#define __Camera3DArcBall_H__

#include "Engine.h"
#include "Vector3D.h"
#include "ThirdPersonCamera.h"

class CThirdPersonCamera;

class EngineExport Camera3DArcBall : public CThirdPersonCamera
{
public:
	// ctor
	Camera3DArcBall(const Vector3Df& vPos, const Vector3Df& vLookAt);
	Camera3DArcBall(const Vector3Df& vPos, const Vector3Df& vLookAt, f32 fMaxDistance, f32 fMinDistance, CSceneNode* pTarget);

	// dtor
	~Camera3DArcBall( void );

	void    Update( void );

    virtual void Rotate( f32 fVertRadian, f32 fHorzRadian );

    void	MoveUpDown( bool bUp );

    void	MoveNearFar(const f32 fdst);

    void    AddNFAccelerate( bool bForward );

	void	ResetCameraToLookAtCenterPoint( void );

protected:
    float       m_fCurMoveNFSpeed;
    float       m_fMaxMoveNFSpeed;
    float       m_fDelta;

    float       m_fAngel;
private:

};//! end class Camera3DArcBall


#endif //! __Camera3DArcBall_H__
