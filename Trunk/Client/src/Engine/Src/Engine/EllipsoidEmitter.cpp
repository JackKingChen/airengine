#include "stdafx.h"
#include "EllipsoidEmitter.h"

CEllipsoidEmitter::CEllipsoidEmitter()
{
    m_strType = "EllipsoidEmitter";
}

CEllipsoidEmitter::~CEllipsoidEmitter()
{

}

void CEllipsoidEmitter::InitParitcle(CParticle* pParticle)
{
	float x, y, z;

	// Call superclass
	//AreaEmitter::_initParticle(pParticle);
	// First we create a random point inside a bounding sphere with a
	// radius of 1 (this is easy to do). The distance of the point from
	// 0,0,0 must be <= 1 (== 1 means on the surface and we count this as
	// inside, too).

	while (true)
	{
		// three random values for one random point in 3D space

		x = AIR::SymmetricRandom();
		y = AIR::SymmetricRandom();
		z = AIR::SymmetricRandom();

		// the distance of x,y,z from 0,0,0 is sqrt(x*x+y*y+z*z), but
		// as usual we can omit the sqrt(), since sqrt(1) == 1 and we
		// use the 1 as boundary:
		if ( x*x + y*y + z*z <= 1)
		{
			break;          // found one valid point inside
		}
	}       

	// scale the found point to the ellipsoid's size and move it
	// relatively to the center of the emitter point

	pParticle->m_position = m_position + 
		m_XRange * x * 2.0f + m_YRange * y * 2.0f + m_ZRange * z * 2.0f;

	// Generate complex data by reference
	GenEmissionColour(pParticle->m_color);
	GenEmissionDirection(pParticle->m_direction);
	GenEmissionVelocity(pParticle->m_direction);

	// Generate simpler data
	pParticle->m_fLifeTime = pParticle->m_fTotalTimeToLive = GenEmissionTTL();
}
