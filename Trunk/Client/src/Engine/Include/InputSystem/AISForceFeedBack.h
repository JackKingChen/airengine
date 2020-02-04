
#pragma once

#include "AISDeviceObject.h"


namespace AIS
{
	
	class ForceEffect;
	class ConstantEffect;
	class RampEffect;
	class PeriodicEffect;
	class ConditionalEffect;


	class AIR_INPUT_API Effect
	{
		Effect();

	public:
		enum EForce
		{
			UnknownForce = 0,
			ConstantForce,	
			RampForce,		
			PeriodicForce,	
			ConditionalForce,	
			CustomForce,
			_ForcesNumber	
		};

		static const char* getForceTypeName(EForce eValue);

		enum EType
		{
			Unknown = 0, // UnknownForce
			Constant,    // ConstantForce
			Ramp,        // RampForce
			Square,      // PeriodicForce
			Triangle,    // PeriodicForce
            Sine,        // PeriodicForce
			SawToothUp,  // PeriodicForce
			SawToothDown,// PeriodicForce
			Friction,    // ConditionalForce
			Damper,      // ConditionalForce
			Inertia,     // ConditionalForce
			Spring,      // ConditionalForce
			Custom,      // CustomForce
			_TypesNumber // Always keep in last position.
		};

		static const char* getEffectTypeName(EType eValue);

		enum EDirection
		{
			NorthWest,
			North,
			NorthEast,
			East,
			SouthEast,
			South,
			SouthWest,
			West,
			_DirectionsNumber
		};

		static const char* getDirectionName(EDirection eValue);

		Effect(EForce ef, EType et);
		virtual ~Effect();

		ForceEffect* getForceEffect() const;
		void setNumAxes(short nAxes);
		short getNumAxes() const;


	public:
		//Infinite Time
		static const unsigned int AIS_INFINITE = 0xFFFFFFFF;

		const EForce	force;
		const EType		type;

		//-------------------------------------------------------------------//
		//--- Set these variables before uploading or modifying an effect ---//

		//Direction to apply to the force - affects two axes+ effects
		EDirection		direction;

		//Number of button triggering an effect (-1 means no trigger)
		short			trigger_button;

		//Time to wait before an effect can be re-triggered (microseconds)
		unsigned int	trigger_interval;

		//Duration of an effect (microseconds)
		unsigned int	replay_length;

		//Time to wait before to start playing an effect (microseconds)
		unsigned int	replay_delay;

		//------------- Library Internal -------------------------------------//
		mutable int		_handle;

	protected:
		ForceEffect		*effect; //Properties depend on EForce
		short			axes;    //Number of axes to use in effect
	};

	//-----------------------------------------------------------------------------//

	class AIR_INPUT_API ForceEffect
	{
	public:
		virtual ~ForceEffect() {}
	};

	//-----------------------------------------------------------------------------//
	/**
		An optional envelope to be applied to the start/end of an effect. If any of
		these values are nonzero, then the envelope will be used in setting up the
		effect.
	*/
	class AIR_INPUT_API Envelope : public ForceEffect
	{
	public:
		Envelope() : attackLength(0), attackLevel(0), fadeLength(0), fadeLevel(0) {}
#if defined(AIS_MSVC_COMPILER)
  #pragma warning (push)
  #pragma warning (disable : 4800)
#endif
		bool isUsed() const { return attackLength | attackLevel | fadeLength | fadeLevel; }
#if defined(AIS_MSVC_COMPILER)
  #pragma warning (pop)
#endif

		// Duration of the attack (microseconds)
		unsigned int	attackLength;

		// Absolute level at the beginning of the attack (0 to 10K)
		// (automatically signed when necessary by FF core according to effect level sign)
		unsigned short	attackLevel;

		// Duration of fade (microseconds)
		unsigned int	fadeLength;

		// Absolute level at the end of fade (0 to 10K)
		// (automatically signed when necessary by FF core according to effect level sign)
		unsigned short	fadeLevel;
	};

	//-----------------------------------------------------------------------------//

	class AIR_INPUT_API ConstantEffect : public ForceEffect
	{
	public:
		ConstantEffect() : level(5000) {}

		class Envelope envelope; //Optional envolope
		signed short level;       //-10K to +10k
	};

	//-----------------------------------------------------------------------------//

	class AIR_INPUT_API RampEffect : public ForceEffect
	{
	public:
		RampEffect() : startLevel(0), endLevel(0) {}

        class Envelope envelope; //Optional envelope
		signed short startLevel;  //-10K to +10k
		signed short endLevel;    //-10K to +10k
	};

	//-----------------------------------------------------------------------------//

	class AIR_INPUT_API PeriodicEffect : public ForceEffect
	{
	public:
		PeriodicEffect() : magnitude(0), offset(0), phase(0), period(0) {}

		class Envelope envelope;  //Optional Envelope

		unsigned short magnitude;  //0 to 10,0000
		signed short   offset;
		unsigned short phase;      //Position at which playback begins 0 to 35,999
		unsigned int   period;     //Period of effect (microseconds)
	};

	//-----------------------------------------------------------------------------//

	class AIR_INPUT_API ConditionalEffect : public ForceEffect
	{
	public:
		ConditionalEffect() :
            rightCoeff(0), leftCoeff(0), rightSaturation(0), leftSaturation(0),
			deadband(0), center(0) {}

		signed short	rightCoeff;      //-10k to +10k (Positive Coeff)
		signed short	leftCoeff;       //-10k to +10k (Negative Coeff)

		unsigned short	rightSaturation; //0 to 10k (Pos Saturation)
		unsigned short	leftSaturation;  //0 to 10k (Neg Saturation)

		//Region around center in which the condition is not active, in the range
		//from 0 through 10,000
		unsigned short	deadband;

		//(Offset in DX) -10k and 10k
		signed short	center;
	};



	//--------------------------------------------------------------------------------------------------

	class AIR_INPUT_API ForceFeedback : public Interface
	{
	public:
		ForceFeedback();
		virtual ~ForceFeedback() {}

		virtual void setMasterGain( float level ) = 0;

		virtual void setAutoCenterMode( bool auto_on ) = 0;

		virtual void upload( const Effect* effect ) = 0;

		virtual void modify( const Effect* effect ) = 0;

		virtual void remove( const Effect* effect ) = 0;

        virtual short getFFAxesNumber() = 0;

		virtual unsigned short getFFMemoryLoad() = 0;

		typedef std::multimap<Effect::EForce, Effect::EType> SupportedEffectList;

		const SupportedEffectList& getSupportedEffects() const;

		bool supportsEffect(Effect::EForce force, Effect::EType type) const;

		void _addEffectTypes( Effect::EForce force, Effect::EType type );
		void _setGainSupport( bool on );
		void _setAutoCenterSupport( bool on );

	protected:
		SupportedEffectList mSupportedEffects;
		bool				mSetGainSupport;
		bool				mSetAutoCenterSupport;
	};


}

