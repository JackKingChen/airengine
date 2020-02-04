#pragma once
#include "GUIEntityList.h"
#include "MarkupSTL.h"
//#include "ImageSetManager.h"
#include "GUIEditCtrl.h"
#include "IteratorWrappers.h"
#include "UILayer.h"
//#include "XMLParser.h"
class CImageSet;
class CImageUnit;

typedef struct tagSchemeData 
{

    AIRString                 strName;            //自己的名字
    AIRVector<AIRString>    vtResourceFiles;    //对应的资源文件名
    bool                        bLoad;              //是否已经加载
}SCHEME_DATA;


class AIRGuiExport CGUISchemeManager : /*public CGUIEntityList, */public Resource
{
public:
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    typedef AIRHashMap<AIRString, CImageSet*>	IMAGESET_MAP;
    typedef IMAGESET_MAP::iterator				IMAGESET_ITER;
    typedef AIRHashMap<AIRString, AIRString>	IMAGESETFILE_MAP;
    typedef IMAGESETFILE_MAP::iterator			IMAGESETFILE_ITOR;
#else
	typedef AIRMap<AIRString, CImageSet*>		IMAGESET_MAP;
	typedef IMAGESET_MAP::iterator				IMAGESET_ITER;
	typedef AIRMap<AIRString, AIRString>		IMAGESETFILE_MAP;
	typedef IMAGESETFILE_MAP::iterator			IMAGESETFILE_ITOR;
#endif

public:
    CGUISchemeManager();
    virtual ~CGUISchemeManager(void);


    //---------------------------------------------------------
    // 名称:  Load
    // 功能:  加载XML节点元素
    // 参数:  ThisElement - XML元素节点
    // 返回:  加载成功?
    //---------------------------------------------------------
	//virtual bool            Load( CMarkupSTL& xml );

    virtual bool            Load(const SCHEME_DATA& scheme_data);

    //---------------------------------------------------------
    // 名称:  LoadSelf
    // 功能:  读取控件自身参数配置
    // 参数:  ThisElement - XML元素节点
    // 返回:  加载成功?
    //---------------------------------------------------------
    //virtual bool            LoadSelf( CMarkupSTL& xml );

    //---------------------------------------------------------
    // 名称:  LoadChild
    // 功能:  读取，生成，配置子控件参数
    // 参数:  ThisElement - XML元素节点
    // 返回:  加载成功?
    //---------------------------------------------------------
    virtual bool            LoadChild( CMarkupSTL& parent );

    //---------------------------------------------------------
    // 名称:  Create
    // 功能:  根据XML节点来创建GUI元素
    // 参数:  ThisElement - XML元素节点
    // 返回:  创建成功
    //---------------------------------------------------------
    virtual bool            Create( CMarkupSTL& xml );

    const char*             FindResource(const char* szResource);

    bool                    AddResource(const char* szResource);

    void                    RemoveResource(const char* szResource);

    bool                    LoadResource(const char* szResource);

    const AIRString& GetName() const { return m_data.strName; }

    //加载其对应的ImageSet
    CImageSet* LoadImageSet(const char* szFilename);
	
    bool OnLButtonDown(int x, int y);
    bool OnLButtonUp(int x, int y);
    bool OnRButtonDown(int x, int y);
    bool OnRButtonUp(int x, int y);
    bool OnMouseMove(int x, int y);
    bool OnMouseWheel(int dz);
    bool OnKeyPress(int nKey);
    bool OnKeyUp(int nKey);
    bool OnLButtonDBClick(int x, int y);

	bool StartDragElement(CGUIElement* e);
    bool EndDragElement(CGUIElement* e, int x, int y);
	void DragElement(int dx, int dy);
	virtual void Update();

    virtual bool ProcessInput(const LPINPUTEVENT pEvent);

    //获得对应的ImageSet
    CImageSet* GetImageSet(const AIRString& strImageSetName);
    CImageUnit* GetImageUnit(const AIRString& strImageSetName, const AIRString& strImageUnitName);

    CImageSet* GetImageSetByName(const char* strImageSetName);

    virtual void Clear();

    //创建默认的UI
    virtual CGUIElement*    CreateDefaultElement(const AIRString& strType, CGUIElement* pParent = NULL);

    //加载完后的设置
    virtual void SetAfterLoad();

    void SetEditMode(bool bEdit);

	bool GetEditMode( void )
	{
		return m_bEditMode;
	}

    virtual bool IsEditMode()
    {
        return m_bEditMode;
    }

	bool SaveToXML();

	bool SaveToXML(const char* szFileName);

	//CImageSetManager*		GetImageSetMgr( void );

    /*
    void NotifyTextureDestroy(ITexture* pTex)
    {
        for (GUIELEMENT_ITER it = m_lstElement.begin(); it != m_lstElement.end(); it++)
        {
            (*it)->NotifyTextureDestroy( pTex );
        }

        for (GUIELEMENT_ITER it = m_lstElementBottom.begin(); it != m_lstElementBottom.end(); it++)
        {
            (*it)->NotifyTextureDestroy( pTex );
        }

        for (GUIELEMENT_ITER it = m_lstElementTop.begin(); it != m_lstElementTop.end(); it++)
        {
            (*it)->NotifyTextureDestroy( pTex );
        }
    }
    */

    void MoveElementToTop(CGUIElement* pElement);

    virtual bool Cache();

    //void CacheUIElements();

    //virtual void CleanUp();
    MapIterator<IMAGESET_MAP> GetImageSetIter()
    {
        return MapIterator<IMAGESET_MAP>(m_mapImageSet);
    }

    void CleanUp();

    void AddEntity(CGUIElement* pElement);

    void RemoveEntity(CGUIElement* pEntity);

    void Render();

    //根据名字查找控件
    CGUIElement* FindCtrl(const char* szCtrlName);

    //计算所有控件的align位置
    void CaculateEntityAlignPosition();

    UILayer* GetUILayer(UILayerDefine uiLayer)
    {
        return &m_uiLayers[uiLayer];
    }

    //清空某控件
    void ClearEntity(CGUIElement* pEntity);
private:
    //list<string>  m_lstResource;
    //typedef list<string>::iterator  LIST_STRING_ITER;
    //string       m_strName;
    //AIRString  m_strFileName;
    SCHEME_DATA             m_data;
    //bool                    m_bIsLoad;
    bool                    m_bEditMode;  //是否为编辑状态
    //CImageSetManager        m_imgSetMgr;
	AIRString				m_szTextureListFile;

    IMAGESET_MAP		m_mapImageSet;
    //FileData*  m_pSchemeResource;   //scheme的文件资源

    bool  m_bImageSetReady;

    UILayer  m_uiLayers[UI_Layer_Max];

    f32 m_fMinZ;
    f32 m_fMaxZ;
};
