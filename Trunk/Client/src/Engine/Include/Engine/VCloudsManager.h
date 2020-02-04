/********************************************************************
	author:		HJJ
	created:	2011:10:10
	purpose:	
*********************************************************************/

#pragma once

#ifndef __VCloudsManager_H__
#define __VCloudsManager_H__

#include "ColorGradient.h"
#include "Vector2D.h"
#include "VClouds.h"

class WeatherManager;

class EngineExport VCloudsManager
{
public:

/** Constructor
		    @param s Parent SkyX pointer
		 */
		VCloudsManager(WeatherManager *s);

		/** Destructor 
		 */
		~VCloudsManager();

		/** Create all resources
		 */
		void Create();

		/** Update
		    @param timeSinceLastFrame Time since last frame
		 */
		void Update(f32 timeSinceLastFrame);

		/** Remove all resources
		 */
		void remove();

		/** Update wind speed config
		 */
		void _updateWindSpeedConfig();

		/** Set ambient gradient
		    @param AmbientGradient Ambient color gradient
		 */
		inline void SetAmbientGradient(const ColorGradient& AmbientGradient)
		{
			mAmbientGradient = AmbientGradient;
		}

		/** Get ambient color gradient
		    @return Ambient color gradient
		 */
		inline const ColorGradient& GetAmbientGradient() const
		{
			return mAmbientGradient;
		}

		/** Set sun gradient
		    @param SunGradient Sun color gradient
		 */
		inline void SetSunGradient(const ColorGradient& SunGradient)
		{
			mSunGradient = SunGradient;
		}

		/** Get sun color gradient
		    @return Sun color gradient
		 */
		inline const ColorGradient& GetSunGradient() const
		{
			return mSunGradient;
		}

		/** Set height parameters
		    @param Height x = Cloud field y-coord start, y: Field height (both in world coordinates)
			@remarks Call it before create(), for now...
			         For autocalculated height based on the radius length set (-1,-1) as height params
		 */
		inline void SetHeight(const AIR::Vector2Df& Height)
		{
			mHeight = Height;
		}

		/** Get height parameters
		    @return Height: x = Cloud field y-coord start, y: Field height (both in world coordinates)
		 */
		inline const AIR::Vector2Df& GetHeight() const
		{
			return mHeight;
		}

		/** Set autoupdate
		    @Autoupdate true if you want to update volumetric clouds wind depending of the SkyX time multiplier
			@remarks You can use VClouds->setWindSpeed(..) for different winds speeds
		 */
		inline void SetAutoupdate( bool Autoupdate) 
		{
			mAutoupdate = Autoupdate;
			_updateWindSpeedConfig();
		}

		/** Get autoupdate
		    @return true if you want to update volumetric clouds wind depending of the SkyX time multiplier
		 */
		inline bool GetAutoupdate() 
		{
			return mAutoupdate;
		}

		/** Set wind speed
		    @param WindSpeed Wind speed
		 */
		inline void SetWindSpeed(f32 WindSpeed)
		{
			mWindSpeed = WindSpeed;
			_updateWindSpeedConfig();
		}

		/** Get wind speed
		    @return Wind speed
		 */
		inline f32 GetWindSpeed() 
		{
			return mWindSpeed;
		}
	
		/** Get VClouds
		 */
		inline VClouds* GetVClouds()
		{
			return mVClouds;
		}

		/** Is moon manager created?
		    @return true if yes, false if not
		 */
		inline bool IsCreated() 
		{
			return mCreated;
		}



	private:
		/** Set light parameters
		 */
		void _setLightParameters();

		/// Ambient and Sun color gradients
		ColorGradient mAmbientGradient;
		ColorGradient mSunGradient;

		/// VClouds pointer
		VClouds* mVClouds;

		/// Height parameters, x = Cloud field y-coord start, y: Field height (both in world coordinates)
		AIR::Vector2Df mHeight;

		/// Autoupdate wind speed depending of skyx time multiplier?
		bool mAutoupdate;
		/// Wind speed
		f32 mWindSpeed;

		/// Is moon manager created?
		bool mCreated;

		/// SkyX parent pointer
		WeatherManager *mSkyX;
};

#endif //__VCloudsManager_H__