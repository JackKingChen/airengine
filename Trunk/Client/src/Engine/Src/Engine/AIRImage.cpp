#include "stdafx.h"
#include "AIRImage.h"
#include "FileSystem.h"
#include "MemoryDataStream.h"
#include "DebugTools.h"

unsigned char Reserse8(unsigned char c)
{
     c = ( c & 0x55 ) << 1 | ( c & 0xAA ) >> 1;
     c = ( c & 0x33 ) << 2 | ( c & 0xCC ) >> 2;
     c = ( c & 0x0F ) << 4 | ( c & 0xF0 ) >> 4;
     return c;
}

AIRImage::AIRImage()
{

}

AIRImage::AIRImage(const char* szFilename)
{
	LoadFromFile(szFilename);
}

AIRImage::~AIRImage()
{

}

bool AIRImage::Create(FREE_IMAGE_TYPE type, u32 nWidth, u32 nHeight)
{
	m_image.clear();
    m_image.setSize(type, nWidth, nHeight, 32);
	return m_image.isValid();
}

bool AIRImage::LoadFromFile(const char* szFilename)
{
	m_image.clear();
	
	//if (m_pFB)
	//{
	//	FreeImage_Unload(m_pFB);
	//	m_pFB = 0;
	//}

	CDataStream* pStream = FILESYSTEM->GetFileStream(szFilename);
	if (pStream == NULL)
	{
		return false;
	}

	CMemoryDataStream dataStream(*pStream);

	SafeDelete(pStream);

	fipMemoryIO memoryIO(dataStream.GetPtr(), dataStream.Size());

	if (memoryIO.isValid())
	{
		return m_image.loadFromMemory(memoryIO);
	}

	return false;
}

bool AIRImage::LoadFromFileStream(CDataStream* pStream)
{
	CMemoryDataStream dataStream(*pStream);

	fipMemoryIO memoryIO(dataStream.GetPtr(), dataStream.Size());

	if (memoryIO.isValid())
	{
		bool ret = m_image.loadFromMemory(memoryIO);
		return ret;
	}

	return false;
}

bool AIRImage::SaveImageToFile(const char* szFile)
{
	if (m_image.isValid())
	{
		return m_image.save(szFile);
	}

	return false;
}

u32 AIRImage::GetPixelAsInt(u32 nX, u32 nY)
{
	RGBQUAD color;
    m_image.getPixelColor(nX, nY, &color);
	SColor outColor(color.rgbRed, color.rgbGreen, color.rgbGreen, color.rgbReserved);
	return outColor.color;
}

u8* AIRImage::GetPixelAsByteArray(u32 nX, u32 nY)
{
	u8* pPixels = 0;
	m_image.getPixelIndex(nX, nY, pPixels);
	return pPixels;
}


void AIRImage::SetPixel(const SColor& col, u32 nX, u32 nY)
{
	RGBQUAD color;
	color.rgbBlue = col.GetBlue();
	color.rgbGreen = col.GetGreen();
	color.rgbRed = col.GetRed();
	color.rgbReserved = col.GetAlpha();
	m_image.setPixelColor(nX, nY, &color);
}

void AIRImage::SetPixel(const CColorValue& value, u32 nX, u32 nY )
{
	SetPixel(SColor(value.GetAsARGB()), nX, nY);
}

void AIRImage::SetPixel(u8* pValue, u32 nX, u32 nY)
{
    RGBQUAD* pColors = (RGBQUAD*)pValue;
    for (u32 i = nY; i < m_image.getHeight(); ++i)
    {
        for (u32 j = nX; j < m_image.getWidth(); ++j)
        {
            if (!m_image.setPixelColor(j, i, pColors))
            {
                assert(0);
            }
            else
            {
                pColors++;
            }
        }
    }
	
}

SColor AIRImage::GetPixelColor(u32 nX, u32 nY)
{
	RGBQUAD color;
	m_image.getPixelColor(nX, nY, &color);
	return SColor(color.rgbReserved, color.rgbRed, color.rgbGreen, color.rgbBlue);
}

CColorValue AIRImage::GetPixelColorValue(u32 nX, u32 nY)
{
	RGBQUAD color;
	m_image.getPixelColor(nX, nY, &color);
	return CColorValue(SColor(color.rgbReserved, color.rgbRed, color.rgbGreen, color.rgbBlue));
}

bool AIRImage::FlipVerticle()
{
    return m_image.flipVertical();
}

void AIRImage::Convert232BitBitmap()
{
    m_image.convertTo32Bits();
}

FREE_IMAGE_TYPE AIRImage::GetFreeImageType()
{
    return m_image.getImageType();
}

void AIRImage::FlipPixels()
{
    RGBQUAD color;
    RGBQUAD colorR;
    for (u32 row = 0; row < m_image.getHeight(); ++row)
    {
        for (u32 col = 0; col < m_image.getWidth(); ++col)
        {
            m_image.getPixelColor(col, row, &color);
            colorR.rgbReserved = Reserse8(color.rgbRed);
            colorR.rgbGreen = Reserse8(color.rgbBlue);
            colorR.rgbBlue = Reserse8(color.rgbGreen);
            colorR.rgbRed = Reserse8(color.rgbReserved);
            m_image.setPixelColor(col, row, &colorR);
        }
    }
}

void AIRImage::CopyPixelFromImage(int nOffsetX, int nOffsetY, int nWidth, int nHeight, void** pData)
{
    fipImage dst;
    m_image.copySubImage(dst, nOffsetX, nOffsetY, nOffsetX + nWidth, nOffsetY + nHeight);

    RGBQUAD color;
    dst.getPixelColor(0, 0, &color);
    memcpy(*pData, &color, dst.getImageSize());
}

bool AIRImage::ScaleSize(u32 nWidth, u32 nHeight)
{
    return m_image.setSize(GetFreeImageType(), nWidth, nHeight, GetBytePerPixel());
}

void AIRImage::ExchangeColorR2B()
{
    RGBQUAD color;
    u8 tmpR;
    for (u32 row = 0; row < m_image.getHeight(); ++row)
    {
        for (u32 col = 0; col < m_image.getWidth(); ++col)
        {
            m_image.getPixelColor(col, row, &color);
            tmpR = color.rgbRed;
            color.rgbRed = color.rgbBlue;
            color.rgbBlue = tmpR;
            m_image.setPixelColor(col, row, &color);
        }
    }
}


//------------------------------------------------------------------

// new image use FreeImage lib instead of Devil
// Note: 
// call this ONLY when linking with FreeImage as a static library
//      #ifdef FREEIMAGE_LIB
//            FreeImage_Initialise();
//            FreeImage_DeInitialise();
//      #endif
//



Image2D::Image2D( AGE_IMAGE_TYPE imageType /*= AIT_BITMAP*/, size_t width /*= 0*/, size_t height /*=0*/, size_t bpp /*= 0 */ )
	:m_pBuffer(NULL)
{
	if(width && height && (bpp || imageType != AIT_BITMAP))
	{
		InitSize(imageType, width, height, bpp);
	}
}

Image2D::~Image2D()
{
	Destory();		
}

bool Image2D::InitSize( AGE_IMAGE_TYPE image_type, unsigned width, unsigned height, unsigned bpp, unsigned red_mask /*= 0*/, unsigned green_mask /*= 0*/, unsigned blue_mask /*= 0*/ )
{		
	Destory();

	m_pBuffer = (AGE_BITMAP*)FreeImage_AllocateT((FREE_IMAGE_TYPE)image_type, width, height, bpp, red_mask, green_mask, blue_mask);
	if (m_pBuffer == NULL)
	{
		return false;
	}

	if(image_type == AIT_BITMAP) 
	{
		// Create palette if needed
		switch(bpp)	{
		case 1:
		case 4:
		case 8:
			FIBITMAP* pDib = (FIBITMAP*)m_pBuffer;
			RGBQUAD *pal = FreeImage_GetPalette(pDib);
			for(unsigned i = 0; i < FreeImage_GetColorsUsed(pDib); i++) 
			{
				pal[i].rgbRed = (BYTE)i;
				pal[i].rgbGreen = (BYTE)i;
				pal[i].rgbBlue = (BYTE)i;
			}
			break;
		}
	}

	return true;
}

void Image2D::Destory()
{
	if(m_pBuffer) 
	{
		FreeImage_Unload((FIBITMAP*)m_pBuffer);
		m_pBuffer = NULL;
	}
}

Image2D::Image2D( const char *path )
	:m_pBuffer(NULL)
{
	Load(path);
}

Image2D::Image2D( const Image2D& src )
	:m_pBuffer(NULL)
{
	FIBITMAP *clone = FreeImage_Clone((FIBITMAP*)src.m_pBuffer);
	Replace((AGE_BITMAP*)clone);
}

Image2D& Image2D::operator=( const Image2D& src )
{
	if(this != &src) {
		FIBITMAP *clone = FreeImage_Clone((FIBITMAP*)src.m_pBuffer);
		Replace((AGE_BITMAP*)clone);
	}
	return *this;
}

Image2D& Image2D::operator=( AGE_BITMAP *dib )
{
	if(m_pBuffer != dib) {
		Replace(dib);
	}
	return *this;
}

bool Image2D::Load( const char* path, int flag /*=0*/)
{
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	fif = FreeImage_GetFileType(path);
	if(fif == FIF_UNKNOWN)
	{
		fif = FreeImage_GetFIFFromFilename(path);
	}
	if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) 
	{
		Destory();
		// Load the file			
		m_pBuffer = (AGE_BITMAP *)FreeImage_Load(fif, path, flag);
		if(m_pBuffer == NULL)
			return false;
		return true;
	}
	return false;
}

bool Image2D::Save( const char* path, int  flag /*=0*/)
{
	FREE_IMAGE_FORMAT format = FIF_UNKNOWN;
	BOOL bResult = false;

	format = FreeImage_GetFIFFromFilename(path);
	if(format != FIF_UNKNOWN ) 
	{
		BOOL bCanSave;
		FIBITMAP* fiBuffer = (FIBITMAP*)m_pBuffer;
		FREE_IMAGE_TYPE image_type = FreeImage_GetImageType(fiBuffer);
		if(image_type == FIT_BITMAP) 
		{
			u32 bpp = FreeImage_GetBPP(fiBuffer);
			bCanSave = (FreeImage_FIFSupportsWriting(format) && FreeImage_FIFSupportsExportBPP(format, bpp));
		} 
		else 
		{
			bCanSave = FreeImage_FIFSupportsExportType(format, image_type);
		}

		if(bCanSave) 
		{
			bResult = FreeImage_Save(format, fiBuffer, path, flag);
			return (bResult?true: false);
		}
	}
	return (bResult?true: false);
}

bool Image2D::IsValid() const
{
	return (m_pBuffer == NULL ? false : true);
}

AGE_IMAGE_TYPE Image2D::GetImageType() const
{
	return  (AGE_IMAGE_TYPE)FreeImage_GetImageType((FIBITMAP*)m_pBuffer);
}

u32 Image2D::GetImageSize() const
{
	return FreeImage_GetDIBSize((FIBITMAP*)m_pBuffer);
}

u32 Image2D::GetBitsPerPixel() const
{
	return FreeImage_GetBPP((FIBITMAP*)m_pBuffer);;
}

bool Image2D::Replace( AGE_BITMAP *new_dib )
{
	if(new_dib == NULL) 
		return false;
	Destory();
	m_pBuffer = new_dib;
	return false;
}

u32 Image2D::GetWidth() const
{
	return FreeImage_GetWidth((FIBITMAP*)m_pBuffer); 
}

u32 Image2D::GetHeight() const
{
	return FreeImage_GetHeight((FIBITMAP*)m_pBuffer); 
}

u32 Image2D::GetPitch() const
{
	return  FreeImage_GetPitch((FIBITMAP*)m_pBuffer); 
}

u8* Image2D::GetBuffer() const
{
	return  FreeImage_GetBits((FIBITMAP*)m_pBuffer); 
}
