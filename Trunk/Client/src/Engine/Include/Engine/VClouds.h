// ***************************************************************
//  文件: VClouds.h
//  日期: 2010-8-21
//  作者: huangjunjie
//  说明: 
// ***************************************************************


#pragma once

#ifndef __VClouds_H_
#define __VClouds_H_

#include "Common.h"
#include "DataManager.h"
#include "GeometryManager.h"


class EngineExport VClouds
{
public:
	/** Constructor
		@param sm Scene manager 
		@param c Camera
		@param Height x = Cloud field y-coord start, y: Field height (both in world coordinates)
		@param Radius Radius
		@param Alpha Alpha angle
		@param Beta Beta angle
		@param NumberOfBlocks Number of geometry blocks
		@param Na Number of slices in A zone
		@param Nb Number of slices in B zone
		@param Nc Number of slices in C zone
	 */
	VClouds( 
		ISceneManager *sm, 
		CCamera *c,
		const Vector2Df& Height, float Radius,
		float AlphaRadian, float BetaRadian, 
        int NumberOfBlocks,  int Na,  int Nb,  int Nc);

	/** Simple constructor
		@param sm Scene manager 
		@param c Camera
		@param Height x = Cloud field y-coord start, y: Field height (both in world coordinates)
		@param Radius Radius
	 */
	VClouds(
		ISceneManager *sm, 
		CCamera *c,
		const Vector2Df& Height, 
		float Radius);

	/** Destructor
	 */
	~VClouds();

	/** Create
	 */
	void create();

	/** Update
	    @param timeSinceLastFrame Time since last frame
     */
    void update( float timeSinceLastFrame);

	/** Remove
	 */
	void remove();

	/** Has been create() already called?
	    @return true if created() have been already called, false if not
	 */
	inline  bool isCreated() 
	{
		return mCreated;
	}

	/** Get height data
	    @return Height data: x = Altitude over the camera, y: Field height (both in world coordinates)
	 */
	inline const Vector2Df& getHeight() const
	{
		return mHeight;
	}

	/** Get radius
	    @return Radius
	 */
	inline float getRadius() 
	{
		return mRadius;
	}

	/** Set wind direction
	    @param WindDirection Wind direction
	 */
	inline void setWindDirection( float WindDirection)
	{
		mWindDirection = WindDirection;
	}

	/** Get wind direction
	    @return Wind direction
	 */
	inline  float getWindDirection() 
	{
		return mWindDirection;
	}

	/** Get wind direction as a Vector2
	    @return Wind direction
	 */
	inline  Vector2Df getWindDirectionV2() 
	{
		return Vector2Df( asm_cos(mWindDirection), asm_sin(mWindDirection) );
	}

	/** Set wind speed
	    @param WindSpeed Wind speed
	 */
	inline void setWindSpeed( float WindSpeed)
	{
		mWindSpeed = WindSpeed;
	}

	/** Get wind speed
	    @return Wind speed
	 */
	inline  float getWindSpeed() 
	{
		return mWindSpeed;
	}

	/** Set sun direction
	    @param SunDirection Sun direction
	 */
	inline void setSunDirection(const Vector3Df& SunDirection)
	{
		mSunDirection = SunDirection;
	}

	/** Get sun direction
	    @return Sun direction
	 */
	inline const Vector3Df& getSunDirection() const
	{
		return mSunDirection;
	}

	/** Set sun color
	    @param SunColor Sun color
	 */
	void setSunColor(const Vector3Df& SunColor);

	/** Get sun color
	    @return Sun color
	 */
	inline const Vector3Df& getSunColor() const
	{
		return mSunColor;
	}

	/** Set ambient color
	    @param AmbientColor Ambient color
	 */
	void setAmbientColor(const Vector3Df& AmbientColor);

	/** Get Ambient color
	    @return Ambient color
	 */
	inline const Vector3Df& getAmbientColor() const
	{
		return mAmbientColor;
	}

	/** Set light response
	    @param LightResponse
			   x - Sun light power
			   y - Sun beta multiplier
			   z - Ambient color multiplier
			   w - Distance attenuation
     */
	void setLightResponse(const Vector4f& LightResponse);

	/** Get light response
	    @return Light response
	 */
	inline const Vector4f& getLightResponse() const
	{
		return mLightResponse;
	}

	/** Set ambient factors
	    @param AmbientFactors x - constant, y - linear, z - cuadratic, w - cubic
     */
	void setAmbientFactors(const Vector4f& AmbientFactors);

	/** Get ambient factors
	    @return Ambient factors
	 */
	inline const Vector4f& getAmbientFactors() const
	{
		return mAmbientFactors;
	}

	/** Set global opacity
	    @param GlobalOpacity Global opacity: [0,1] range 0->Transparent cloud field
	 */
	inline void setGlobalOpacity( float GlobalOpacity)
	{
		mGlobalOpacity = GlobalOpacity;
	}

	/** Get global opacity
	    @return Global opacity
	 */
	inline  float getGlobalOpacity() 
	{
		return mGlobalOpacity;
	}

	/** Set cloud field scale
	    @param CloudFieldScale Cloud field scale
	 */
	inline void setCloudFieldScale( float CloudFieldScale)
	{
		mCloudFieldScale = CloudFieldScale;
	}

	/** Get cloud field scale
	    @return Cloud field scale
	 */
	inline  float getCloudFieldScale() 
	{
		return mCloudFieldScale;
	}

	/** Set noise scale
	    @param NoiseScale Noise scale
	 */
	inline void setNoiseScale( float NoiseScale)
	{
		mNoiseScale = NoiseScale;
	}

	/** Get noise scale
	    @return Noise scale
	 */
	inline  float getNoiseScale() 
	{
		return mNoiseScale;
	}

	/** Set wheater parameters
	    Use this funtion to update the cloud field parameters, you'll get a smart and smooth transition from your old 
		setting to your new ones.
		@param Humidity Humidity, in other words: the percentage of clouds in [0,1] range.
		@param AverageCloudsSize Average clouds size, for example: if previous wheater clouds size parameter was very different from new one(i.e: more little)
		       only the old biggest clouds are going to be keept and the little ones are going to be replaced
	    @param NumberOfForcedUpdates Number of times the data simulation are going to be re-calculated for the next frame.
		       This parameters is useful if you want to avoid a delayed response or, in other words, 0 means that you're going to get a smooth transition
			   between old and news wheater parameters(delayed response) and a positive number(2 might be sufficient) is going to change the clouds for the next frame
	 */
	void setWheater( float Humidity,  float AverageCloudsSize,  int NumberOfForcedUpdates = 0);

	/** Get scene manager
	    @return Ogre::SceneManager pointer
	 */
	inline ISceneManager* getSceneManager()
	{
		return mSceneManager;
	}

	/** Get camera
	    @return SkyX camera
	 */
	inline CCamera* getCamera()
	{
        return mCamera;
	}

	/** Get data manager
	    @return Data manager
	 */
	inline DataManager* getDataManager()
	{
		return mDataManager;
	}

	/** Get geometry manager
	    @return Geometry manager
	 */
	inline GeometryManager* getGeometryManager()
	{
		return mGeometryManager;
	}

private:

	/// Has been create() already called?
	bool mCreated;

	/// Height: x = Altitude over the camera, y: Field height (both in world coordinates)
	Vector2Df mHeight;
	/// Angles
	float mAlpha, mBeta;
	/// Radius
	float mRadius;
	/// Number of blocks
	int mNumberOfBlocks;
	/// Number of slices per geometry zone
	int mNa, mNb, mNc;

	/// Wind direction
	float mWindDirection;
	/// Wind speed
	float mWindSpeed;

	/// Wheater parameters: x = Humidity, y = Average clouds size, both un [0,1] range
	Vector2Df mWheater;
	/// Number of forced updates (This param is stored because of the user can call setWheater(...) before create() )
	int mNumberOfForcedUpdates;

	/// Sun direction
	Vector3Df mSunDirection;

	/// Sun color
	Vector3Df mSunColor;
	/// Ambient color
	Vector3Df mAmbientColor;

	/** Light response:
	    x - Sun light power
		y - Sun beta multiplier
	    z - Ambient color multiplier
	    w - Distance attenuation
     */
	Vector4f mLightResponse;
	/** Ambient factors
	    x - constant, y - linear, z - cuadratic, w - cubic
	 */
	Vector4f mAmbientFactors;

	/// Global opacity
	float mGlobalOpacity;

	/// Cloud field scale
	float mCloudFieldScale;
	/// Noise scale
	float mNoiseScale;

	/// Data manager
	DataManager *mDataManager;
	/// Geometry manager
	GeometryManager *mGeometryManager;

	/// Ogre::SceneManager pointer
    ISceneManager *mSceneManager;
	/// Main Ogre camera
	CCamera* mCamera;
}; //! end class VClouds



#endif //! end __VClouds_H_