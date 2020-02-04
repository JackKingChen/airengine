// ***************************************************************
//  文件: FastFakeRandom.h
//  日期: 2010-8-21
//  作者: huangjunjie
//  说明: 
// ***************************************************************


#pragma once

#ifndef __FastFakeRandom_H_
#define __FastFakeRandom_H_


#include "EngineConfig.h"
#include "MyMath.h"
#include "Util.h"


class EngineExport FastFakeRandom
{
public:



	/** Constructor 
	    @param n Capacity
		@param min Min value
		@param max Max value
	 */
	FastFakeRandom( int n, float min, float max)
		: mCapacity(n)
		, mIndex(-1)
	{
		mData = new float[n];

		for (int k = 0; k < n; k++)
		{
			mData[k] = AIR::RangeRandom(min, max);
		}
	}

	/** Destructor
     */
	~FastFakeRandom()
	{
		SafeDeleteArray( mData );
	}

	/** Get random number
	 */
	float& get()
	{
		mIndex ++; 
		if (mIndex >= mCapacity) 
		{
			mIndex = -1;
		}

		return mData[mIndex];
	}

private:
	/// Data pointer
	float *	mData;
	/// Capacity
	int		mCapacity;
	/// Index
	int		mIndex;


}; //! end class FastFakeRandom



#endif //! end __FastFakeRandom_H_