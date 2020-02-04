#include "stdafx.h"
#include "GeometryManager.h"
#include "VClouds.h"


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
GeometryManager::GeometryManager(VClouds* vc,
	const Vector2Df& Height,  float Radius,
	float AlphaRadian, float BetaRadian, 
    int NumberOfBlocks, int Na, int Nb, int Nc)
	: mVClouds(vc)
	, mCreated(false)
	, mHeight(Height)
	, mRadius(Radius)
	, mAlpha(AlphaRadian)
	, mBeta(BetaRadian)
	, mPhi( TWO_PI / NumberOfBlocks )
	, mNumberOfBlocks(NumberOfBlocks)
	, mNa(Na) , mNb(Nb) , mNc (Nc)
	, mWorldOffset(Vector2Df(0,0))
	, mLastCameraPosition(Vector3Df(0,0,0))
{

}

/** Destructor
 */
GeometryManager::~GeometryManager()
{
	remove();
}

/** Create
 */
void GeometryManager::create()
{
	remove();

	//mSceneNode = mVClouds->getSceneManager()->getRootSceneNode()->createChildSceneNode();
	_createGeometry();

	mCreated = true;
}

/** Remove
 */
void GeometryManager::remove()
{
	if (!mCreated)
	{
		return;
	}

	//mSceneNode->detachAllObjects();
	//mSceneNode->getParentSceneNode()->removeAndDestroyChild(mSceneNode->getName());
	//mSceneNode = 0;

	for (int k = 0; k < mNumberOfBlocks; k++)
	{
		delete mGeometryBlocks.at(k);
		mGeometryBlocks.at(k) = 0;
	}

	mGeometryBlocks.clear();

	mCreated = false;
}

/** Call every frame
    @param timeSinceLastFrame Time since last frame
 */
void GeometryManager::update( float timeSinceLastFrame)
{
	if (!mCreated)
	{
		return;
	}

	mSceneNode->SetPosition(mVClouds->getCamera()->GetPosition().x, mHeight.x, mVClouds->getCamera()->GetPosition().z);

	_updateGeometry(timeSinceLastFrame);

	mLastCameraPosition = mVClouds->getCamera()->GetPosition();
}




/** Create geometry
 */
void GeometryManager::_createGeometry()
{
	for (int k = 0; k < mNumberOfBlocks; k++)
	{
		mGeometryBlocks.push_back(new GeometryBlock(mVClouds, mHeight.y, mAlpha, mBeta, mRadius, mPhi, mNa, mNb, mNc, k));
		mGeometryBlocks.at(k)->create();
		// Each geometry block must be in a different scene node, See: GeometryBlock::isInFrustum(Ogre::Camera *c)
		//Ogre::SceneNode *sn = mSceneNode->createChildSceneNode();
		//sn->attachObject(mGeometryBlocks.at(k)->getEntity());
	}
}

/** Update geometry
    @param timeSinceLastFrame Time since last frame
 */
void GeometryManager::_updateGeometry( float timeSinceLastFrame)
{
	// Calculate wind offset
	Vector2Df CameraDirection = Vector2Df(mVClouds->getCamera()->GetPosition().x, mVClouds->getCamera()->GetPosition().z);
	float offset = - CameraDirection.DotProduct(mVClouds->getWindDirectionV2()) * mVClouds->getWindSpeed() * timeSinceLastFrame;
	mWorldOffset += mVClouds->getWindDirectionV2() * mVClouds->getWindSpeed() * timeSinceLastFrame;

	// Calculate camera offset
	Vector2Df CameraOffset = Vector2Df(mVClouds->getCamera()->GetPosition().x - mLastCameraPosition.x, mVClouds->getCamera()->GetPosition().z - mLastCameraPosition.z);
	offset -= CameraOffset.DotProduct(CameraDirection);
	mWorldOffset += CameraOffset;

	for (int k = 0; k < mNumberOfBlocks; k++)
	{
		mGeometryBlocks.at(k)->update(offset);
		mGeometryBlocks.at(k)->setWorldOffset(mWorldOffset);
	}
}