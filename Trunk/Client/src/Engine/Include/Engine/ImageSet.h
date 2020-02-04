#pragma once

//#include "AIRGUI.h"
#include "Rect.h"
#include "MarkupSTL.h"
#include "Common.h"
#include "ITexture.h"
#include "IteratorWrappers.h"
#include "EngineResource.h"


class CImageSet;

class EngineExport CImageUnit// : public EngineAllocator(CImageUnit)
{
	EngineAllocator(CImageUnit)
public:
    CImageUnit();
    ~CImageUnit();
    bool LoadElementAttribute(CMarkupSTL& xml);
public:
    AIRString  m_strName;  // 图像单元名称
    Rectf        m_fUV;      // 图像单元的UV坐标信息
    int          m_nWidth;   // 图像单元的宽度
    int          m_nHeight;  // 图像单元的高度
	int			 m_nOffsetX;
	int			 m_nOffsetY;
    //int          m_nID;
	AIRString	 m_szImgSetName;
    CImageSet*   m_pSet;
};

class EngineExport CImageSet : public Resource
{
public:
	typedef AIRMap<AIRString, CImageUnit*> MAP_IMAGE_UNIT;
	typedef AIRMap<AIRString, CImageUnit*>::iterator IMAGEUNIT_ITER;

public:
    CImageSet();
    virtual ~CImageSet();
    //bool                LoadElementAttribute(CMarkupSTL& xml);
    CImageUnit*         GetImgUnit( const AIRString &strImgUnitName );
    CImageUnit*         GetImgUnit( int posx, int posy );

    //CDXTexture* GetTexture() const { return m_pTexture; }
    const AIRString&  GetName() const { return m_strName; }
    //const AIRString&  GetFileName() const { return m_strFilename; }

    ITexture*           GetTexture() { return m_pTexture; }

    bool                Use(u32 nStage);

    void                Clear();

	int					GetWidth()	{ return m_nWidth; }
	int					GetHeight() { return m_nHeight; }

	MAP_IMAGE_UNIT*		GetAllImageUnit( void ) { return &m_mapImageUnit; }

    size_t  GetUnitsNum() const
    {
        return m_mapImageUnit.size();
    }

    MapIterator<MAP_IMAGE_UNIT> GetImageUnitIter()
    {
        return MapIterator<MAP_IMAGE_UNIT>(m_mapImageUnit);
    }

    virtual bool Cache();

    virtual s32 Release();
protected:
private:

    MAP_IMAGE_UNIT  m_mapImageUnit;
    ITexture*       m_pTexture;
    int             m_nWidth;
    int             m_nHeight;
    AIRString     m_strName;
    //AIRString     m_strFilename;
    //int             m_nID;
};