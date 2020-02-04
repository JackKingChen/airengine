#pragma once

#include "AISPrereqs.h"

namespace AIS
{

	//  An Object's interface is a way to gain write access to devices which support
	class AIR_INPUT_API Interface
	{
	public:
		virtual ~Interface() {};

		enum IType
		{
			ForceFeedback,
			Reserved
		};
	};


	class AIR_INPUT_API DeviceObject
	{
	public:
		virtual ~DeviceObject() {}

		AISDeviceType type() const { return mType; }

		const std::string& vendor() const { return mVendor; }

		virtual bool buffered() const { return mBuffered; }

		InputManager* getCreator() const { return mCreator; }

		virtual void setBuffered(bool buffered) = 0;

		virtual void capture() = 0;

		virtual int getID() const {return mDevID;}

		virtual Interface* queryInterface(Interface::IType type) = 0;

		virtual void _initialize() = 0;

	protected:
		DeviceObject(const std::string &vendor, AISDeviceType iType, bool buffered, int devID, InputManager* creator) :
					mVendor(vendor),
					mType(iType),
					mBuffered(buffered),
					mDevID(devID),
					mCreator(creator) {}

	protected:
		std::string		mVendor;
		AISDeviceType	mType;
		bool			mBuffered;
		int				mDevID;
		InputManager	*mCreator;
	};


}
