#include "RenderWindow.h"
#include "Engine.h"

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32

IMPLEMENT_RTTI(RenderWindow, RenderTarget);

LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch (uMsg)
	{
	case WM_SIZE:

		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
	//return ENGINE_INST->MsgProc( hWnd, uMsg, wParam, lParam );
}

RenderWindow::RenderWindow(HWND hWnd) : m_hWnd(hWnd)
	, m_hInstance(0)
	, m_nWidth(0)
	, m_nHeight(0)
{
	//m_Priority = RTT_PRIORITY_DEFAULT;
	m_strName = "MainWindow";
	Create();
	m_bUseDepthSurface = true;
}

RenderWindow::RenderWindow(HINSTANCE instance, int nWidth, int nHeight) : m_hWnd(NULL)
	, m_hInstance(instance)
	, m_nWidth(nWidth)
	, m_nHeight(nHeight)
{
	//m_Priority = RTT_PRIORITY_DEFAULT;
	m_strName = "MainWindow";
	Create();
	m_bUseDepthSurface = true;
}

#endif

RenderWindow::RenderWindow() : m_hWnd(NULL), m_hInstance(NULL), m_nWidth(0)
	, m_nHeight(0)
{
	m_strName = "MainWindow";
}

RenderWindow::~RenderWindow()
{

}

bool RenderWindow::Create()
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	LPAPPCONFIG pConfig = ENGINE_INST->GetConfig();
	HICON hIcon = NULL;
	if ( pConfig->m_szIconFile )
	{
		hIcon = (HICON)LoadImage( m_hInstance, pConfig->m_szIconFile, IMAGE_ICON, 0, 0, LR_LOADFROMFILE );
	}
	if (m_hWnd == NULL)
	{
		WNDCLASSEX wcex;
		wcex.cbSize			= sizeof(WNDCLASSEX);
		wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wcex.lpfnWndProc	= (WNDPROC)WndProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= m_hInstance;
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName	= 0;
		wcex.lpszClassName	= pConfig->m_szWindowClass;
		wcex.hIcon			= hIcon;
		wcex.hIconSm		= hIcon;

		if (RegisterClassEx(&wcex) == 0)
		{
			//注册失败
			int nErrorCode = GetLastError();
			return false;
		}

		//DWORD dwWindowStyle = /*WS_OVERLAPPED |*/ WS_CAPTION | WS_SYSMENU | /*WS_THICKFRAME |*/ 
		//    WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE;
		DWORD dwWindowStyle = WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_BORDER | WS_SIZEBOX;
		DWORD dwWindowExtendedStyle = WS_EX_CLIENTEDGE;


		// Set the window's initial width
		int fx = GetSystemMetrics( SM_CXFULLSCREEN );
		int fy = GetSystemMetrics( SM_CYFULLSCREEN );
		RECT rcWindow;
		SetRect( &rcWindow, 0, 0, m_nWidth, m_nHeight );
		int nWindowWidth = rcWindow.right - rcWindow.left;
		int nWindowHeight = rcWindow.bottom - rcWindow.top;

		if ( pConfig->m_bFullScreen )
		{
			if ( FALSE == ChangeScreenResolution( pConfig->m_windowSize.m_nWidth, pConfig->m_windowSize.m_nHeight ) )
			{
				AdjustWindowRectEx( &rcWindow, dwWindowStyle, false, dwWindowExtendedStyle );
			}
			else
			{
				dwWindowStyle = WS_POPUPWINDOW;	
				dwWindowExtendedStyle = WS_EX_TOPMOST;					
			}
		}
		else
		{
			//调整window位置
			rcWindow.left	= (fx - nWindowWidth)/2;
			rcWindow.top	= (fy - nWindowHeight)/2;
			rcWindow.right	= rcWindow.left + nWindowWidth;
			rcWindow.bottom = rcWindow.top + nWindowHeight;
			AdjustWindowRectEx( &rcWindow, dwWindowStyle, false, dwWindowExtendedStyle );
		}

		// Create the render window
		m_hWnd = CreateWindowExA( 
			dwWindowExtendedStyle,
			pConfig->m_szWindowClass, 
			pConfig->m_szWindowTitle, 
			dwWindowStyle,
			rcWindow.left,
			rcWindow.top,
			nWindowWidth, 
			nWindowHeight, 
			NULL,
			NULL,
			m_hInstance, 
			NULL );

		if (m_hWnd == NULL)
		{
			DT_TO_LOG( LT_FATAL, "创建窗口失败！CreateWindowExA() Failed!" );
			int nErrorCode = GetLastError();
			//return false;
			return false;
		}

		GetClientRect(m_hWnd, &rcWindow);
		int nCurWidth = rcWindow.right - rcWindow.left;
		int nCurHeight = rcWindow.bottom - rcWindow.top;
		int cx = nWindowWidth + nWindowWidth - nCurWidth;
		int cy = nWindowHeight + nWindowHeight - nCurHeight;
		rcWindow.left	= (fx - cx) / 2;
		rcWindow.top	= (fy - cy) / 2;

		int px = rcWindow.left;
		int py = rcWindow.top;
		//if ( pConfig->m_bAutoAlignWnd == false )
		//{
		//    px = pConfig->m_nWndPosX;
		//    py = pConfig->m_nWndPosY;
		//}
		if( pConfig->m_bCenterWindow )
			SetWindowPos(m_hWnd, NULL, px, py, cx, cy, SWP_NOACTIVATE);
		else
			SetWindowPos(m_hWnd, NULL, pConfig->m_nWndPosX, pConfig->m_nWndPosY, cx, cy, SWP_NOACTIVATE);
		GetClientRect(m_hWnd, &rcWindow);
	}
	else
	{


	}
#endif

	return true;
}
