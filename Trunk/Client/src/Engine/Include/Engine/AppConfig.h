#pragma once 

#include "Common.h"
#include "Dimension2D.h"
#include "EngineConfig.h"

// 图形设备类型
enum EDriverType
{
    DRIVER_D3D9				= 0,
    DRIVER_D3D10			= 1,
    DRIVER_OPENGL			= 2,
};


// 纹理质量类型
enum ETexQuality
{
	TEXQUALITY_POINT		= 0,
	TEXQUALITY_LINEAR		= 1,
	TEXQUALITY_BILINEAR		= 2,
	TEXQUALITY_TRILINEAR	= 3,
	TEXQUALITY_ANISOTROPY	= 4,
};


// 全屏反锯齿质量类型
enum EFSAA
{
	FSAA_0					= 0,
	FSAA_2					= 2,
	FSAA_4					= 4,
	FSAA_8					= 8,
	FSAA_16					= 16,
};


// 阴影质量类型
enum EShadowQuality
{
	SHADOWQUALITY_NONE		= 0,
	SHADOWQUALITY_LOW		= 1,
	SHADOWQUALITY_MEDIUM	= 2,
	SHADOWQUALITY_HIGHT		= 3,
	SHADOWQUALITY_ULTRA		= 4,
};


// 窗口显示模式
enum EWindowShow
{
	WS_WINDOW				= 0,
	WS_FULLSCREEN			= 1,
};


// 同屏角色数量限制
enum ECharLimit
{
	CHARLIMIT_0				= 0,
	CHARLIMIT_25			= 20,
	CHARLIMIT_50			= 50,
	CHARLIMIT_75			= 75,
	CHARLIMIT_100			= 100,
	CHARLIMIT_MAX			= 0xffffffff,
};


// 窗口分辨率
struct WinSize
{
	WinSize() : m_nWidth(800), m_nHeight(600) 
	{ 
		//sprintf_s( m_szDesc, 64, "800 * 600"); 
		snprintf(m_szDesc, 64, "800 * 600" );
	}

	WinSize( int width, int height ) : m_nWidth(width), m_nHeight(height) 
	{ 
		//sprintf_s( m_szDesc, 64, "%d * %d", width, height ); 
		snprintf(m_szDesc, 64, "%d * %d", width, height ); 
	}

	void operator = ( const WinSize& other ) 
	{ 
		m_nWidth = other.m_nWidth; 
		m_nHeight = other.m_nHeight; 
		//sprintf_s( m_szDesc, 64, "%d * %d", m_nWidth, m_nHeight ); 
		snprintf( m_szDesc, 64, "%d * %d", m_nWidth, m_nHeight ); 
	}

	int		m_nWidth;
	int		m_nHeight;
	char	m_szDesc[64];
};

const WinSize G_WinSizes[] =
{
	WinSize( 800,	600  ),
	WinSize( 1024,	768  ),
	WinSize( 1152,	864  ),
	WinSize( 1280,	768  ),
	WinSize( 1280,	800  ),
	WinSize( 1280,	960  ),
	WinSize( 1280,	1024 ),
	WinSize( 1360,	768	 ),
	WinSize( 1400,	900  ),
	WinSize( 1600,	900  ),
	WinSize( 1600,	1200 ),
	WinSize( 1680,	1050 ),
	WinSize( 1920,	1200 ),
	WinSize( 1920,	1440 ),
	WinSize( 2048,	1536 ),
};


typedef struct EngineExport tagAppConfig 
{
    tagAppConfig() : 
		m_nDeviceType(DRIVER_D3D9), 
		m_nMaxFramePerSecond(30), 
		m_nBackgroundColor(0x0), 
		m_bFullScreen(false), 
		m_bShowDebugConsoleWin(false),
		m_bUseNVPerfHud(false),
		m_nCharLimit(CHARLIMIT_MAX), 
		m_nCharAddCount(10),
		m_bUseEffect(false), 
		m_eTexQuality(TEXQUALITY_LINEAR),
		m_eFSAA(FSAA_0),
		m_eShadowQuality(SHADOWQUALITY_NONE)
        , m_fMeshFileVersion(0.1f)
        , m_bBinaryMeshFile(false)
        , m_bHardwareSkeleton(false)
		, m_bAutoGenLod(false)
		, m_bLoadPlant(true)
        , m_bBackgroundThread(true)
        , m_bNewTerrain(false)
		, m_nWndPosX(0)
		, m_nWndPosY(0)
		, m_bCenterWindow(false)
		, m_bAutoAlignWnd(false)
		, m_bEnableOC(false)
		, m_bCharacterCollision(false)
		, m_useVAO(false)
    {
		m_windowSize = G_WinSizes[1];
        strcpy_s(m_szWindowClass, "GraphicWindow");
        strcpy_s(m_szWindowTitle, "Client");
        strcpy_s(m_szSceneFile,   "Default");
		memset( m_szIconFile, 0, sizeof(m_szIconFile) );
        memset( m_szSpeedWindFile, 0, sizeof(m_szSpeedWindFile) );
		m_fMinFpsTor = 10.0;
		m_fMaxFrameTimeTor = 1.0f / m_fMinFpsTor;
        memset( m_szShaderPatch, 0, sizeof(m_szShaderPatch) );
		memset( m_szResourceDir, 0, sizeof(m_szResourceDir) );
        strcpy_s(m_szFont, "FreeType");
    }

    u32				m_nDeviceType;				//设备类型
    u32				m_nMaxFramePerSecond;		//每秒最大帧数
    u32				m_nBackgroundColor;			//背景颜色
    bool			m_bFullScreen;				//是否全屏显示
	bool			m_bShowDebugConsoleWin;		//是否显示调试控制台
	bool			m_bUseNVPerfHud;			//是否打开NVPERF调试工具
	int				m_nCharLimit;				//同屏显示角色数量限制
	int				m_nCharAddCount;			//测试每次往屏幕添加m_nCharAddCount个角色
	ETexQuality		m_eTexQuality;				//纹理质量	
	EFSAA			m_eFSAA;					//fsaa质量
	EShadowQuality	m_eShadowQuality;			//阴影质量
    bool			m_bUseEffect;				//使用effect
    char			m_szWindowClass[256];		//窗口类名
    char			m_szWindowTitle[256];		//窗口标题
    char			m_szSceneFile[256];			//默认场景文件
	char			m_szIconFile[256];			//程序图标文件
    WinSize			m_windowSize;				//窗口大小
	s32				m_nWndPosX;
	s32				m_nWndPosY;
	bool			m_bCenterWindow;
	bool			m_bAutoAlignWnd;
    float           m_fMeshFileVersion;			//模型网格文件的版本
    bool            m_bBinaryMeshFile;          //是否为二进制文件
    bool            m_bHardwareSkeleton;        //是否为基于vertexshader的骨骼动画
	bool			m_bAutoGenLod;				//是否自动创建lod
	int				m_nAutoLodLevelCount;		//自动创建lod的层数
	u8              m_byAutoLodType;			//自动创建lod的类型( 0-按固定的顶点数进行删减, 1-按比例进行删减 )
	float			m_fAutoLodParm;				//自动创建lod的创建参数类型( m_byAutoLodType = 0 时,表示删除的顶点数, m_byAutoLodType = 1时, 表示删除的比例 )
	bool			m_bAutoLodShareVB;			//自动创建lod时是否共享原始网格的顶点缓冲

	bool			m_bLoadPlant;

	float			m_fMinFpsTor;
	float			m_fMaxFrameTimeTor;

    bool            m_bBackgroundThread;   //后台IO线程

    char            m_szSpeedWindFile[256];     //speedwind配置文件

    bool            m_bNewTerrain;   //新地形方法，临时用的，实现后要去掉。
	char			m_szDefaultScene[256];//默认场景文件
	bool			m_bEnableOC;//是否打开遮挡剔除
    bool            m_bCharacterCollision;//是否开启角色模拟碰撞

    char            m_szShaderPatch[256];
	char			m_szResourceDir[256];
    char            m_szFont[256];

	bool			m_useVAO;

}APPCONFIG, *LPAPPCONFIG;
