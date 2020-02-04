// ***************************************************************
//  文件: GeometryBlock.h
//  日期: 2010-8-20
//  作者: huangjunjie
//  说明: geometory class for weather system
// ***************************************************************

#pragma once

#ifndef __GeometryBlock_H__
#define __GeometryBlock_H__


class VClouds;

#include "EngineConfig.h"
#include "IVertexBuffer.h"
#include "IIndexBuffer.h"
//#include "Camera.h"
#include "Renderable.h"
#include "MaterialManager.h"
#include "Vector3D.h"

using namespace AIR;

class CCamera;

class EngineExport GeometryBlock
{
public:
	struct VERTEX
	{	      
		float x,y,z;		// Position
		float xc, yc, zc;	// 3D Coords
		float u, v;			// Noise coords
		float o;			// Opacity
	};

	//************************************
	// Method:    GeometryBlock
	// Returns:   none
	// Qualifier: constructor
	// Parameter: VClouds * vc - VClouds pointer
	// Parameter: const float & Height - world coordinates
	// Parameter: float AlphaRadian - Alpha angle
	// Parameter: float BetaRadian - Beta angle
	// Parameter: const float & Radius - Total radius
	// Parameter: float PhiRadian - Phi Actimutal angle
	// Parameter: const int & Na - Number of slices in A zone
	// Parameter: const int & Nb - Number of slices in B zone
	// Parameter: const int & Nc - Number of slices in C zone
	// Parameter: const int & Position - position
	//************************************
	GeometryBlock(
		VClouds *vc,
		float Height, float AlphaRadian, float BetaRadian, 
		float Radius, float PhiRadian, int Na, 
		int Nb, int Nc, int Position);


	//************************************
	// Method:    ~GeometryBlock
	// Returns:   none
	// Qualifier: destroctor
	//************************************
	~GeometryBlock();


	void create();

	void remove();

	//************************************
    // Method:    update
    // Returns:   void
    // Qualifier: Call every frame
    // Parameter: const float & offset - Amount of world units moved
    //************************************
    void update( float offset);

	//************************************
	// Method:    isCreated
	// Returns:   return true if created() have been already called, false if not
	// Qualifier: Has been create() already called?
	//************************************
	inline bool isCreated() 
	{
		return mCreated;
	}


	/** Get hardware vertex buffer reference
        @return Ogre::HardwareVertexBufferSharedPtr reference
     */
    inline IVertexBuffer* getHardwareVertexBuffer()
    {
        return mVertexBuffer;
    }

	/** Get hardware index buffer reference
	    @return Ogre::HardwareIndexBufferSharedPtr reference
	 */
	inline IIndexBuffer* getHardwareIndexBuffer()
	{
		return mIndexBuffer;
	}

	/** Set world offset
	    @param WorldOffset World offset
	 */
	inline void setWorldOffset(const Vector2Df& WorldOffset)
	{
		mWorldOffset = WorldOffset;
	}

	/** Is the geometry block inside the camera frustum?
	    @param c Camera
		@return true if yes, false if not
	 */
	bool isInFrustum( CCamera *c );
private:
	/** Build axis aligned box
	 */
	AABBBox3Df _buildAABox();
	/** Calculate data size
	 */
	void _calculateDataSize();
	/** Create geometry
	 */
	void _createGeometry();
	/** Update geometry
	 */
	void _updateGeometry();
	/** Update zone C slice
	    @param n Number of slice
	 */
	void _updateZoneCSlice( int n);
	
	/** Update zone B slice
	    @param n Number of slice
	 */
	void _updateZoneBSlice( int n);
	/** Update zone A slice
	    @param n Number of slice
	 */
	void _updateZoneASlice( int n);
	/** Set vertex data
		@param index Vertex index
		@param o Slice opacity
		@param p Position
	 */
	void _setVertexData( int index, const Vector3Df& p,  float o);
private:
	/// Has been create() already called?
	bool			mCreated;

    /// Vertex buffer
    IVertexBuffer*	mVertexBuffer;
    /// Index buffer
    IIndexBuffer*	mIndexBuffer;

	/// Vertices pointer
	VERTEX *		mVertices;

	/// Current number of triangles
	int				mNumberOfTriangles;
	/// Vertex count
	int				mVertexCount;

	/// Height
	float			mHeight;
	/// Angles
	float			mAlpha, mBeta;
	/// Radius
	float			mRadius;
	/// Acimutal angle
	float			mPhi;
	/// Number of slices per geometry zone
	int				mNa, mNb, mNc;
	/// A, B and C radius
	float			mA, mB, mC;
	/// Number of block(Position)
	int				mPosition;

	/// Precomputed Cos/Sin vectors
	Vector2Df		mV2Cos;
	Vector2Df		mV2Sin;
	/// PI - Beta, PI - Alpha Sin
	float			mBetaSin;
	float			mAlphaSin;

	/// Displacement
	Vector3Df		mDisplacement;
	/// World coords offset
	Vector2Df		mWorldOffset;

	/// VClouds pointer
	VClouds *		mVClouds;


	Renderable*	mRenderObject;


};//! end class GeometryBlock

#endif //! __GeometryBlock_H__