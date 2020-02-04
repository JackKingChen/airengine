#include "stdafx.h"//#include "stdafx.h"
#include "ImageSet.h"
//#include "GUISystem.h"
#include "FileSystem.h"
//#include "TextureManager.h"
#include "StringUtil.h"
#include "Engine.h"
#include "TextureMgr.h"



CImageUnit::CImageUnit() : m_pSet(0), m_nHeight(0), m_nWidth(0)
, m_nOffsetX(0), m_nOffsetY(0)
{

}

CImageUnit::~CImageUnit()
{
    m_pSet = NULL;
    //g_imageUnit--;
    //OutputDebugString(GetString("now the image unit count is %d id[%d]\n", g_imageUnit, m_nID));
}

bool CImageUnit::LoadElementAttribute(CMarkupSTL& xml)
{
    AIRString	 value;
    //ImageUnit    *pp = (ImageUnit*) parentctrl;

    m_strName = xml.GetAttrib("Name").c_str();

    // 图像单元名称
    //if( pElement->Attribute( "Name" ) )
    //{
    //    this->m_strName = pElement->Attribute( "Name" );
    //}

    // 图像单元UV的左边界值
    value = xml.GetAttrib("X0").c_str();
    m_fUV.left = (float)atoi( value.c_str() );
	m_nOffsetX = m_fUV.left;
    //value.clear();
    //if( pElement->Attribute( "X0") )
    //{
    //    value = pElement->Attribute( "X0"); 
    //    this->m_fUV.left = (float)atoi( value.c_str() );
    //}

    // 图像单元UV的上边界值
    value.clear();
    value = xml.GetAttrib("Y0").c_str();
    m_fUV.top = (float)atoi( value.c_str() );
	m_nOffsetY = m_fUV.top;
    //if( pElement->Attribute( "Y0") )
    //{
    //    value = pElement->Attribute( "Y0"); 
    //    this->m_fUV.top = (float)atoi( value.c_str() );
    //}

    // 图像单元UV的右边界值
    value.clear();
    value = xml.GetAttrib("X1").c_str();
    m_fUV.right = (float)atoi( value.c_str() );
    //if( pElement->Attribute( "X1") )
    //{
    //    value = pElement->Attribute( "X1"); 
    //    this->m_fUV.right = (float)atoi( value.c_str() );
    //}

    // 图像单元UV的下边界值
    value.clear();
    value = xml.GetAttrib("Y1").c_str();
    m_fUV.bottom = (float)atoi(value.c_str());
    //if( pElement->Attribute( "Y1") )
    //{
    //    value = pElement->Attribute( "Y1"); 
    //    this->m_fUV.bottom = (float)atoi( value.c_str() );
    //}
    this->m_nWidth  = m_fUV.right - m_fUV.left;
    this->m_nHeight = m_fUV.bottom - m_fUV.top;

    return true;
}

CImageSet::CImageSet()
{
	this->m_pTexture = NULL;
	this->m_strName.clear();
	this->m_strFilename.clear();
	this->m_nWidth = 0;
	this->m_nHeight = 0;
	this->m_mapImageUnit.clear();
}

CImageSet::~CImageSet()
{
    //TEXTUREMANAGER->ClearTexture(m_pTexture);
    //m_pTexture = NULL;
    //GUISYSTEM->ClearTexture(m_pTexture);
    Clear();
}

void CImageSet::Clear()
{
    if (m_pTexture)
    {
        //GUISYSTEM->ClearTexture(m_pTexture);
		TEXTURE_MANAGER->ClearTexture(m_pTexture);
        m_pTexture = NULL;
    }

    for (IMAGEUNIT_ITER it = m_mapImageUnit.begin(); it != m_mapImageUnit.end(); ++it)
    {
        CImageUnit* pUnit = it->second;
        SafeDelete(pUnit);
    }

    m_mapImageUnit.clear();
}

/*
bool CImageSet::LoadElementAttribute(CMarkupSTL& xml)
{
	// imageset的名称
    m_strName = xml.GetAttrib("Name");

	// 图像文件名
    m_strFilename = xml.GetAttrib( "Imagefile" );
	if ( m_strFilename == "" )
	{
		m_strFilename = xml.GetAttrib( "File" );
	}

	// 加载纹理
	//m_pTexture = GUISYSTEM->LoadTexture(m_strFilename.c_str());
	IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    TextureParam param;
    param.nMipmaps = 1;
	m_pTexture = TEXTURE_MANAGER->CreateTextureFromFile( m_strFilename.c_str(), param);
    if (m_pTexture == 0)
    {
        return false;
    }

	// 是否反转图像
    AIRString	 value = xml.GetAttrib("FlipVert");
    bool         bFlipVert = false;
    if (value.compare("TRUE"))
    {
        bFlipVert = true;
    }
    
	// 图像宽度
    value = xml.GetAttrib("Width");
    m_nWidth = atoi(value.c_str());

	// 图像高度
    value = xml.GetAttrib("Height");
    m_nHeight = atoi(value.c_str());

	// 加载图像单元
    CImageUnit    *ImgUnit = NULL;
    while (xml.FindChildElem())
    {
        xml.IntoElem();
        ImgUnit = new CImageUnit;
        if( ImgUnit->LoadElementAttribute( xml ) )
        {
            ImgUnit->m_pSet         = this;
            ImgUnit->m_fUV.left		= float(ImgUnit->m_fUV.left  )  / float(this->m_nWidth  );
            ImgUnit->m_fUV.top		= float(ImgUnit->m_fUV.top   )  / float(this->m_nHeight );
            ImgUnit->m_fUV.right	= float(ImgUnit->m_fUV.right )  / float(this->m_nWidth  );
            ImgUnit->m_fUV.bottom	= float(ImgUnit->m_fUV.bottom)  / float(this->m_nHeight );
			ImgUnit->m_szImgSetName	= m_strName;
            AIRString unitname	= ImgUnit->m_strName;
            CStringUtil::ToUpperCase( unitname );
            this->m_mapImageUnit.insert( std::make_pair( unitname, ImgUnit ) );
        }
        xml.OutOfElem();
    }

    return true;
}
*/

CImageUnit* CImageSet::GetImgUnit(const AIRString &strImgUnitName )
{
    //----------------------------------
    AIRString unitname = strImgUnitName;
    CStringUtil::ToUpperCase( unitname );
    IMAGEUNIT_ITER it = m_mapImageUnit.find(unitname);
    if (it != m_mapImageUnit.end())
    {
        return it->second;
    }
    return NULL;
    //----------------------------------

    //IMAGEUNIT_ITER it = m_mapImageUnit.find(strImgUnitName);
    //if (it != m_mapImageUnit.end())
    //{
    //    return it->second;
    //}

    //return NULL;
}

bool CImageSet::Use(u32 nStage)
{
    //return m_pTexture->Use(nStage);
    return true;
}


CImageUnit* CImageSet::GetImgUnit( int posx, int posy )
{
    IMAGEUNIT_ITER it = m_mapImageUnit.begin();
    for ( ; it != m_mapImageUnit.end(); ++it )
    {
        CImageUnit* pUnit = it->second;
        if ( posx < pUnit->m_nOffsetX || 
             posx > pUnit->m_nOffsetX + pUnit->m_nWidth ||
             posy < pUnit->m_nOffsetY ||
             posy > pUnit->m_nOffsetY + pUnit->m_nHeight )
        {
            continue;
        }
        else
        {
            return pUnit;
        }
    }
    return NULL;
}

bool CImageSet::Cache()
{
    if (m_pStream)
    {
        AIRString strDoc = m_pStream->GetAsString();
        // 获得imageset XML树
        CMarkupSTL xml(strDoc.c_str());
        // 获得图像文件名
        //AIRString strImgSetFile = xmlImgset.GetAttrib("Imagefile");
        //// 根据图像文件名查找是否已经加载了该图像文件
        //CImageSet* img = GetImageSet(strImgSetFile);

        // 根据imageset的名查找是否已经加载了
        // 尝试找根结点
        if ( !xml.FindElem() )
        {
            assert(false);
            SafeDelete(m_pStream);
            return false;
        }

        m_strName = xml.GetAttrib("Name").c_str();

        // 图像文件名
        AIRString strImageFile = xml.GetAttrib( "Imagefile" ).c_str();
        if ( strImageFile == "" )
        {
            strImageFile = xml.GetAttrib( "File" ).c_str();
        }

        // 加载纹理
        //m_pTexture = GUISYSTEM->LoadTexture(m_strFilename.c_str());
        IRenderer* pRenderer = ENGINE_INST->GetRenderer();
        TextureParam param;
        param.nMipmaps = 1;
        m_pTexture = TEXTURE_MANAGER->CreateTextureFromFile( strImageFile.c_str(), param);
        if (m_pTexture == 0)
        {
            return false;
        }

        // 是否反转图像
        AIRString	 value = xml.GetAttrib("FlipVert").c_str();
        bool         bFlipVert = false;
        if (value.compare("TRUE"))
        {
            bFlipVert = true;
        }

        // 图像宽度
        value = xml.GetAttrib("Width").c_str();
        m_nWidth = atoi(value.c_str());

        // 图像高度
        value = xml.GetAttrib("Height").c_str();
        m_nHeight = atoi(value.c_str());

        // 加载图像单元
        CImageUnit    *ImgUnit = NULL;
        while (xml.FindChildElem())
        {
            xml.IntoElem();
            ImgUnit = new CImageUnit;
            if( ImgUnit->LoadElementAttribute( xml ) )
            {
                ImgUnit->m_pSet         = this;
                ImgUnit->m_fUV.left		= float(ImgUnit->m_fUV.left  )  / float(this->m_nWidth  );
                ImgUnit->m_fUV.top		= float(ImgUnit->m_fUV.top   )  / float(this->m_nHeight );
                ImgUnit->m_fUV.right	= float(ImgUnit->m_fUV.right )  / float(this->m_nWidth  );
                ImgUnit->m_fUV.bottom	= float(ImgUnit->m_fUV.bottom)  / float(this->m_nHeight );
                ImgUnit->m_szImgSetName	= m_strName;
                AIRString unitname	= ImgUnit->m_strName;
                CStringUtil::ToUpperCase( unitname );
                this->m_mapImageUnit.insert( std::make_pair( unitname, ImgUnit ) );
            }
            xml.OutOfElem();
        }
        SafeDelete(m_pStream);
    }

    //开始解释纹理
    if (!m_bReady)
    {
        if (m_pTexture)
        {
            if (!m_pTexture->IsReady())
            {
                if (m_pTexture->IsLoaded())
                {
                    m_pTexture->Cache();
                }
            }

            m_bReady = m_pTexture->IsReady();
            if (m_bReady)
            {
                NotifyAllListenerReady();
            }
        }
    }
    

    return m_bReady;
}

s32 CImageSet::Release()
{
    if (DropRef() == 0)
    {
        Clear();
    }

    return GetRef();
}
