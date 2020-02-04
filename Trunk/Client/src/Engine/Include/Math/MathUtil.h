#pragma once 


#include "Vector3D.h"
#include "Quaternion.h"
#include "AABBBox3D.h"
#include "Matrix4.h"

using namespace AIR;

class EngineExport CMathUtil
{
public:
    static inline  Vector3Df RandomDeviant(const float fRadius, const Vector3Df& vSrc, const Vector3Df& vUp)
    {
        Vector3Df newUp;

        if (vUp == Vector3Df::ZERO)
        {
            // Generate an up vector
            newUp = vSrc.Perpendicular();
        }
        else
        {
            newUp = vUp;
        }

        // Rotate up vector by random amount around this
        Quaternion q;
        q.FromAngleAxis( AIR::UnitRandom() * AIR::TWO_PI, vSrc );
        newUp = q * newUp;

        // Finally rotate this by given angle around randomised up
        q.FromAngleAxis( fRadius, newUp );
        return q * vSrc;
    }

	static inline  Vector3Df RandomDeviant(const float fHorzRadian, const float fVertRadian, const Vector3Df& vSrc, const Vector3Df& vUp)
	{
		Vector3Df newUp(vUp);
		if (vUp == Vector3Df::ZERO)
		{
			newUp = vSrc.Perpendicular();
		}
		Quaternion q;
		q.FromAngleAxis( (0.5 - AIR::UnitRandom()) * fVertRadian, newUp );
		Vector3Df vRes = q * vSrc;
		Vector3Df vRight = newUp.CrossProduct( vRes );
		q.FromAngleAxis( (0.5 - AIR::UnitRandom()) * fHorzRadian, vRight );
		return q * vRes;
	}

    static inline void XFormBoundingBox(AABBBox3Df& out, const AABBBox3Df& src, const Matrix4f& mat)
    {
        Vector3Df  pts[8];
        for ( int i = 0; i < 8; i++ )
            pts[i] = src.Point(i);

        out.MinEdge = Vector3Df(3.3e33f, 3.3e33f, 3.3e33f);
        out.MaxEdge = Vector3Df(-3.3e33f, -3.3e33f, -3.3e33f);

        Vector3Df tmp;
        for (u32 i = 0; i < 8; i++)
        {
            //tmp = src.Point(i);
            mat.TransformCoord(pts[i], tmp);
            out.AddInternalPoint(tmp);
            //D3DXVec3TransformCoord(&tmp, &pts[i], matrix);
            //result->Merge(&tmp);
        }
    }

	static inline f32 Exp( f32 fValue ) 
	{ 
		return f32( exp(fValue) ); 
	}

	static inline f32 Floor( f32 fValue )
	{ 
		return f32( floor(fValue) ); 
	}

	static inline f32 Log( f32 fValue ) 
	{ 
		return f32( log(fValue) ); 
	}

	static inline f32 Pow( f32 fBase, f32 fExponent ) 
	{ 
		return f32(pow(fBase,fExponent)); 
	}

	static inline f32 Sign (f32 fValue)
	{
		if ( fValue > 0.0 )
			return 1.0;

		if ( fValue < 0.0 )
			return -1.0;

		return 0.0;
	}

	static inline f32 Cos( f32 fValue ) 
	{
		return f32( cos(fValue) );
	}

	static inline f32 Sin( f32 fValue )
	{
		return f32( sin(fValue) );
	}

	static inline int IAbs (int iValue) 
	{ 
		return ( iValue >= 0 ? iValue : -iValue ); 
	}

	static inline int ICeil (float fValue) 
	{ 
		return int(ceil(fValue)); 
	}

	static inline int IFloor (float fValue) 
	{ 
		return int(floor(fValue)); 
	}

	static inline int ISign (int iValue)
	{
		return ( iValue > 0 ? +1 : ( iValue < 0 ? -1 : 0 ) );
	}

	static inline f32 Abs (f32 fValue) 
	{ 
		return f32(AIR::Abs(fValue)); 
	}

	static inline f32 ACos (f32 fValue)
	{
		if ( -1.0 < fValue )
		{
			if ( fValue < 1.0 )
				return f32(acos(fValue));
			else
				return 0.0f;
		}
		else
		{
			return f32(AIR::PI);
		}
	}

	static inline f32 ASin (f32 fValue)
	{
		if ( -1.0 < fValue )
		{
			if ( fValue < 1.0 )
				return f32(asin(fValue));
			else
				return f32(AIR::HALF_PI);
		}
		else
		{
			return f32(-AIR::HALF_PI);
		}
	}

	static inline f32 ATan (f32 fValue) 
	{ 
		return f32(atan(fValue)); 
	}

	static inline f32 ATan2 (f32 fY, f32 fX) 
	{ 
		return f32(atan2(fY,fX)); 
	}

	static inline f32 Ceil (f32 fValue) 
	{ 
		return f32(ceil(fValue)); 
	}

	static inline void GetGaussianOffsets( 
		bool								bHorizontal, 
		const AIR::Vector2D        & vViewportTexelSize,
		AIRVector< AIR::Vector2D>&	vSampleOffsets, 
		AIRVector<float>&					fSampleWeights, 
		int									nArraySize,
		float                               fCullWidth)
	{
		// Get the center texel offset and weight
		fSampleWeights[0] = 1.0f * AIR::GetGaussianDistribution( 0, 0, fCullWidth, 1.0f );
		vSampleOffsets[0] = AIR::Vector2D( 0.0f, 0.0f );

		// Get the offsets and weights for the remaining taps
		if( bHorizontal ) 
		{
			for( int i = 1; i < nArraySize; i += 2 ) 
			{
				vSampleOffsets[i + 0] = AIR::Vector2D(  i * vViewportTexelSize.x, 0.0f );
				vSampleOffsets[i + 1] = AIR::Vector2D( -i * vViewportTexelSize.x, 0.0f );

				fSampleWeights[i + 0] = 2.0f * AIR::GetGaussianDistribution( float(i + 0), 0.0f, 3.0f, 1.0f );
				fSampleWeights[i + 1] = 2.0f * AIR::GetGaussianDistribution( float(i + 1), 0.0f, 3.0f, 1.0f );
			}
		}
		else 
		{
			for( int i = 1; i < nArraySize; i += 2 ) 
			{
				vSampleOffsets[i + 0] = AIR::Vector2D( 0.0f,  i * vViewportTexelSize.y );
				vSampleOffsets[i + 1] = AIR::Vector2D( 0.0f, -i * vViewportTexelSize.y );

				fSampleWeights[i + 0] = 2.0f * AIR::GetGaussianDistribution( 0.0f, float(i + 0), 3.0f, 1.0f );
				fSampleWeights[i + 1] = 2.0f * AIR::GetGaussianDistribution( 0.0f, float(i + 1), 3.0f, 1.0f );
			}
		}

	}

    static inline void GetGaussianOffsets( 
        bool								bHorizontal, 
        const AIR::Vector2D&			vViewportTexelSize,
        AIRVector< AIR::Vector4 >&	vSampleOffsets, 
        AIRVector< Vector4 >&		vtSampleWeights, 
        int									nArraySize,
        float                               fCullWidth)
    {
        // Get the center texel offset and weight
        vtSampleWeights[0].x = 1.0f * AIR::GetGaussianDistribution( 0, 0, fCullWidth, 1.0f );
        vSampleOffsets[0] = AIR::Vector4( 0.0f, 0.0f, 0.0f, 0.0f );

        // Get the offsets and weights for the remaining taps
        if( bHorizontal ) 
        {
            for( int i = 1; i < nArraySize; i += 2 ) 
            {
                vSampleOffsets[i + 0] = AIR::Vector4(  i * vViewportTexelSize.x, 0.0f, 0.0f, 0.0f );
                vSampleOffsets[i + 1] = AIR::Vector4( -i * vViewportTexelSize.x, 0.0f, 0.0f, 0.0f );

                vtSampleWeights[i + 0].x = 2.0f * AIR::GetGaussianDistribution( float(i + 0), 0.0f, 3.0f, 1.0f );
                vtSampleWeights[i + 1].x = 2.0f * AIR::GetGaussianDistribution( float(i + 1), 0.0f, 3.0f, 1.0f );
            }
        }
        else 
        {
            for( int i = 1; i < nArraySize; i += 2 ) 
            {
                vSampleOffsets[i + 0] = AIR::Vector4( 0.0f,  i * vViewportTexelSize.y, 0.0f, 0.0f );
                vSampleOffsets[i + 1] = AIR::Vector4( 0.0f, -i * vViewportTexelSize.y, 0.0f, 0.0f );

                vtSampleWeights[i + 0].x = 2.0f * AIR::GetGaussianDistribution( 0.0f, float(i + 0), 3.0f, 1.0f );
                vtSampleWeights[i + 1].x = 2.0f * AIR::GetGaussianDistribution( 0.0f, float(i + 1), 3.0f, 1.0f );
            }
        }

    }
};

