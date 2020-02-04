#include "stdafx.h"
#include "Ellipsoid.h"
#include "DataManager.h"


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
Ellipsoid::Ellipsoid( int a,  int b,  int c, 
	      int nx, int ny, int nz,
		  int x, int y, int z, 
		  float Density)
		  : mA(a), mB(b), mC(c)
		  , mA2(powf(a, 2)), mB2(powf(b, 2)), mC2(powf(c, 2))
		  , mNx(nx), mNy(ny), mNz(nz)
		  , mX(x), mY(y), mZ(z)
		  , mDensity(Density)
{

}

/** Destructor 
 */
Ellipsoid::~Ellipsoid()
{

}

/** Move the ellipsoid
    @param Ax x increment
	@param Ay y increment
	@param Az z increment
 */
void Ellipsoid::move( int Ax,  int Ay,  int Az)
{
	mX += Ax; mY += Ay; mZ += Az;
}

/** Get probabilities at a point
	@param x x Coord
	@param y y Coord
	@param z z Coord 
	@return Probabilities (Hum, Ext, Act)
 */
Vector3Df Ellipsoid::getProbabilities( int x, int y, int z) 
{
	float density = pow(1-_getLength(x, y, z), 1.0f/mDensity);

	return Vector3Df(density, 1-density, density);
}

/** Update probabilities
	@param c Cells
	@param nx X complexity
	@param ny Y complexity
	@param nz Z complexity
 */
void Ellipsoid::updateProbabilities(Cell ***c,  int nx,  int ny,  int nz)
{
	int u, v, w, uu, vv;
	// Ogre::Vector3 prob;
	float length;

	for (u = mX-mA; u < mX+mA; u++)
	{
		for (v = mY-mB; v < mY+mB; v++)
		{
			for (w = mZ-mC; w < mZ+mC; w++)
			{
				//	prob = getProbabilities(u, v, w);

				length = _getLength(uu, vv, w);

				// x/y Seamless!
				uu = (u<0) ? (u + nx) : u; if (u>=nx) { uu-= nx; }
				vv = (v<0) ? (v + ny) : v; if (v>=ny) { vv-= ny; }

				//	c[uu][vv][w].phum = Ogre::Math::Clamp<Ogre::Real>(c[uu][vv][w].phum+prob.x, 0, 1);
				//	c[uu][vv][w].pext = std::min<float>(prob.y, c[uu][vv][w].pext);
				//	c[uu][vv][w].pact = Ogre::Math::Clamp<Ogre::Real>(c[uu][vv][w].pact+prob.z, 0, 1);

				if (length < 1)
				{
					c[uu][vv][w].phum = 0.1f;
					c[uu][vv][w].pext = 0.5f;
					c[uu][vv][w].pact = 0.2f;
				}
			}
		}
	}

}

/** Determines if the ellipsoid is out of the cells domain and needs to be removed
 */
bool Ellipsoid::isOutOfCells() const
{
	if ( (mX+mA) >= mNx || (mX-mA) < 0 ||
		(mY+mB) >= mNy || (mY-mB) < 0 ||
		(mZ+mZ) >= mNz || (mZ-mZ) < 0 )
	{
		return true;
	}

	return false;
}



/** Set dimensions
    @param Dimensions New dimensions
 */
void Ellipsoid::setDimensions(const Vector3Df& Dimensions)
{
	mA = Dimensions.x; 
	mB = Dimensions.y; 
	mC = Dimensions.z;

	mA2 = powf(mA, 2); 
	mB2 = powf(mB, 2); 
	mC2 = powf(mC, 2);
}


/** Get length
    @param x x Coord
	@param y y Coord
	@param z z Coord
	@return [0,1] range where 0 is the center of the ellipsoid and 1 the superfice
 */
float Ellipsoid::_getLength( int x,  int y,  int z) 
{
	//  x^2   y^2   z^2
	//  /   + /   + /    = 1  (Ellipsoid ecuation)
	//  a^2   b^2   c^2
	// 
	//  maxradatdir = lambda (Xo, Yo, Zo) = lambda; where Xo, Yo and Zo are the components of the normaliced direction vector
	//
	//  => lambda^2 = 1 / ( EllipsoidEcuation...)
	//
	//  => lambda = sqrt (...) => maxradatdir = lambda

	Vector3Df Direction = Vector3Df(x-mX, y-mY, z-mZ),
		DirectionNormalized = Direction.Normalize();

	float a = pow(DirectionNormalized.x, 2) / mA2 + 
		pow(DirectionNormalized.y, 2) / mB2 +
		pow(DirectionNormalized.z, 2) / mC2,

		lambda = 1.0f / asm_sqrt(a);

	return Clamp<float>(Direction.GetLength() / lambda, 0, 1);
}