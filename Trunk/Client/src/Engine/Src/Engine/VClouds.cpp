#include "stdafx.h"
#include "VClouds.h"



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
VClouds::VClouds( 
	ISceneManager *sm, 
	CCamera *c,
	const Vector2Df& Height, float Radius,
	float AlphaRadian,  float BetaRadian, 
    int NumberOfBlocks, int Na, int Nb, int Nc)
	: mSceneManager(sm)
	, mCamera(c)
	, mCreated(false)
	, mHeight(Height)
	, mAlpha(AlphaRadian)
	, mBeta(BetaRadian)
	, mRadius(Radius)
	, mNumberOfBlocks(NumberOfBlocks)
	, mNa(Na) , mNb(Nb) , mNc (Nc)
	, mWindDirection(0)
	, mWindSpeed(80.0f)
	, mWheater(Vector2Df(1.0f, 1.0f))
	, mNumberOfForcedUpdates(-1)
	, mSunDirection(Vector3Df(0,-1,0))
	, mSunColor(Vector3Df(1,1,1))
	, mAmbientColor(Vector3Df(0.63f,0.63f,0.7f))
	, mLightResponse(Vector4f(0.25f,0.2f,1.0f,0.1f))
	, mAmbientFactors(Vector4f(0.4f,1.0f,1.0f,1.0f))
	, mGlobalOpacity(1.0f)
	, mCloudFieldScale(1.0f)
	, mNoiseScale(4.2f)
	, mDataManager(0)
	, mGeometryManager(0)
{

}

/** Simple constructor
	@param sm Scene manager 
	@param c Camera
	@param Height x = Cloud field y-coord start, y: Field height (both in world coordinates)
	@param Radius Radius
 */
VClouds::VClouds(
	ISceneManager *sm, 
	CCamera *c,
	const Vector2Df& Height, 
	float Radius)
	: mSceneManager(sm)
	, mCamera(c)
	, mCreated(false)
	, mHeight(Height)
	, mAlpha(DegreeToRadian(12))
	, mBeta(DegreeToRadian(40))
	, mRadius(Radius)
	, mNumberOfBlocks(12)
	, mNa(10) , mNb(8) , mNc (6)
	, mWindDirection((0))
	, mWindSpeed(80.0f)
	, mWheater(Vector2Df(1.0f, 1.0f))
	, mNumberOfForcedUpdates(-1)
	, mSunDirection(Vector3Df(0,-1,0))
	, mSunColor(Vector3Df(1,1,1))
	, mAmbientColor(Vector3Df(0.63f,0.63f,0.7f))
	, mLightResponse(Vector4f(0.25f,0.2f,1.0f,0.1f))
	, mAmbientFactors(Vector4f(0.4f,1.0f,1.0f,1.0f))
	, mGlobalOpacity(1.0f)
	, mCloudFieldScale(1.0f)
	, mNoiseScale(4.2f)
	, mDataManager(0)
	, mGeometryManager(0)
{

}

/** Destructor
 */
VClouds::~VClouds()
{
	remove();
}

/** Create
 */
void VClouds::create()
{
	remove();

	// Data manager
	mDataManager = new DataManager(this);
	mDataManager->create(128,128,20);

	// Geometry manager
	mGeometryManager = new GeometryManager(this, mHeight, mRadius, mAlpha, mBeta, mNumberOfBlocks, mNa, mNb, mNc);
	mGeometryManager->create();

	//static_cast<Ogre::MaterialPtr>(
	//	Ogre::MaterialManager::getSingleton().getByName("SkyX_VolClouds"))
	//	->getTechnique(0)->getPass(0)->getVertexProgramParameters()->setNamedConstant("uRadius", mRadius);

	mCreated = true;

	// Update material parameters
	setSunColor(mSunColor);
	setAmbientColor(mAmbientColor);
	setLightResponse(mLightResponse);
	setAmbientFactors(mAmbientFactors);

	// Set current wheater
	int nforced = (mNumberOfForcedUpdates == -1) ? 2 : mNumberOfForcedUpdates;
	setWheater(mWheater.x, mWheater.y, nforced);
}

/** Update
    @param timeSinceLastFrame Time since last frame
 */
void VClouds::update( float timeSinceLastFrame)
{
	if (!mCreated)
	{
		return;
	}

	mDataManager->update(timeSinceLastFrame);
	mGeometryManager->update(timeSinceLastFrame);

	//static_cast<Ogre::MaterialPtr>(
	//	Ogre::MaterialManager::getSingleton().getByName("SkyX_VolClouds"))
	//	->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("uInterpolation", mDataManager->_getInterpolation());
	//static_cast<Ogre::MaterialPtr>(
	//	Ogre::MaterialManager::getSingleton().getByName("SkyX_VolClouds"))
	//	->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("uSunDirection", -mSunDirection);

}

/** Remove
 */
void VClouds::remove()
{
	if (!mCreated)
	{
		return;
	}

	delete mDataManager;
	delete mGeometryManager;

	mCreated = false;
}


/** Set sun color
    @param SunColor Sun color
 */
void VClouds::setSunColor(const Vector3Df& SunColor)
{
	mSunColor = SunColor;

	if (!mCreated)
	{
		return;
	}

	//static_cast<Ogre::MaterialPtr>(
	//	Ogre::MaterialManager::getSingleton().getByName("SkyX_VolClouds"))
	//	->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("uSunColor", mSunColor);

}



/** Set ambient color
    @param AmbientColor Ambient color
 */
void VClouds::setAmbientColor(const Vector3Df& AmbientColor)
{
	mAmbientColor = AmbientColor;

	if (!mCreated)
	{
		return;
	}

	//static_cast<Ogre::MaterialPtr>(
	//	Ogre::MaterialManager::getSingleton().getByName("SkyX_VolClouds"))
	//	->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("uAmbientColor", mAmbientColor);

}


/** Set light response
    @param LightResponse
		   x - Sun light power
		   y - Sun beta multiplier
		   z - Ambient color multiplier
		   w - Distance attenuation
 */
void VClouds::setLightResponse(const Vector4f& LightResponse)
{
	mLightResponse = LightResponse;

	if (!mCreated)
	{
		return;
	}

	//static_cast<Ogre::MaterialPtr>(
	//	Ogre::MaterialManager::getSingleton().getByName("SkyX_VolClouds"))
	//	->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("uLightResponse", mLightResponse);

}



/** Set ambient factors
    @param AmbientFactors x - constant, y - linear, z - cuadratic, w - cubic
 */
void VClouds::setAmbientFactors(const Vector4f& AmbientFactors)
{
	mAmbientFactors = AmbientFactors;

	if (!mCreated)
	{
		return;
	}

	//static_cast<Ogre::MaterialPtr>(
	//	Ogre::MaterialManager::getSingleton().getByName("SkyX_VolClouds"))
	//	->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("uAmbientFactors", mAmbientFactors);

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
void VClouds::setWheater( float Humidity,  float AverageCloudsSize,  int NumberOfForcedUpdates /*= 0*/)
{
	mWheater = Vector2Df(Humidity, AverageCloudsSize);
	mNumberOfForcedUpdates = NumberOfForcedUpdates;

	if (!mCreated)
	{
		return;
	}

	mDataManager->setWheater(mWheater.x, mWheater.y, mNumberOfForcedUpdates);

}
