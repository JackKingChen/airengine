//// ***************************************************************
//  �ļ�: ImageOperator.h
//  ����: 2010-2-3
//  ����: huangjunjie
//  ˵��: ͼ�����
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


 ���ظ�ʽ
enum PixelFormatDevil
{
	ePF_COLOR_INDEX			= 0x1900,   // ������ɫ
	ePF_ALPHA				= 0x1906,	// ��ͨ��alpha
	ePF_RGB              	= 0x1907,	// ��ͨ��r,g,b
	ePF_RGBA             	= 0x1908,	// ��ͨ��r,g,b,a
	ePF_BGR              	= 0x80E0,	// ��ͨ��b,g,r
	ePF_BGRA             	= 0x80E1,	// ��ͨ��b,g,r,a
	ePF_LUMINANCE        	= 0x1909,	// ��ͨ���Ҷ�
	ePF_LUMINANCE_ALPHA  	= 0x190A,	// ˫ͨ���Ҷ�+alpha
};


 ͼ������ʽ
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


 ͼ�񱣴��ʽ����
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


��ɫ������
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


 ͼ����ʼ��ĸ�ʽ
enum OriginType
{
	eOT_ORIGIN_LOWER_LEFT	= 0x0601,
	eOT_ORIGIN_UPPER_LEFT	= 0x0602,
};


 ������ͼ���
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


 ���Ų�������
enum ScaleParmName
{
	eSPN_FILTER				= 0x2600,
	eSPN_PLACEMENT			= 0x0700,
};


 ���Ų���ֵ
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



// ͼ�������
class EngineExport ImageOperator// : public EngineAllocator(ImageOperator)
{
	EngineAllocator(ImageOperator)
public:
	ImageOperator();
	~ImageOperator();

	 ���ļ�����ͼ��
	bool		LoadImageFromFile( const char* szFile, bool bFullPath = false );
	 ����ͼ���ļ�
	bool		SaveImageToFile( const char* szFile );
	 ���ͼ��������RGBA��ʽ,���ص���RGBA��u32��ֵ
	u32		GetPixelAsInt( int nX, int nY, int nZ = 0 );
	 ���ͼ��������RGBA��ʽ,���ص���RGBA��u8����
	u8*		GetPixelAsByteArray( int nX, int nY, int nZ = 0 );
	
	u8*		GetPixelAsByteArray( u32 id );

    ��ø�������
    float       GetPixelAsFloat(int nX, int nY, int nZ = 0);

	 ���ͼ��������RGBA��ʽ,pValue������������ARGB
	void		SetPixel( u32 value, int nX, int nY, int nZ = 0 );
	 ���ͼ��������RGBA��ʽ,pValue������˳����A,R,G,B
	void		SetPixel( u8* pValue, int nX, int nY, int nZ = 0 );
	 �����4ͨ������ɫ,�������������ĳ�����ص����ɫֵ���
	void		SetPixel( const SColor& col, int nX, int nY, int nZ = 0 );	
	 ������������
	size_t		CopyPixelDataFromImage( int nOffsetX, int nOffsetY, int nWidth, int nHeight, void** ppData );

    �������ص�����
    bool        CopyPixelDataToImage(int nOffsetX, int nOffsetY, int nWidth, int nHeight, void* pData);

	 ����
	void		ResetForDynamicImageOpt( void );

	 ����ģ��
	void		BlurAvg( u32 iter );
	 ��˹ģ��
	void		BlurGaussian( u32 iter );
	 �������������Լ�ֵ
	void		SetScaleParm( ScaleParmName parm, ScaleParmValue value );
	 ����
	void		ScaleSize( int nWidth, int nHeight, int nDepth = 1);
	 ��Ե���
	void		EdgeDetectE( void );
	void		EdgeDetectP( void );
	void		EdgeDetectS( void );
	 ٤��У��
	void		GammaCorrect( float gama );
	 ��תͼ��
	void		FlipImage( void );
	 ����ͼ��
	void		Mirror( void );
	 ����ȡ��
	void		Negative( void );
	 ��������
	void		Noisify( float tolerance );
	 ɫ�黯
	void		Pixelize( u32 dwPixelSize );
	 ͼ����
	void		Sharpen( float Factor, u32 Iter);
	 ������ɫ
	void		SwapColours( void ); 
	 ��ɫ����
	void		ScaleColours( float r, float g, float b );
	 ��ת
	void		Rotate(float Angle);
	

    void        SetData(float* pData, u32 size);
	 ����ͼ������
	void		SetData( u8* pData, u32 size );
	 ����ͼ����
	void		SetImageWidth( int nWidth );
	 ����ͼ��߶�
	void		SetImageHeight( int nHeight );
	 ����ͼ�����
	void		SetImageDepth( int nDepth );
	 ����ͼ��ÿ���ص��ֽ���
	void		SetBytePerPixel( u8 byBPP );
	 �����������ص����ݴ�С
	void		SetSizeOfData( u32 size );
	 �������ظ�ʽ
	void		SetPixelFormat( PixelFormatDevil pf );
	 �����������ݵ��ڲ������ʽ
	void		SetCodecType( CodecType codectype );
	 ���������������
	void		SetOriginType( OriginType origintype );
	 �����������ͼ,����������ͼ���������
	void		SetCubeFlag( CubeFlag cudeflag );
	 ����mipmap������
	void		SetNumMips( int nMips );
	 ����ͼ�������
	void		SetNumLayers( int nLayers );
	void		SetPaletteData( u8* pData, u32 size );
	void		SetPaletteType( PaletteType pt );
	void		SetPaletteDataSize( u32 size );
	void		SetNumNext( int numNext );


	 �����������ָ��
	void*		GetData( void );
	 ���ͼ����
	int			GetImageWidth( void );
	 ���ͼ��߶�
	int			GetImageHeight( void );
	 ���ͼ�����
	int			GetImageDepth( void );
	 ���ͼ��ÿ���ص��ֽ���
	u8		GetBytePerPixel( void );
	 ����������ص����ݴ�С
	u32		GetSizeOfData( void );
	 ������ظ�ʽ
	PixelFormatDevil	GetPixelFormat( void );
	 ����������ݵ��ڲ������ʽ
	CodecType	GetCodecType( void );
	 ��������������
	OriginType	GetOriginType( void );
	 �����������ͼ,��ȡ������ͼ���������
	CubeFlag	GetCubeFlag( void );
	 ���mipmap������
	int			GetNumMips( void );
	 ���ͼ�������
	int			GetNumLayers( void );

	void*		GetPaletteData( void );

	PaletteType GetPaletteType( void );

	u32		GetPaletteDataSize( void );

	int			GetNumNexe( void );

    �����ɫ
    CColorValue GetColourAt(int x, int y);

    ��ʽת��
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
	void*			m_pData;				// ����ָ��
	int				m_nWidth;				// ͼ����
	int				m_nHeight;				// ͼ��߶�
	int				m_nDepth;				// ͼ�����
	u8			m_byBPP;				// ÿ���ص��ֽ���
	u32			m_dwSizeOfData;			// ���������ܴ�С
	PixelFormatDevil		m_PixelFormat;			// ���ظ�ʽ
	CodecType		m_CodecType;			// ͼ���ļ��ı����ʽ
	OriginType		m_OriginType;			// ͼ��������������
	u8*			m_pPelette;				// ��ɫ������
	PaletteType		m_PaletteType;			// ��ɫ������
	u32			m_PalSize;				// ��ɫ�����ݴ�С
	CubeFlag		m_CubeFlag;				// �����������ͼ,��¼���ĸ���
	int				m_nNumNext;				// number of images following
	int				m_nNumMips;				// mipmaps����
	int				m_nNumLayers;			// ͼ��Ĳ������
	unsigned int	m_dwImgName;			// ILͼ������
	ScaleParmValue  m_ScaleValueFilter;		// ���Ź���ֵ
	ScaleParmValue	m_ScaleValuePlacement;	// ���ŷ�������ֵ
    AIRString     m_szFile;               // ���ص��ļ���
    AIRString     m_strName;              // ����
};//! end class ImageOperator



class EngineExport ImageOperatorMgr
{
private:
	ImageOperatorMgr();

public:
	~ImageOperatorMgr();

	 ����ͼ��������������ļ�
	ImageOperator*				CreateImageOperatorFromFile( const char* szName, const char* szFile, bool bFullPath = false );

	 ����һ����̬��ͼ�������
	ImageOperator*				CreateDynamicImageOperator( const char* szName );

	 �ͷ�ͼ�������
	bool						ReleaseImageOperator( const char* szName );

    �ͷ�
    bool                        ReleaseImageOperator( ImageOperator* pImageOperator);

	 ��ȡͼ�������
	ImageOperator*				GetImageOperator( const char* szName );

	 ���ͼ���ʽ����
	ImgExtType					GetImgExtType( const AIRString& szExt );

	 ��ö���ʵ��
	static ImageOperatorMgr*	GetSingletonPtr( void );

    �жϺ�׺��
    static bool					JudgeImageExt(const char* szExt);

    ��õ�ǰͼ�������
    static void* GetCurrentImageData();

protected:
     �ͷ�
    void						Release( void );

	 ��ʼ��
	void						Init( void );

	

protected:
    typedef std::map<AIRString, ImageOperator*>	MAP_IMAGEOPERATOR;
	typedef MAP_IMAGEOPERATOR::iterator				MAP_IMAGEOPERATOR_ITOR;

	MAP_IMAGEOPERATOR					m_mapImageOperator; // ImageOperator��������
	std::map<AIRString, ImgExtType>	m_mapImgExtType;	// ͼ���ʽ��������

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
