
#include "stdafx.h"
#include "AISKeyboard.h"

using namespace AIS;

//----------------------------------------------------------------------//
void Keyboard::setTextTranslation( TextTranslationMode mode )
{
	mTextMode = mode;
}

//----------------------------------------------------------------------//
bool Keyboard::isModifierDown( Modifier mod ) const
{
#if defined(AIS_MSVC_COMPILER)
  #pragma warning (push)
  #pragma warning (disable : 4800)
#endif
	return (mModifiers & mod);
#if defined(AIS_MSVC_COMPILER)
  #pragma warning (pop)
#endif
}
