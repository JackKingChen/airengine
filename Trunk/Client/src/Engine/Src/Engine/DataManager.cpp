#include "stdafx.h"
#include "DataManager.h"
#include "Ellipsoid.h"
#include "VClouds.h"



	/** Constructor
	    @param vc VClouds parent pointer
	 */
DataManager::DataManager(VClouds *vc)
		: mVClouds(vc)
		, mCellsCurrent(0)
		, mCellsTmp(0)
		, mFFRandom(0)
		, mNx(0), mNy(0), mNz(0)
		, mCurrentTransition(0)
		, mUpdateTime(10.0f)
		, mMaxNumberOfClouds(250)
		, mVolTexToUpdate(true)
		, mCreated(false)
{
	//for (int k = 0; k < 2; k++)
	//{
	//	mVolTextures[k].setNull();
	//}
}

/** Destructor
 */
DataManager::~DataManager()
{
	remove();
}

/** Create
    @param nx X complexity
	@param ny Y complexity
	@param nz Z complexity
 */
void DataManager::create( int nx,  int ny,  int nz)
{
	remove();

	mNx = nx; mNy = ny; mNz = nz;

	mFFRandom = new FastFakeRandom(1024, 0, 1);

	_initData(nx, ny, nz);

	for (int k = 0; k < 2; k++)
	{
		_createVolTexture(static_cast<VolTextureId>(k), nx, ny, nz);
	}

/*	setWheater(1, 1);

	_performCalculations(nx, ny, nz);
	_updateVolTextureData(mCellsCurrent, VOL_TEX0, mNx, mNy, mNz);

	_performCalculations(nx, ny, nz);
	_updateVolTextureData(mCellsCurrent, VOL_TEX1, mNx, mNy, mNz); */

	mCreated = true;
}

/** Update
	@param timeSinceLastFrame Time elapsed since last frame
 */
void DataManager::update( float timeSinceLastFrame)
{
	if (mVolTexToUpdate)
	{
		mCurrentTransition += timeSinceLastFrame;

		if (mCurrentTransition > mUpdateTime)
		{
			_performCalculations(mNx, mNy, mNz);
			_updateVolTextureData(mCellsCurrent, VOL_TEX0, mNx, mNy, mNz);

			mCurrentTransition = mUpdateTime;
			mVolTexToUpdate = !mVolTexToUpdate;
		}
	}
	else
	{
		mCurrentTransition -= timeSinceLastFrame;

		if (mCurrentTransition < 0)
		{
			_performCalculations(mNx, mNy, mNz);
			_updateVolTextureData(mCellsCurrent, VOL_TEX1, mNx, mNy, mNz);

			mCurrentTransition = 0;
			mVolTexToUpdate = !mVolTexToUpdate;
		}
	}

}

/** Remove
 */
void DataManager::remove()
{
	if (!mCreated)
	{
		return;
	}

	//for (int k = 0; k < 2; k++)
	//{
	//	Ogre::TextureManager::getSingleton().remove(mVolTextures[k]->getName());
	//	mVolTextures[k].setNull();
	//}

	_delete3DCellArray(mCellsCurrent, mNx, mNy);
	_delete3DCellArray(mCellsTmp, mNx, mNy);

	delete mFFRandom;

	mNx = mNy = mNz = 0;

	mCreated = false;
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
void DataManager::setWheater( float Humidity,  float AverageCloudsSize,  int NumberOfForcedUpdates)
{
	int numberofclouds = static_cast<int>(Humidity * mMaxNumberOfClouds);
	Vector3Df maxcloudsize = AverageCloudsSize * Vector3Df(mNx/14, mNy/14, static_cast<int>(static_cast<float>(mNz)/2.75));

	// Update old clouds with new parameters
	Vector3Df currentdimensions;
	AIRVector<Ellipsoid*>::const_iterator mEllipsoidsIt;

	for(mEllipsoidsIt = mEllipsoids.begin(); mEllipsoidsIt != mEllipsoids.end(); mEllipsoidsIt++)
	{
		currentdimensions = (*mEllipsoidsIt)->getDimensions();

		if (currentdimensions.x / maxcloudsize.x < 0.5 || currentdimensions.x / maxcloudsize.x > 2)
		{
			currentdimensions.x = maxcloudsize.x + RangeRandom(-0.2,0.2)*maxcloudsize.x;
		}
		if (currentdimensions.y / maxcloudsize.y < 0.5 || currentdimensions.y / maxcloudsize.y > 2)
		{
			currentdimensions.y = maxcloudsize.y + RangeRandom(-0.2,0.2)*maxcloudsize.y;
		}
		if (currentdimensions.z / maxcloudsize.z < 0.5 || currentdimensions.x / maxcloudsize.z > 2)
		{
			currentdimensions.z = maxcloudsize.z + RangeRandom(-0.2,0.2)*maxcloudsize.z;
		}
	}

	// Remove some clouds if needed
	while (static_cast<unsigned int>(numberofclouds) < mEllipsoids.size())
	{
		mEllipsoids.pop_back();
	}

	// Add new clouds if needed
	Vector3Df newclouddimensions;
	while (static_cast<unsigned int>(numberofclouds) > mEllipsoids.size())
	{
		newclouddimensions = maxcloudsize * Vector3Df( RangeRandom(0.5, 2), RangeRandom(0.5, 2), RangeRandom(0.8, 1.2));
		addEllipsoid(new Ellipsoid(newclouddimensions.x,  newclouddimensions.y,  newclouddimensions.z, mNx, mNy, mNz, (int)RangeRandom(0, mNx), (int)RangeRandom(0, mNy), static_cast<int>(RangeRandom(newclouddimensions.z+2,mNz-newclouddimensions.z-2)), RangeRandom(1,5.0f)), false);
	}

	_updateProbabilities(mCellsCurrent, mNx, mNy, mNz);

	for (int k = 0; k < NumberOfForcedUpdates; k++)
	{
		forceToUpdateData();
	}

}

/** Add ellipsoid: clouds are modelled as ellipsoids in our simulation approach, so.. different kind of clouds 
    can be modelled with ellipsoids compositions.
	@param e Ellipsoid
	@param UpdateProbabilities Update probabilities?
 */
void DataManager::addEllipsoid(Ellipsoid *e,  bool UpdateProbabilities)
{
	mEllipsoids.push_back(e);

	if (UpdateProbabilities)
	{
		e->updateProbabilities(mCellsCurrent,mNx,mNy,mNz);
	}
}

/** Forces the data manager to calculate the next step right now
 */
void DataManager::forceToUpdateData()
{
	if (mVolTexToUpdate)
	{
		_performCalculations(mNx, mNy, mNz);
		_updateVolTextureData(mCellsCurrent, VOL_TEX0, mNx, mNy, mNz);

		mCurrentTransition = mUpdateTime;
		mVolTexToUpdate = !mVolTexToUpdate;
	}
	else
	{
		_performCalculations(mNx, mNy, mNz);
		_updateVolTextureData(mCellsCurrent, VOL_TEX1, mNx, mNy, mNz);

		mCurrentTransition = 0;
		mVolTexToUpdate = !mVolTexToUpdate;
	}
}



/** Initialize data
    @param nx X complexity
	@param ny Y complexity
	@param nz Z complexity
 */
void DataManager::_initData( int nx,  int ny,  int nz)
{
	mCellsCurrent = _create3DCellArray(nx, ny, nz);
	mCellsTmp     = _create3DCellArray(nx, ny, nz);
}

/** Create tridimensional cell array
	@param nx X size
	@param ny Y size
	@param nz Z size
	@param init Initialize values
	@return Cell 3d pointer
 */
Cell *** DataManager::_create3DCellArray( int nx,  int ny,  int nz,  bool init)
{
	Cell ***c = new Cell** [nx];

	int u, v, w;

	for (u = 0; u < nx; u++)
	{
		c[u] = new Cell* [ny];

		for (v = 0; v < ny; v++)
		{
			c[u][v] = new Cell[nz];
		}
	}

	if (!init)
	{
		return c;
	}

	for (u = 0; u < nx; u++)
	{
		for (v = 0; v < ny; v++)
		{
			for (w = 0; w < nz; w++)
			{
				c[u][v][w].act = false;
				c[u][v][w].cld = false;
				c[u][v][w].hum = false;

				c[u][v][w].pact = 0;
				c[u][v][w].pext = 1;
				c[u][v][w].phum = 0;

				c[u][v][w].dens = 0.0f;
				c[u][v][w].light = 0.0f;
			}
		}
	}

	return c;
}

/** Delete tridimensional cell array
	@param c Cell pointer to be deleted
	@param nx X size
	@param ny Y size
 */
void DataManager::_delete3DCellArray(Cell ***c,  int nx,  int ny)
{
	int u, v;

	for (u = 0; u < nx; u++)
	{
		for (v = 0; v < ny; v++)
		{
			delete [] c[u][v];
		}

		delete [] c[u];
	}

	delete [] c;
}

/** Copy 3d cells arrays data
	@param or Origin
	@param dest Dest
	@param nx X size
	@param ny Y size
	@param nz Z size
*/
void DataManager::_copy3DCellArraysData(Cell ***ori, Cell ***dest,  int nx,  int ny,  int nz)
{
	int u, v, w;

	for (u = 0; u < nx; u++)
	{
		for (v = 0; v < ny; v++)
		{
			for (w = 0; w < nz; w++)
			{
				dest[u][v][w].act = ori[u][v][w].act;
				dest[u][v][w].cld = ori[u][v][w].cld;
				dest[u][v][w].hum = ori[u][v][w].hum;

				dest[u][v][w].pact = ori[u][v][w].pact;
				dest[u][v][w].pext = ori[u][v][w].pext;
				dest[u][v][w].phum = ori[u][v][w].phum;

				dest[u][v][w].dens = ori[u][v][w].dens;
				dest[u][v][w].light = ori[u][v][w].light;
			}
		}
	}
}

/** Perform celullar automata simulation
    @param nx X size
	@param ny Y size
	@param nz Z size
 */
void DataManager::_performCalculations( int nx,  int ny,  int nz)
{
	// First step

	int u, v, w;

	for (u = 0; u < nx; u++)
	{
		for (v = 0; v < ny; v++)
		{
			for (w = 0; w < nz; w++)
			{
				// ti+1                       ti
				mCellsCurrent[u][v][w].hum =  mCellsTmp[u][v][w].hum && !mCellsTmp[u][v][w].act;
				mCellsCurrent[u][v][w].cld =  mCellsTmp[u][v][w].cld ||  mCellsTmp[u][v][w].act;
				mCellsCurrent[u][v][w].act = !mCellsTmp[u][v][w].act &&  mCellsTmp[u][v][w].hum && _fact(mCellsTmp, nx, ny, nz, u,v,w);
			}
		}
	}

	// Second step

	_copy3DCellArraysData(mCellsCurrent, mCellsTmp, nx, ny, nz);

	for (u = 0; u < nx; u++)
	{
		for (v = 0; v < ny; v++)
		{
			for (w = 0; w < nz; w++)
			{
				// ti+1                       ti
				mCellsCurrent[u][v][w].hum = mCellsTmp[u][v][w].hum || (mFFRandom->get() < mCellsTmp[u][v][w].phum);
				mCellsCurrent[u][v][w].cld = mCellsTmp[u][v][w].cld && (mFFRandom->get() > mCellsTmp[u][v][w].pext);
				mCellsCurrent[u][v][w].act = mCellsTmp[u][v][w].act || (mFFRandom->get() < mCellsTmp[u][v][w].pact);
			}
		}
	}

	// Final steps

	// Continous density
	for (u = 0; u < nx; u++)
	{
		for (v = 0; v < ny; v++)
		{
			for (w = 0; w < nz; w++)
			{
				mCellsCurrent[u][v][w].dens = _getDensityAt(mCellsCurrent, nx, ny, nz, u,v,w, 1);
				//mCellsCurrent[u][v][w].dens = _getDensityAt(mCellsCurrent,u,v,w);
			}
		}
	}

	// Light scattering
	Vector3Df SunDir = Vector3Df(mVClouds->getSunDirection().x, mVClouds->getSunDirection().z, mVClouds->getSunDirection().y);

	for (u = 0; u < nx; u++)
	{
		for (v = 0; v < ny; v++)
		{
			for (w = 0; w < nz; w++)
			{
				mCellsCurrent[u][v][w].light = _getLightAbsorcionAt(mCellsCurrent, mNx, mNy, mNz, u,v,w, SunDir, 0.15f);
			}
		}
	}

	_copy3DCellArraysData(mCellsCurrent, mCellsTmp, nx, ny, nz);
}

/** Update volumetric texture data
    @param c Cells data
	@param TexId Texture Id
    @param nx X size
	@param ny Y size
	@param nz Z size
 */
void DataManager::_updateVolTextureData(Cell ***c,  VolTextureId TexId,  int nx,  int ny,  int nz)
{
	//Ogre::HardwarePixelBufferSharedPtr buffer = mVolTextures[TexId]->getBuffer(0,0);

	//buffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
	//const Ogre::PixelBox &pb = buffer->getCurrentLock();

	//Ogre::uint16 *pbptr = static_cast<Ogre::uint16*>(pb.data);
	//size_t x, y, z;

	//for (z=pb.front; z<pb.back; z++) 
	//{
	//	for (y=pb.top; y<pb.bottom; y++)
	//	{
	//		for (x=pb.left; x<pb.right; x++)
	//		{
	//			Ogre::PixelUtil::packColour(c[x][y][z].dens, c[x][y][z].light, 0, 0, Ogre::PF_BYTE_RGB, &pbptr[x]);
	//		} 
	//		pbptr += pb.rowPitch;
	//	}
	//	pbptr += pb.getSliceSkip();
	//}

	//buffer->unlock();
}

/** Get continous density at a point
    @param c Cells data
	@param nx X size
	@param ny Y size
	@param nz Z size
	@param x x Coord
	@param y y Coord
	@param z z Coord 
	@param r Radius
 */	
float DataManager::_getDensityAt(Cell ***c,  int nx,  int ny,  int nz,  int x,  int y,  int z,  int r) 
{
	int zr = ((z-r)<0) ? 0 : z-r,
		zm = ((z+r)>=nz) ? nz : z+r,
		u, uu, v, vv, w, 
		clouds = 0, div = 0;

	for (u = x-r; u <= x+r; u++)
	{
		for (v = y-r; v <= y+r; v++)
		{
			for (w = zr; w < zm; w++)
			{
				// x/y Seamless!
				uu = (u<0) ? (u + nx) : u; if (u>=nx) { uu-= nx; }
				vv = (v<0) ? (v + ny) : v; if (v>=ny) { vv-= ny; }

				clouds += c[uu][vv][w].cld ? 1 : 0;
				div++;
			}
		}
	}

	return ((float)clouds)/div;
}

/** Get discrete density at a point
    @param c Cells data
	@param x x Coord
	@param y y Coord
	@param z z Coord 
 */	
float DataManager::_getDensityAt(Cell ***c,  int x,  int y,  int z) 
{
	return c[x][y][z].cld ? 1.0f : 0.0f;
}

/** Fact funtion
    @param c Cells data
	@param nx X size
	@param ny Y size
	@param nz Z size
	@param x x Coord
	@param y y Coord
	@param z z Coord 
 */
bool DataManager::_fact(Cell ***c,  int nx,  int ny,  int nz,  int x,  int y,  int z) 
{
	bool i1m, j1m, k1m, 
		i1r, j1r, k1r, 
		i2r, i2m, j2r, j2m, k2r;

	i1m = ((x+1)>=nx) ? c[0][y][z].act : c[x+1][y][z].act;
	j1m = ((y+1)>=ny) ? c[x][0][z].act : c[x][y+1][z].act;
	k1m = ((z+1)>=nz) ? false : c[x][y][z+1].act;

	i1r = ((x-1)<0) ? c[nx-1][y][z].act : c[x-1][y][z].act;
	j1r = ((y-1)<0) ? c[x][ny-1][z].act : c[x][y-1][z].act;
	k1r = ((z-1)<0) ? false : c[x][y][z-1].act;

	i2r = ((x-2)<0) ? c[nx-2][y][z].act : c[x-2][y][z].act;
	j2r = ((y-2)<0) ? c[x][ny-2][z].act : c[x][y-2][z].act;
	k2r = ((z-2)<0) ? false : c[x][y][z-2].act;

	i2m = ((x+2)>=nx) ? c[1][y][z].act : c[x+2][y][z].act;
	j2m = ((y+2)>=ny) ? c[x][1][z].act : c[x][y+2][z].act;

	return i1m || j1m || k1m  || i1r || j1r || k1r || i2r || i2m || j2r || j2m || k2r;
}

/** Clear probabilities
    @param c Cells data
	@param nx X size
	@param ny Y size
	@param nz Z size
 */
void DataManager::_clearProbabilities(Cell*** c,  int nx,  int ny,  int nz)
{
	int u, v, w;

	for (u = 0; u < nx; u++)
	{
		for (v = 0; v < ny; v++)
		{
			for (w = 0; w < nz; w++)
			{
				c[u][v][w].pact = 0;
				c[u][v][w].pext = 1;
				c[u][v][w].phum = 0;
			}
		}
	}
}

/** Update probabilities based from the Ellipsoid vector
    @param c Cells data
	@param nx X size
	@param ny Y size
	@param nz Z size
	@param ClearProbabilities Clear probabilities?
 */
void DataManager::_updateProbabilities(Cell*** c,  int nx,  int ny,  int nz,  bool ClearProbabilities)
{
	if (ClearProbabilities)
	{
		_clearProbabilities(c,nx,ny,nz);
	}

	AIRVector<Ellipsoid*>::const_iterator mEllipsoidsIt;

	for(mEllipsoidsIt = mEllipsoids.begin(); mEllipsoidsIt != mEllipsoids.end(); mEllipsoidsIt++)
	{
		(*mEllipsoidsIt)->updateProbabilities(c,nx,ny,nz);
	}
}

/** Get light absorcion factor at a point
	@param c Cells data
	@param nx X size
	@param ny Y size
	@param nz Z size
	@param x x Coord
	@param y y Coord
	@param z z Coord 
	@param d Light direction
	@param att Attenuation factor
 */
float DataManager::_getLightAbsorcionAt(Cell*** c,  int nx,  int ny,  int nz,  int x,  int y,  int z, const Vector3Df& d,  float att) 
{
	float step = 1, factor = 1;
	Vector3Df pos = Vector3Df(x, y, z);
	bool outOfBounds = false;
	int u, v, uu, vv,
		current_iteration = 0, max_iterations = 8;

	while(!outOfBounds)
	{
		if ( (int)pos.z >= nz || (int)pos.z < 0 || factor <= 0 || current_iteration >= max_iterations)
		{
			outOfBounds = true;
		}
		else
		{
			u = (int)pos.x; v = (int)pos.y;

			uu = (u<0) ? (u + nx) : u; if (u>=nx) { uu-= nx; }
			vv = (v<0) ? (v + ny) : v; if (v>=ny) { vv-= ny; }

			factor -= c[uu][vv][(int)pos.z].dens*att*(1-static_cast<float>(current_iteration)/max_iterations);
			pos += step*(-d);

			current_iteration++;
		}
	}

	return Clamp<float>(factor,0,1);
}

/** Create volumetric texture
	@param TexId Texture Id
	@param nx X size
	@param ny Y size
	@param nz Z size
 */
void DataManager::_createVolTexture( VolTextureId TexId,  int nx,  int ny,  int nz)
{
	//mVolTextures[static_cast<int>(TexId)] 
	//= Ogre::TextureManager::getSingleton().
	//	createManual("_SkyX_VolCloudsData"+Ogre::StringConverter::toString(TexId),
	//	SKYX_RESOURCE_GROUP, 
	//	Ogre::TEX_TYPE_3D,
	//	nx, ny, nz, 0,
	//	Ogre::PF_BYTE_RGB);

	//mVolTextures[static_cast<int>(TexId)]->load();

	//static_cast<Ogre::MaterialPtr>(
	//	Ogre::MaterialManager::getSingleton().getByName("SkyX_VolClouds"))
	//	->getTechnique(0)->getPass(0)->getTextureUnitState(static_cast<int>(TexId))
	//	->setTextureName("_SkyX_VolCloudsData"+Ogre::StringConverter::toString(TexId), Ogre::TEX_TYPE_3D);

}