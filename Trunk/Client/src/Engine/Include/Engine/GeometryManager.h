// ***************************************************************
//  文件: GeometryManager.h
//  日期: 2010-8-21
//  作者: huangjunjie
//  说明: 
// ***************************************************************


#pragma once

#ifndef __GeometryManager_H_
#define __GeometryManager_H_

#include "GeometryBlock.h"
#include "SceneNode.h"


class VClouds;

class EngineExport GeometryManager
{
public:
	/** Constructor
		@param vc VClouds pointer
		@param Height x = Altitude over the camera, y: Field height (both in world coordinates)
		@param Radius Radius
		@param Alpha Alpha angle
		@param Beta Beta angle
		@param NumberOfBlocks Number of geometry blocks
		@param Na Number of slices in A zone
		@param Nb Number of slices in B zone
		@param Nc Number of slices in C zone
	 */
	GeometryManager(VClouds* vc,
		const Vector2Df& Height,  float Radius,
		float AlphaRadian, float BetaRadian, 
        int NumberOfBlocks, int Na,  int Nb,  int Nc);

	/** Destructor
	 */
	~GeometryManager();

	/** Create
	 */
	void create();

	/** Remove
	 */
	void remove();

	/** Call every frame
	    @param timeSinceLastFrame Time since last frame
     */
    void update( float timeSinceLastFrame);

	/** Has been create() already called?
	    @return true if created() have been already called, false if not
	 */
	inline  bool isCreated() 
	{
		return mCreated;
	}

private:
	/** Create geometry
	 */
	void _createGeometry();

	/** Update geometry
	    @param timeSinceLastFrame Time since last frame
	 */
	void _updateGeometry( float timeSinceLastFrame);

	/// Has been create() already called?
	bool mCreated;

	/// Height: x = Altitude over the camera, y: Field height (both in world coordinates)
	Vector2Df mHeight;
	/// Angles
	float mAlpha, mBeta;
	/// Radius
	float mRadius;
	/// Azimutal angle per block
	float mPhi;
	/// Number of blocks
	int mNumberOfBlocks;
	/// Number of slices per geometry zone
	int mNa, mNb, mNc;

	/// World coords offset
	Vector2Df mWorldOffset;

	/// Geometry blocks
	AIRVector<GeometryBlock*> mGeometryBlocks;

	/// Scene node
	CSceneNode *mSceneNode;

	/// Last camera position
	Vector3Df mLastCameraPosition;

	/// VClouds pointer
	VClouds *mVClouds;
}; //! end class GeometryManager



#endif //! end __GeometryManager_H_