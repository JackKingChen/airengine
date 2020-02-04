// ***************************************************************
//  文件: DataManager.h
//  日期: 2010-8-21
//  作者: huangjunjie
//  说明: 
// ***************************************************************


#pragma once

#ifndef __DataManager_H_
#define __DataManager_H_


class VClouds;
class Ellipsoid;



#include "FastFakeRandom.h"
#include "Vector3D.h"
#include "ITexture.h"




using namespace AIR;

//单元结构体
struct Cell
{
	//湿度, 相位, 云
	bool hum, act, cld;

	//几率
	float phum, pext, pact;

	//持续密度
	float dens;

	//Light absorcion
	float light;
};


class EngineExport DataManager 
{
public:


	//体积纹理
	enum VolTextureId
	{
		VOL_TEX0 = 0,
		VOL_TEX1 = 1
	};


	DataManager(VClouds *vc);


	~DataManager();

	//创建
	//@param nx X 复杂度
	//@param ny Y 复杂度
	//@param nz Z 复杂度
	void create(int nx, int ny, int nz);

	//每帧更新
	void update( float timeSinceLastFrame);

	//移除
	void remove();

	//是否已经创建完毕
	inline bool isCreated()
	{
		return mCreated;
	}

	//设置更新时间长度
	inline void setUpdateTime( float UpdateTime)
	{
		mUpdateTime = UpdateTime;
	}

	//获得更新时间长度
	inline float getUpdateTime() const
	{
		return mUpdateTime;
	}

	//获得插值系数
	inline float _getInterpolation() const
	{
		return mCurrentTransition/mUpdateTime;
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
	void setWheater( float Humidity,  float AverageCloudsSize, int NumberOfForcedUpdates = 0);

	/** Add ellipsoid: clouds are modelled as ellipsoids in our simulation approach, so.. different kind of clouds 
	    can be modelled with ellipsoids compositions.
		@param e Ellipsoid
		@param UpdateProbabilities Update probabilities?
	 */
	void addEllipsoid(Ellipsoid *e,  bool UpdateProbabilities = true);

	/** Forces the data manager to calculate the next step right now
	 */
	void forceToUpdateData();

private:

	/** Initialize data
	    @param nx X complexity
		@param ny Y complexity
		@param nz Z complexity
	 */
	void _initData( int nx, int ny, int nz);

	/** Create tridimensional cell array
		@param nx X size
		@param ny Y size
		@param nz Z size
		@param init Initialize values
		@return Cell 3d pointer
	 */
	Cell *** _create3DCellArray( int nx,  int ny,  int nz, bool init = true);

	/** Delete tridimensional cell array
		@param c Cell pointer to be deleted
		@param nx X size
		@param ny Y size
	 */
	void _delete3DCellArray(Cell ***c,  int nx,  int ny);

	/** Copy 3d cells arrays data
		@param or Origin
		@param dest Dest
		@param nx X size
		@param ny Y size
		@param nz Z size
	*/
	void _copy3DCellArraysData(Cell *** ori, Cell *** dest,  int nx,  int ny,  int nz);

	/** Perform celullar automata simulation
	    @param nx X size
		@param ny Y size
		@param nz Z size
	 */
	void _performCalculations( int nx,  int ny,  int nz);

	/** Update volumetric texture data
	    @param c Cells data
		@param TexId Texture Id
	    @param nx X size
		@param ny Y size
		@param nz Z size
	 */
	void _updateVolTextureData(Cell ***c,  VolTextureId TexId,  int nx,  int ny,  int nz);

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
	float _getDensityAt(Cell ***c,  int nx,  int ny,  int nz,  int x,  int y,  int z,  int r) ;

	/** Get discrete density at a point
	    @param c Cells data
		@param x x Coord
		@param y y Coord
		@param z z Coord 
	 */	
	float _getDensityAt(Cell ***c,  int x,  int y,  int z) ;

	/** Fact funtion
	    @param c Cells data
		@param nx X size
		@param ny Y size
		@param nz Z size
		@param x x Coord
		@param y y Coord
		@param z z Coord 
	 */
	bool _fact(Cell ***c,  int nx,  int ny,  int nz,  int x,  int y,  int z);

	/** Clear probabilities
	    @param c Cells data
		@param nx X size
		@param ny Y size
		@param nz Z size
	 */
	void _clearProbabilities(Cell*** c,  int nx,  int ny,  int nz);

	/** Update probabilities based from the Ellipsoid vector
	    @param c Cells data
		@param nx X size
		@param ny Y size
		@param nz Z size
		@param ClearProbabilities Clear probabilities?
	 */
	void _updateProbabilities(Cell*** c,  int nx,  int ny,  int nz,  bool ClearProbabilities = true);

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
	float _getLightAbsorcionAt(Cell*** c,  int nx,  int ny,  int nz,  int x,  int y,  int z, const Vector3Df& d,  float att) ;

	/** Create volumetric texture
		@param TexId Texture Id
		@param nx X size
		@param ny Y size
		@param nz Z size
	 */
	void _createVolTexture( VolTextureId TexId,  int nx,  int ny,  int nz);

	/// Simulation data
	Cell ***mCellsCurrent,
		 ***mCellsTmp;

	/// Current transition
	float mCurrentTransition;
	/// Update time
	float mUpdateTime;

	/// Complexities
	int mNx, mNy, mNz;

	/// Volumetric textures array
	ITexture* mVolTextures[2];//Ogre::TexturePtr mVolTextures[2];
	/// Current texture
	bool mVolTexToUpdate;

	/// Has been create(...) already called?
	bool mCreated;

	/// Fast fake random
	FastFakeRandom *mFFRandom;

	/// Max number of clouds(Ellipsoids)
	int mMaxNumberOfClouds;
	/// Ellipsoids
	AIRVector<Ellipsoid*> mEllipsoids;

	/// SkyX parent pointer
	VClouds *mVClouds;
	}; //! end class DataManager



#endif //! end __DataManager_H_