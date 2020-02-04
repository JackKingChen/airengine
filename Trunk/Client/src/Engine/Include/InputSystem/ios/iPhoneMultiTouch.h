
#pragma once

#include "AISMultiTouch.h"

#ifdef AIS_IPHONE_PLATFORM

#import <UIKit/UIKit.h>


namespace AIS
{
	class iPhoneMultiTouch : public MultiTouch
    {
	public:
		iPhoneMultiTouch( InputManager* creator, bool buffered );
		virtual ~iPhoneMultiTouch();

		virtual void setBuffered(bool buffered);

		virtual void capture();

		virtual Interface* queryInterface(Interface::IType type) {return 0;}

		virtual void _initialize();
	};
}

#endif //! AIS_IPHONE_PLATFORM
