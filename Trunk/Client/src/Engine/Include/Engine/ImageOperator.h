//// ***************************************************************
//  文件: ImageOperator.h
//  日期: 2010-2-3
//  作者: huangjunjie
//  说明: 图像操作
// ***************************************************************


#pragma once

#ifdef _WIN32

#include "EngineConfig.h"
#include "ColorValue.h"
#include <Windows.h>
#include "MemoryAlloter.h"
#include "MemoryAllocatorConfig.h"
#include <map>
#include <string>
#include "ColorValue.h"
#include "Color.h"

#ifdef _MSC_VER
#ifdef USE_DEVIL

#pragma comment(lib, "ILUT.lib")
#pragma comment(lib, "ILU.lib")
#pragma comment(lib, "Devil.lib")

typedef	u32	RGBA;
typedef u32	ARGB;

class SColor;


 像素格式
enum PixelFormatDevil
{
	ePF_COLOR_INDEX			= 0x1900,   // 索引颜色
	ePF_ALPHA				= 0x1906,	// 单通道alpha
	ePF_RGB              	= 0x1907,	// 三通道r,g,b
	ePF_RGBA             	= 0x1908,	// 四通道r,g,b,a
	ePF_BGR              	= 0x80E0,	// 三通道b,g,r
	ePF_BGRA             	= 0x80E1,	// 四通道b,g,r,a
	ePF_LUMINANCE        	= 0x1909,	// 单通道灰度
	ePF_LUMINANCE_ALPHA  	= 0x190A,	// 双通道灰度+alpha
};


 图像编码格式
enum CodecType
{
	eCT_BYTE				= 0x1400,
	eCT_UNSIGNED_BYTE  		= 0x1401,
	eCT_SHORT          		= 0x1402,
	eCT_UNSIGNED_SHORT 		= 0x1403,
	eCT_INT            		= 0x1404,
	eCT_UNSIGNED_INT   		= 0x1405,
	eCT_FLOAT          		= 0x1406,
	eCT_DOUBLE         		= 0x140A,
	eCT_HALF           		= 0x140B,
};


 图像保存格式类型
enum ImgExtType
{
	eIET_UNKNOWN			= 0x0000,
	eIET_BMP         		= 0x0420,  //!< Microsoft Windows Bitmap - .bmp extension
	eIET_CUT         		= 0x0421,  //!< Dr. Halo - .cut extension
	eIET_DOOM        		= 0x0422,  //!< DooM walls - no specific extension
	eIET_DOOM_FLAT   		= 0x0423,  //!< DooM flats - no specific extension
	eIET_ICO         		= 0x0424,  //!< Microsoft Windows Icons and Cursors - .ico and .cur extensions
	eIET_JPG         		= 0x0425,  //!< JPEG - .jpg, .jpe and .jpeg extensions
	eIET_JFIF        		= 0x0425,  //!<
	eIET_LBM         		= 0x0426,  //!< Interlaced bitmap - .lbm extension
	eIET_PCD         		= 0x0427,  //!< Kodak PhotoCD - .pcd extension
	eIET_PCX         		= 0x0428,  //!< ZSoft PCX - .pcx extension
	eIET_PIC         		= 0x0429,  //!< PIC - .pic extension
	eIET_PNG         		= 0x042A,  //!< Portable Network Graphics - .png extension
	eIET_PNM         		= 0x042B,  //!< Portable Any Map - .pbm, .pgm, .ppm and .pnm extensions
	eIET_SGI         		= 0x042C,  //!< Silicon Graphics - .sgi, .bw, .rgb and .rgba extensions
	eIET_TGA         		= 0x042D,  //!< TrueVision Targa File - .tga, .vda, .icb and .vst extensions
	eIET_TIF         		= 0x042E,  //!< Tagged Image File Format - .tif and .tiff extensions
	eIET_CHEAD       		= 0x042F,  //!< C-Style Header - .h extension
	eIET_RAW         		= 0x0430,  //!< Raw Image Data - any extension
	eIET_MDL         		= 0x0431,  //!< Half-Life Model Texture - .mdl extension
	eIET_WAL         		= 0x0432,  //!< Quake 2 Texture - .wal extension
	eIET_LIF         		= 0x0434,  //!< Homeworld Texture - .lif extension
	eIET_MNG         		= 0x0435,  //!< Multiple-image Network Graphics - .mng extension
	eIET_JNG         		= 0x0435,  //!< 
	eIET_GIF         		= 0x0436,  //!< Graphics Interchange Format - .gif extension
	eIET_DDS         		= 0x0437,  //!< DirectDraw Surface - .dds extension
	eIET_DCX         		= 0x0438,  //!< ZSoft Multi-PCX - .dcx extension
	eIET_PSD         		= 0x0439,  //!< Adobe PhotoShop - .psd extension
	eIET_EXIF        		= 0x043A,  //!< 
	eIET_PSP         		= 0x043B,  //!< PaintShop Pro - .psp extension
	eIET_PIX         		= 0x043C,  //!< PIX - .pix extension
	eIET_PXR         		= 0x043D,  //!< Pixar - .pxr extension
	eIET_XPM         		= 0x043E,  //!< X Pixel Map - .xpm extension
	eIET_HDR         		= 0x043F,  //!< Radiance High Dynamic Range - .hdr extension
	eIET_ICNS				= 0x0440,  //!< Macintosh Icon - .icns extension
	eIET_JP2				= 0x0441,  //!< Jpeg 2000 - .jp2 extension
	eIET_EXR				= 0x0442,  //!< OpenEXR - .exr extension
	eIET_WDP				= 0x0443,  //!< 
	eIET_VTF				= 0x0444,  //!< Valve Texture Format - .vtf extension
	eIET_WBMP				= 0x0445,  //!< Wireless Bitmap - .wbmp extension
	eIET_SUN				= 0x0446,  //!< Sun Raster - .sun, .ras, .rs, .im1, .im8, .im24 and .im32 extensions
	eIET_IFF				= 0x0447,  //!< Interchange File Format - .iff extension
	eIET_TPL				= 0x0448,  //!< Gamecube Texture - .tpl extension
};


调色板类型
enum PaletteType
{
	ePAL_NONE				= 0x0400,	
	ePAL_RGB24				= 0x0401,
	ePAL_RGB32				= 0x0402,
	ePAL_RGBA32				= 0x0403,
	ePAL_BGR24				= 0x0404,
	ePAL_BGR32				= 0x0405,
	ePAL_BGRA32				= 0x0406,
};


 图像起始点的格式
enum OriginType
{
	eOT_ORIGIN_LOWER_LEFT	= 0x0601,
	eOT_ORIGIN_UPPER_LEFT	= 0x0602,
};


 立方贴图标记
enum CubeFlag
{
	eCF_CUBEMAP_POSITIVEX	= 0x00000400,
	eCF_CUBEMAP_NEGATIVEX	= 0x00000800,
	eCF_CUBEMAP_POSITIVEY	= 0x00001000,
	eCF_CUBEMAP_NEGATIVEY	= 0x00002000,
	eCF_CUBEMAP_POSITIVEZ	= 0x00004000,
	eCF_CUBEMAP_NEGATIVEZ	= 0x00008000,
	eCF_SPHEREMAP			= 0x00010000,
};


 缩放参数类型
enum ScaleParmName
{
	eSPN_FILTER				= 0x2600,
	eSPN_PLACEMENT			= 0x0700,
};


 缩放参数值
enum ScaleParmValue
{
	 filter value
	eSPV_NEAREST			= 0x2601,
	eSPV_LINEAR				= 0x2602,
	eSPV_BILINEAR			= 0x2603,
	eSPV_SCALE_BOX			= 0x2604,
	eSPV_SCALE_TRIANGLE		= 0x2605,
	eSPV_SCALE_BELL			= 0x2606,
	eSPV_SCALE_BSPLINE		= 0x2607,
	eSPV_SCALE_LANCZOS3		= 0x2608,
	eSPV_SCALE_MITCHELL		= 0x2609,

	 placement value
	eSPV_LOWER_LEFT         = 0x0701,
	eSPV_LOWER_RIGHT		= 0x0702,
	eSPV_UPPER_LEFT			= 0x0703,
	eSPV_UPPER_RIGHT		= 0x0704,
	eSPV_CENTER				= 0x0705,
	eSPV_CONVOLUTION_MATRIX	= 0x0710,
};



// 图像操作器
class EngineExport ImageOperator// : public EngineAllocator(ImageOperator)
{
	EngineAllocator(ImageOperator)
public:
	ImageOperator();
	~ImageOperator();

	 从文件加载图像
	bool		LoadImageFromFile( const char* szFile, bool bFullPath = false );
	 保存图像到文件
	bool		SaveImageToFile( const char* szFile );
	 如果图像像素是RGBA格式,返回的是RGBA的u32数值
	u32		GetPixelAsInt( int nX, int nY, int nZ = 0 );
	 如果图像像素是RGBA格式,返回的是RGBA的u8数组
	u8*		GetPixelAsByteArray( int nX, int nY, int nZ = 0 );
	
	u8*		GetPixelAsByteArray( u32 id );

    获得浮点像素
    float       GetPixelAsFloat(int nX, int nY, int nZ = 0);

	 如果图像像素是RGBA格式,pValue的整数类型是ARGB
	void		SetPixel( u32 value, int nX, int nY, int nZ = 0 );
	 如果图像像素是RGBA格式,pValue的输入顺序是A,R,G,B
	void		SetPixel( u8* pValue, int nX, int nY, int nZ = 0 );
	 如果是4通道的颜色,用这个方法设置某个像素点的颜色值最快
	void		SetPixel( const SColor& col, int nX, int nY, int nZ = 0 );	
	 拷贝矩形像素
	size_t		CopyPixelDataFromImage( int nOffsetX, int nOffsetY, int nWidth, int nHeight, void** ppData );

    拷贝像素到矩阵
    bool        CopyPixelDataToImage(int nOffsetX, int nOffsetY, int nWidth, int nHeight, void* pData);

	 重置
	void		ResetForDynamicImageOpt( void );

	 均匀模糊
	void		BlurAvg( u32 iter );
	 高斯模糊
	void		BlurGaussian( u32 iter );
	 设置缩放类型以及值
	void		SetScaleParm( ScaleParmName parm, ScaleParmValue value );
	 缩放
	void		ScaleSize( int nWidth, int nHeight, int nDepth = 1);
	 边缘检测
	void		EdgeDetectE( void );
	void		EdgeDetectP( void );
	void		EdgeDetectS( void );
	 伽马校正
	void		GammaCorrect( float gama );
	 翻转图像
	void		FlipImage( void );
	 镜像图像
	void		Mirror( void );
	 像素取反
	void		Negative( void );
	 噪声处理
	void		Noisify( float tolerance );
	 色块化
	void		Pixelize( u32 dwPixelSize );
	 图像锐化
	void		Sharpen( float Factor, u32 Iter);
	 交换颜色
	void		SwapColours( void ); 
	 颜色缩放
	void		ScaleColours( float r, float g, float b );
	 旋转
	void		Rotate(float Angle);
	

    void        SetData(float* pData, u32 size);
	 设置图像数据
	void		SetData( u8* pData, u32 size );
	 设置图像宽度
	void		SetImageWidth( int nWidth );
	 设置图像高度
	void		SetImageHeight( int nHeight );
	 设置图像深度
	void		SetImageDepth( int nDepth );
	 设置图像每像素的字节数
	void		SetBytePerPixel( u8 byBPP );
	 设置所有像素的数据大小
	void		SetSizeOfData( u32 size );
	 设置像素格式
	void		SetPixelFormat( PixelFormatDevil pf );
	 设置像素数据的内部保存格式
	void		SetCodecType( CodecType codectype );
	 设置像素起点类型
	void		SetOriginType( OriginType origintype );
	 如果是立方贴图,设置立方贴图的面的类型
	void		SetCubeFlag( CubeFlag cudeflag );
	 设置mipmap的数量
	void		SetNumMips( int nMips );
	 设置图层的数量
	void		SetNumLayers( int nLayers );
	void		SetPaletteData( u8* pData, u32 size );
	void		SetPaletteType( PaletteType pt );
	void		SetPaletteDataSize( u32 size );
	void		SetNumNext( int numNext );


	 获得像素数据指针
	void*		GetData( void );
	 获得图像宽度
	int			GetImageWidth( void );
	 获得图像高度
	int			GetImageHeight( void );
	 获得图像深度
	int			GetImageDepth( void );
	 获得图像每像素的字节数
	u8		GetBytePerPixel( void );
	 获得所有像素的数据大小
	u32		GetSizeOfData( void );
	 获得像素格式
	PixelFormatDevil	GetPixelFormat( void );
	 获得像素数据的内部保存格式
	CodecType	GetCodecType( void );
	 获得像素起点类型
	OriginType	GetOriginType( void );
	 如果是立方贴图,获取立方贴图的面的类型
	CubeFlag	GetCubeFlag( void );
	 获得mipmap的数量
	int			GetNumMips( void );
	 获得图层的数量
	int			GetNumLayers( void );

	void*		GetPaletteData( void );

	PaletteType GetPaletteType( void );

	u32		GetPaletteDataSize( void );

	int			GetNumNexe( void );

    获得颜色
    CColorValue GetColourAt(int x, int y);

    格式转换
    bool		ConvertImage(PixelFormatDevil pfDest, CodecType ctDest);

    const AIRString& GetName() const
    {
        return m_strName;
    }

    void SetName(const char* szName)
    {
        m_strName = szName;
    }

    HBITMAP ConvertToHBITMAP(HDC hDC);


	size_t GetCodeTypeByteSize( CodecType ct );

    u32 GetPixelAsRGBA(int nX, int nY, int nZ = 0) const;

    u32 GetPixelAsARGB(int nX, int nY, int nZ = 0) const;

	void UsePalette( u8* pData, u32 size, PaletteType pt );

	bool SavePaletteFile( const char* szPalFile );

protected:
	void*			m_pData;				// 数据指针
	int				m_nWidth;				// 图像宽度
	int				m_nHeight;				// 图像高度
	int				m_nDepth;				// 图像深度
	u8			m_byBPP;				// 每像素的字节数
	u32			m_dwSizeOfData;			// 像素数据总大小
	PixelFormatDevil		m_PixelFormat;			// 像素格式
	CodecType		m_CodecType;			// 图像文件的保存格式
	OriginType		m_OriginType;			// 图像像素排列类型
	u8*			m_pPelette;				// 调色板数据
	PaletteType		m_PaletteType;			// 调色板类型
	u32			m_PalSize;				// 调色板数据大小
	CubeFlag		m_CubeFlag;				// 如果是立方贴图,记录是哪个面
	int				m_nNumNext;				// number of images following
	int				m_nNumMips;				// mipmaps数量
	int				m_nNumLayers;			// 图像的层次数量
	unsigned int	m_dwImgName;			// IL图像名称
	ScaleParmValue  m_ScaleValueFilter;		// 缩放过滤值
	ScaleParmValue	m_ScaleValuePlacement;	// 缩放放置类型值
    AIRString     m_szFile;               // 加载的文件名
    AIRString     m_strName;              // 名字
};//! end class ImageOperator



class EngineExport ImageOperatorMgr
{
private:
	ImageOperatorMgr();

public:
	~ImageOperatorMgr();

	 创建图像操作器并加载文件
	ImageOperator*				CreateImageOperatorFromFile( const char* szName, const char* szFile, bool bFullPath = false );

	 创建一个动态的图像操作器
	ImageOperator*				CreateDynamicImageOperator( const char* szName );

	 释放图像操作器
	bool						ReleaseImageOperator( const char* szName );

    释放
    bool                        ReleaseImageOperator( ImageOperator* pImageOperator);

	 获取图像操作器
	ImageOperator*				GetImageOperator( const char* szName );

	 获得图像格式类型
	ImgExtType					GetImgExtType( const AIRString& szExt );

	 获得对象实例
	static ImageOperatorMgr*	GetSingletonPtr( void );

    判断后缀名
    static bool					JudgeImageExt(const char* szExt);

    获得当前图像的数据
    static void* GetCurrentImageData();

protected:
     释放
    void						Release( void );

	 初始化
	void						Init( void );

	

protected:
    typedef std::map<AIRString, ImageOperator*>	MAP_IMAGEOPERATOR;
	typedef MAP_IMAGEOPERATOR::iterator				MAP_IMAGEOPERATOR_ITOR;

	MAP_IMAGEOPERATOR					m_mapImageOperator; // ImageOperator对象容器
	std::map<AIRString, ImgExtType>	m_mapImgExtType;	// 图像格式类型容器

};//! end class ImageOperatorMgr


#define IMAGEOPERATORMGR (ImageOperatorMgr::GetSingletonPtr())

#endif
#endif

#endif
/*
//----------------------------------------------------------------
// test code:
// 1> 		
ImageOperator* pIO = IMAGEOPERATORMGR->CreateImageOperatorFromFile( "2DMap.jpg", "2DMap.jpg" );
if ( pIO )
{
	pIO->ScaleSize( 400, 300 );
	pIO->SaveImageToFile( "E:\\2DMap.jpg" );
}

// 2>
ImageOperator* pIO = IMAGEOPERATORMGR->CreateDynamicImageOperator( "test" );
if ( pIO )
{
	u32 size = 128*128*4;
	pIO->SetImageDepth(1);
	pIO->SetImageHeight( 128 );
	pIO->SetImageWidth( 128 );
	pIO->SetNumMips(1);
	pIO->SetOriginType(eOT_ORIGIN_UPPER_LEFT);
	pIO->SetSizeOfData( size );
	pIO->SetCodecType( eCT_UNSIGNED_BYTE );
	pIO->SetBytePerPixel( 4 );
	pIO->SetPixelFormat( ePF_RGBA );
	u8* pData = AURO_NEW_VERBOSE u8[size];
	memset( pData, 255, size );
	pIO->SetData( pData, size );
	for ( int i = 0; i < 64; ++i )
	{
		u8 a = 255-i*3;
		for ( int j = 0; j < 128; ++j )
		{
			u8 color[4] = { a, 255, 0, 0 };
			pIO->SetPixel( color, j,i );
		}
	}
	pIO->ScaleSize( 400, 300 );
	pIO->SaveImageToFile( "E:\\2DMap.jpg" );
	SafeDeleteArray( pData );
}
//----------------------------------------------------------------
*/
