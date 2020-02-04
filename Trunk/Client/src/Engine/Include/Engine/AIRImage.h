#pragma once
#include "EngineDefination.h"

#include "freeimage/FreeImagePlus.h"

#include "MemoryAlloter.h"
#include "Color.h"

//#ifdef _DEBUG
//#pragma comment(lib, "freeimage_d.lib")
//#else
//#pragma comment(lib, "freeimage.lib")
//#endif

class CDataStream;

class EngineExport AIRImage// : public EngineAllocator(AIRImage)
{
	EngineAllocator(AIRImage)
public:
	AIRImage();
	AIRImage(const char* szFilename);
	~AIRImage();

	bool Create(FREE_IMAGE_TYPE type, u32 nWidth, u32 nHeight);

	bool LoadFromFile(const char* szFilename);

	bool LoadFromFileStream(CDataStream* pStream);

	bool SaveImageToFile( const char* szFile );

	// 如果图像像素是RGBA格式,返回的是RGBA的u32数值
	u32		GetPixelAsInt( u32 nX, u32 nY );
	// 如果图像像素是RGBA格式,返回的是RGBA的u8数组
	u8*		GetPixelAsByteArray( u32 nX, u32 nY );

	SColor GetPixelColor(u32 nX, u32 nY);

	CColorValue GetPixelColorValue(u32 nX, u32 nY);

	// 如果图像像素是RGBA格式,pValue的整数类型是ARGB
	void		SetPixel( const CColorValue& value, u32 nX, u32 nY );
	// 如果图像像素是RGBA格式,pValue的输入顺序是A,R,G,B
	void		SetPixel( u8* pValue, u32 nX, u32 nY);
	// 如果是4通道的颜色,用这个方法设置某个像素点的颜色值最快
	void		SetPixel( const SColor& col, u32 nX, u32 nY );

	u32 GetWidth() const
	{
		return m_image.getWidth();
	}

	u32 GetHeight() const
	{
		return m_image.getHeight();
	}

	u8* GetPixelBits()
	{
		return m_image.accessPixels();
	}

	u32 GetBytePerPixel() const
	{
		return m_image.getBitsPerPixel();
	}

    RGBQUAD* GetPalette()
    {
        return m_image.getPalette();
    }

    void Convert232BitBitmap();

	//PixelFormat GetPixelFormat() const;

    bool FlipVerticle();
	
	fipImage m_image;

    FREE_IMAGE_TYPE GetFreeImageType();

    //把像素的位顺序反转
    void FlipPixels();

    void CopyPixelFromImage(int nOffsetX, int nOffsetY, int nWidth, int nHeight, void** pData);

    bool ScaleSize(u32 nWidth, u32 nHeight);

    //为了opengl服务
    void ExchangeColorR2B();
};



struct AGE_BITMAP
{
	void *data;
};

enum AGE_IMAGE_TYPE
{
	AIT_UNKNOWN = 0,	// unknown type
	AIT_BITMAP  = 1,	// standard image			: 1-, 4-, 8-, 16-, 24-, 32-bit
	AIT_UINT16	= 2,	// array of unsigned short	: unsigned 16-bit
	AIT_INT16	= 3,	// array of short			: signed 16-bit
	AIT_UINT32	= 4,	// array of unsigned long	: unsigned 32-bit
	AIT_INT32	= 5,	// array of long			: signed 32-bit
	AIT_FLOAT	= 6,	// array of float			: 32-bit IEEE floating point
	AIT_DOUBLE	= 7,	// array of double			: 64-bit IEEE floating point
	AIT_COMPLEX	= 8,	// array of FICOMPLEX		: 2 x 64-bit IEEE floating point
	AIT_RGB16	= 9,	// 48-bit RGB image			: 3 x 16-bit
	AIT_RGBA16	= 10,	// 64-bit RGBA image		: 4 x 16-bit
	AIT_RGBF	= 11,	// 96-bit RGB float image	: 3 x 32-bit IEEE floating point
	AIT_RGBAF	= 12	// 128-bit RGBA float image	: 4 x 32-bit IEEE floating point
};

class EngineExport Image2D// : EngineAllocator(Image2D)
{
	EngineAllocator(Image2D)
public:
	Image2D(AGE_IMAGE_TYPE imageType = AIT_BITMAP, size_t width = 0, size_t height =0, size_t bpp = 0 );
	~Image2D();

	Image2D( const char *path );
	Image2D(const Image2D& src);
	Image2D& operator=(const Image2D& src);
	Image2D& operator=(AGE_BITMAP *dib);

	void Destory();

	//load & save, 
	//Note: flag is set to 0 by default, see FreeImage_Load, FreeImage documentation
	bool Load(const char* path, int flag = 0);
	bool Save(const char* path, int flag = 0);

	bool IsValid() const;

	u32 GetImageSize() const;
	u32 GetBitsPerPixel() const; //get bpp
	u32 GetWidth() const;
	u32 GetHeight() const;
	u32 GetPitch() const;

	//Get Pixel Buffer only
	u8* GetBuffer() const;
	AGE_IMAGE_TYPE GetImageType() const;

protected:
	bool InitSize(AGE_IMAGE_TYPE image_type, unsigned width, unsigned height, unsigned bpp, unsigned red_mask = 0, unsigned green_mask = 0, unsigned blue_mask = 0);
	bool Replace(AGE_BITMAP *new_dib);

protected:

	AGE_BITMAP      *m_pBuffer;
};
