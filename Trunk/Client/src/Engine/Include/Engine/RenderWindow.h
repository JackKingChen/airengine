#pragma once

#include "RenderTarget.h"

class EngineExport RenderWindow : public RenderTarget
{
	DECLARE_RTTI;
	EngineAllocator(RenderWindow);
public:
	RenderWindow();
	virtual ~RenderWindow();

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32

	//没有创建window的情况
	RenderWindow(HINSTANCE instance, int nWidth, int nHeight);
	//已经由外部创建了window的情况
	RenderWindow(HWND hWnd);

	HWND GetHWnd() const
	{
		return m_hWnd;
	}
#endif

	virtual int GetWidth() const
	{
		return m_nWidth;
	}

	virtual int GetHeight() const
	{
		return m_nHeight;
	}

	virtual bool IsWindow()
	{
		return true;
	}

protected:
	virtual bool Create();

	virtual bool ChangeScreenResolution(int width,int height, int bitsPerPixel = 32 )
	{
		return true;
	}

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	HINSTANCE m_hInstance;
	HWND      m_hWnd;
#endif
	int       m_nWidth;
	int       m_nHeight;
};
