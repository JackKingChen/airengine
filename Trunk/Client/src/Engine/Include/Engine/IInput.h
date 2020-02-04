#pragma once
#include "Common.h"
#include "Rect.h"

#include "EventReader.h"
#include "MemoryAlloter.h"

/*
** AIR Virtual-key codes
*/
enum AIR_KEY_CODE
{
    AIRK_LBUTTON	= 0x01,
    AIRK_RBUTTON	= 0x02,
    AIRK_MBUTTON	= 0x04,

    AIRK_ESCAPE		= 0x1B,
    AIRK_BACKSPACE	= 0x08,
    AIRK_TAB		= 0x09,
    AIRK_ENTER		= 0x0D,
    AIRK_SPACE		= 0x20,

    AIRK_SHIFT		= 0x10,
    AIRK_CTRL		= 0x11,
    AIRK_ALT		= 0x12,

    AIRK_LWIN		= 0x5B,
    AIRK_RWIN		= 0x5C,
    AIRK_APPS		= 0x5D,

    AIRK_PAUSE		= 0x13,
    AIRK_CAPSLOCK	= 0x14,
    AIRK_NUMLOCK	= 0x90,
    AIRK_SCROLLLOCK	= 0x91,

    AIRK_PGUP		= 0x21,
    AIRK_PGDN		= 0x22,
    AIRK_HOME		= 0x24,
    AIRK_END		= 0x23,
    AIRK_INSERT		= 0x2D,
    AIRK_DELETE		= 0x2E,

    AIRK_LEFT		= 0x25,
    AIRK_UP			= 0x26,
    AIRK_RIGHT		= 0x27,
    AIRK_DOWN		= 0x28,

    AIRK_0			= 0x30,
    AIRK_1			= 0x31,
    AIRK_2			= 0x32,
    AIRK_3			= 0x33,
    AIRK_4			= 0x34,
    AIRK_5			= 0x35,
    AIRK_6			= 0x36,
    AIRK_7			= 0x37,
    AIRK_8			= 0x38,
    AIRK_9			= 0x39,

    AIRK_A			= 0x41,
    AIRK_B			= 0x42,
    AIRK_C			= 0x43,
    AIRK_D			= 0x44,
    AIRK_E			= 0x45,
    AIRK_F			= 0x46,
    AIRK_G			= 0x47,
    AIRK_H			= 0x48,
    AIRK_I			= 0x49,
    AIRK_J			= 0x4A,
    AIRK_K			= 0x4B,
    AIRK_L			= 0x4C,
    AIRK_M			= 0x4D,
    AIRK_N			= 0x4E,
    AIRK_O			= 0x4F,
    AIRK_P			= 0x50,
    AIRK_Q			= 0x51,
    AIRK_R			= 0x52,
    AIRK_S			= 0x53,
    AIRK_T			= 0x54,
    AIRK_U			= 0x55,
    AIRK_V			= 0x56,
    AIRK_W			= 0x57,
    AIRK_X			= 0x58,
    AIRK_Y			= 0x59,
    AIRK_Z			= 0x5A,

    AIRK_GRAVE		= 0xC0,
    AIRK_MINUS		= 0xBD,
    AIRK_EQUALS		= 0xBB,
    AIRK_BACKSLASH	= 0xDC,
    AIRK_LBRACKET	= 0xDB,
    AIRK_RBRACKET	= 0xDD,
    AIRK_SEMICOLON	= 0xBA,
    AIRK_APOSTROPHE	= 0xDE,
    AIRK_COMMA		= 0xBC,
    AIRK_PERIOD		= 0xBE,
    AIRK_SLASH		= 0xBF,

    AIRK_NUMPAD0	= 0x60,
    AIRK_NUMPAD1	= 0x61,
    AIRK_NUMPAD2	= 0x62,
    AIRK_NUMPAD3	= 0x63,
    AIRK_NUMPAD4	= 0x64,
    AIRK_NUMPAD5	= 0x65,
    AIRK_NUMPAD6	= 0x66,
    AIRK_NUMPAD7	= 0x67,
    AIRK_NUMPAD8	= 0x68,
    AIRK_NUMPAD9	= 0x69,

    AIRK_MULTIPLY	= 0x6A,
    AIRK_DIVIDE		= 0x6F,
    AIRK_ADD		= 0x6B,
    AIRK_SUBTRACT	= 0x6D,
    AIRK_DECIMAL	= 0x6E,

    AIRK_F1			= 0x70,
    AIRK_F2			= 0x71,
    AIRK_F3			= 0x72,
    AIRK_F4			= 0x73,
    AIRK_F5			= 0x74,
    AIRK_F6			= 0x75,
    AIRK_F7			= 0x76,
    AIRK_F8			= 0x77,
    AIRK_F9			= 0x78,
    AIRK_F10		= 0x79,
    AIRK_F11		= 0x7A,
    AIRK_F12		= 0x7B
};



class EngineExport IInput// : public EngineAllocator(IInput)
{
	EngineAllocator(IInput)
public:
    IInput() {};
    virtual ~IInput() {};

    virtual bool Initialize( size_t hWnd ) = 0;
    virtual void Update() = 0;
    virtual bool LButtonDown() const = 0;
    virtual bool RButtonDown() const = 0;
	virtual bool MButtonDown() const = 0;
    virtual bool LButtonUp() const = 0;
    virtual bool RButtonUp() const = 0;
	virtual bool MButtonUp() const = 0;
    virtual bool KeyDown(const u8 cKey) const = 0;
    virtual bool KeyUp(const u8 cKey) const = 0;
    virtual void GetMovement(s32& dx, s32& dy) = 0;
    virtual void GetMousePosInWindow(s32& nX, s32& nY) = 0;
    virtual bool IsMouseInRect(const AIR::Rect& rect) = 0;
    virtual void Release() = 0;
    virtual CInputEventList* GetEventList() = 0;

	virtual bool LastLButtonDown() const = 0;
	virtual bool LastRButtonDown() const = 0;
    //virtual bool LastMButtonDowm() const = 0;
	virtual bool LastLButtonUp() const = 0;
	virtual bool LastRButtonUp() const = 0;
    virtual bool LastMButtonUp() const = 0;
	virtual bool KeyLastDown(const u8 cKey) const = 0;
	virtual bool KeyLastUp(const u8 cKey) const = 0;
protected:
private:
};
