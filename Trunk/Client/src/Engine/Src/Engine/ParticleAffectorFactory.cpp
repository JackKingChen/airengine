#include "stdafx.h"
#include "ParticleAffectorFactory.h"
#include "ScaleAffector.h"
#include "RotationAffector.h"
#include "ColorAffector.h"
#include "LinearForceAffector.h"
#include "DeflectorPlaneAffector.h"
#include "DirectionRandomiserAffector.h"
#include "ColorImageAffector.h"
#include "Util.h"
#include "AxisRotateAffector.h"
#include "PointAttractAffector.h"
#include "ColorSegmentAffector.h"
#include "ScaleSegmentAffector.h"
#include "FullScaleSegmentAffector.h"
#include "AlphaSegmentAffector.h"


CParticleAffectorFactory::CParticleAffectorFactory()
{

}

CParticleAffectorFactory::~CParticleAffectorFactory()
{

}

CParticleAffector* CParticleAffectorFactory::CreateAffector(const AIRString& strName, CMarkupSTL& xml)
{
    CParticleAffector* pAffector = NULL;
    if (strName == "Scaler")
    {
        pAffector = new CScaleAffector;
    }
    else if (strName == "Rotator")
    {
        pAffector = new CRotationAffector;
    }
    else if (strName == "Color")
    {
        pAffector = new CColorAffector;
    }
    else if (strName == "LinearForce")
    {
        pAffector = new CLinearForceAffector;
    }
    else if (strName == "DeflectorPlane")
    {
        pAffector = new CDeflectorPlaneAffector;
    }
    else if (strName == "DirectionRandomiser")
    {
        pAffector = new CDirectionRandomiserAffector;
    }
	else if (strName == "AxisRotator")
	{
		pAffector = new CAxisRotateAffector;
	}
    else if (strName == "ColorImage")
    {
		pAffector = new CColorImageAffector;
    }
	else if (strName == "PointAttract" )
	{
		pAffector = new CPointAttractAffector;
	}
	else if (strName == "ColorSegment")
	{
		pAffector = new CColorSegmentAffector;
	}
	else if (strName == "ScaleSegment" )
	{
		pAffector = new CScaleSegmentAffector;
	}
	else if (strName == "FullScaleSegment" )
	{
		pAffector = new CFullScaleSegmentAffector;
	}
	else if (strName == "AlphaSegment" )
	{
		pAffector = new CAlphaSegmentAffector;
	}
    
    if (pAffector == NULL)
    {
        return NULL;
    }

    if (!pAffector->LoadFromXML(xml))
    {
        SafeDelete(pAffector);
    }

    return pAffector;
}
