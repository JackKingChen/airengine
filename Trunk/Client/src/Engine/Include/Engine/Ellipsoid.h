// ***************************************************************
//  文件: Ellipsoid.h
//  日期: 2010-8-21
//  作者: huangjunjie
//  说明: 
// ***************************************************************


#pragma once

#ifndef __Ellipsoid_H_
#define __Ellipsoid_H_



class DataManager;
struct Cell;


#include "EngineConfig.h"
#include "Vector3D.h"


using namespace AIR;



/** Ellipsoid class
    x^2   y^2   z^2
	/   + /   + /    = 1
	a^2   b^2   c^2
 */
class EngineExport Ellipsoid
{
public:
	/** Constructor
	    @param a A constant
		@param b B constant
		@param c C constant
		@param nx X size
		@param ny Y size
		@param nz Z size
		@param x x Coord
		@param y y Coord
		@param z z Coord 
		@param Density Cloud density
	 */
	Ellipsoid(int a, int b, int c, 
		      int nx, int ny, int nz,
			  int x, int y, int z, 
			  float Density = 1.0f);

	/** Destructor 
	 */
	~Ellipsoid();

	/** Move the ellipsoid
	    @param Ax x increment
		@param Ay y increment
		@param Az z increment
	 */
	void move( int Ax,  int Ay,  int Az);

	/** Get probabilities at a point
		@param x x Coord
		@param y y Coord
		@param z z Coord 
		@return Probabilities (Hum, Ext, Act)
	 */
	Vector3Df getProbabilities( int x, int y, int z) ;

	/** Update probabilities
		@param c Cells
		@param nx X complexity
		@param ny Y complexity
		@param nz Z complexity
	 */
	void updateProbabilities(Cell ***c,  int nx,  int ny,  int nz);

	/** Determines if the ellipsoid is out of the cells domain and needs to be removed
	 */
	bool isOutOfCells() const;

	/** Get dimensions
	    @return Ellipsoid dimensions
	 */
	inline Vector3Df getDimensions() 
	{
		return Vector3Df(mA, mB, mC);
	}

	/** Set dimensions
	    @param Dimensions New dimensions
	 */
	void setDimensions(const Vector3Df& Dimensions);

private:
	/** Get length
	    @param x x Coord
		@param y y Coord
		@param z z Coord
		@return [0,1] range where 0 is the center of the ellipsoid and 1 the superfice
	 */
	float _getLength( int x,  int y,  int z);

	/// Ellipsoid parameters
	int mA, mB, mC, mA2, mB2, mC2;

	/// Position
	int mX, mY, mZ;

	/// Cells size
	int mNx, mNy, mNz;

	/// Cloud density
	float mDensity;
}; //! end class Ellipsoid



#endif //! end __Ellipsoid_H_