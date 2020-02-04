#include "stdafx.h"
#include "CylinderEmitter.h"

CCylinderEmitter::CCylinderEmitter()
{
    m_strType = "CylinderEmitter";
}

CCylinderEmitter::~CCylinderEmitter()
{

}

void CCylinderEmitter::InitParitcle(CParticle* pParticle)
{
    float x, y, z;

    // Call superclass
    //CParticleEmitter::InitParticle(pParticle);

    // First we create a random point inside a bounding cylinder with a
    // radius and height of 1 (this is easy to do). The distance of the
    // point from 0,0,0 must be <= 1 (== 1 means on the surface and we
    // count this as inside, too).

    while (true)
    {
        /* ClearSpace not yet implemeted

        */
        // three random values for one random point in 3D space
        x = AIR::SymmetricRandom();
        y = AIR::SymmetricRandom();
        z = AIR::SymmetricRandom();

        // the distance of x,y from 0,0 is sqrt(x*x+y*y), but
        // as usual we can omit the sqrt(), since sqrt(1) == 1 and we
        // use the 1 as boundary. z is not taken into account, since
        // all values in the z-direction are inside the cylinder:
        if ( x*x + z*z <= 1)
        {
            break;          // found one valid point inside
        }
    }       

    // scale the found point to the cylinder's size and move it
    // relatively to the center of the emitter point

    pParticle->m_position = m_position 
        + m_XRange * x * 2.0f + m_YRange * y + m_ZRange * z * 2.0f;

    // Generate complex data by reference
    GenEmissionColour(pParticle->m_color);
    GenEmissionDirection(pParticle->m_direction);
    GenEmissionVelocity(pParticle->m_direction);

    // Generate simpler data
    pParticle->m_fTotalTimeToLive = pParticle->m_fLifeTime = GenEmissionTTL();
}
