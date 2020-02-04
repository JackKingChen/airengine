
#pragma once

#ifdef AIS_WIN32_PLATFORM

#include "AISPrereqs.h"
#include "AISForceFeedback.h"
#include "win32/Win32Prereqs.h"

namespace AIS
{

	class Win32ForceFeedback : public ForceFeedback
	{
		Win32ForceFeedback() {}
	public:
		Win32ForceFeedback(IDirectInputDevice8* pDIJoy, const DIDEVCAPS* pDIJoyCaps);
		~Win32ForceFeedback();

		void upload( const Effect* effect );

		void modify( const Effect* effect );

		void remove( const Effect* effect );

		void setMasterGain( float level );
		
		void setAutoCenterMode( bool auto_on );

		short getFFAxesNumber();

		unsigned short getFFMemoryLoad();

		void _addEffectSupport( LPCDIEFFECTINFO pdei );

		void _addFFAxis();

	protected:
		void _updateConstantEffect( const Effect* effect );
		void _updateRampEffect( const Effect* effect );
		void _updatePeriodicEffect( const Effect* effect );
		void _updateConditionalEffect( const Effect* effect );
		void _updateCustomEffect( const Effect* effect );

		void _setCommonProperties( DIEFFECT* diEffect, DWORD* rgdwAxes,
									LONG* rglDirection, DIENVELOPE* diEnvelope, DWORD struct_size, 
									LPVOID struct_type, const Effect* effect, const Envelope* envelope );
		void _upload( GUID, DIEFFECT*, const Effect* );

		typedef std::map<int,LPDIRECTINPUTEFFECT> EffectList;
		EffectList			mEffectList;
		int					mHandles;
		IDirectInputDevice8	*mJoyStick;
		const DIDEVCAPS		*mpDIJoyCaps;
		short				mFFAxes;
	};

}

#endif //! AIS_WIN32_PLATFORM

