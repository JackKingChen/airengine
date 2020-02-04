
#pragma once

#include "AISMultiTouch.h"

namespace AIS
{

	class AndroidMultiTouch : public MultiTouch
	{
	public:
		AndroidMultiTouch( InputManager* creator, bool buffered );
		virtual ~AndroidMultiTouch();

		virtual void setBuffered(bool buffered);

		virtual void capture();

		virtual Interface* queryInterface(Interface::IType type) {return 0;}

		virtual void _initialize();

	protected:
		float mContentScale;
	};

};
