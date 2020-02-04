#pragma once 

#include "Common.h"
#include "Dimension2D.h"
#include "EngineConfig.h"

// ͼ���豸����
enum EDriverType
{
    DRIVER_D3D9				= 0,
    DRIVER_D3D10			= 1,
    DRIVER_OPENGL			= 2,
};


// ������������
enum ETexQuality
{
	TEXQUALITY_POINT		= 0,
	TEXQUALITY_LINEAR		= 1,
	TEXQUALITY_BILINEAR		= 2,
	TEXQUALITY_TRILINEAR	= 3,
	TEXQUALITY_ANISOTROPY	= 4,
};


// ȫ���������������
enum EFSAA
{
	FSAA_0					= 0,
	FSAA_2					= 2,
	FSAA_4					= 4,
	FSAA_8					= 8,
	FSAA_16					= 16,
};


// ��Ӱ��������
enum EShadowQuality
{
	SHADOWQUALITY_NONE		= 0,
	SHADOWQUALITY_LOW		= 1,
	SHADOWQUALITY_MEDIUM	= 2,
	SHADOWQUALITY_HIGHT		= 3,
	SHADOWQUALITY_ULTRA		= 4,
};


// ������ʾģʽ
enum EWindowShow
{
	WS_WINDOW				= 0,
	WS_FULLSCREEN			= 1,
};


// ͬ����ɫ��������
enum ECharLimit
{
	CHARLIMIT_0				= 0,
	CHARLIMIT_25			= 20,
	CHARLIMIT_50			= 50,
	CHARLIMIT_75			= 75,
	CHARLIMIT_100			= 100,
	CHARLIMIT_MAX			= 0xffffffff,
};


// ���ڷֱ���
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

    u32				m_nDeviceType;				//�豸����
    u32				m_nMaxFramePerSecond;		//ÿ�����֡��
    u32				m_nBackgroundColor;			//������ɫ
    bool			m_bFullScreen;				//�Ƿ�ȫ����ʾ
	bool			m_bShowDebugConsoleWin;		//�Ƿ���ʾ���Կ���̨
	bool			m_bUseNVPerfHud;			//�Ƿ��NVPERF���Թ���
	int				m_nCharLimit;				//ͬ����ʾ��ɫ��������
	int				m_nCharAddCount;			//����ÿ������Ļ���m_nCharAddCount����ɫ
	ETexQuality		m_eTexQuality;				//��������	
	EFSAA			m_eFSAA;					//fsaa����
	EShadowQuality	m_eShadowQuality;			//��Ӱ����
    bool			m_bUseEffect;				//ʹ��effect
    char			m_szWindowClass[256];		//��������
    char			m_szWindowTitle[256];		//���ڱ���
    char			m_szSceneFile[256];			//Ĭ�ϳ����ļ�
	char			m_szIconFile[256];			//����ͼ���ļ�
    WinSize			m_windowSize;				//���ڴ�С
	s32				m_nWndPosX;
	s32				m_nWndPosY;
	bool			m_bCenterWindow;
	bool			m_bAutoAlignWnd;
    float           m_fMeshFileVersion;			//ģ�������ļ��İ汾
    bool            m_bBinaryMeshFile;          //�Ƿ�Ϊ�������ļ�
    bool            m_bHardwareSkeleton;        //�Ƿ�Ϊ����vertexshader�Ĺ�������
	bool			m_bAutoGenLod;				//�Ƿ��Զ�����lod
	int				m_nAutoLodLevelCount;		//�Զ�����lod�Ĳ���
	u8              m_byAutoLodType;			//�Զ�����lod������( 0-���̶��Ķ���������ɾ��, 1-����������ɾ�� )
	float			m_fAutoLodParm;				//�Զ�����lod�Ĵ�����������( m_byAutoLodType = 0 ʱ,��ʾɾ���Ķ�����, m_byAutoLodType = 1ʱ, ��ʾɾ���ı��� )
	bool			m_bAutoLodShareVB;			//�Զ�����lodʱ�Ƿ���ԭʼ����Ķ��㻺��

	bool			m_bLoadPlant;

	float			m_fMinFpsTor;
	float			m_fMaxFrameTimeTor;

    bool            m_bBackgroundThread;   //��̨IO�߳�

    char            m_szSpeedWindFile[256];     //speedwind�����ļ�

    bool            m_bNewTerrain;   //�µ��η�������ʱ�õģ�ʵ�ֺ�Ҫȥ����
	char			m_szDefaultScene[256];//Ĭ�ϳ����ļ�
	bool			m_bEnableOC;//�Ƿ���ڵ��޳�
    bool            m_bCharacterCollision;//�Ƿ�����ɫģ����ײ

    char            m_szShaderPatch[256];
	char			m_szResourceDir[256];
    char            m_szFont[256];

	bool			m_useVAO;

}APPCONFIG, *LPAPPCONFIG;
