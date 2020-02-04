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

    AIRString                 strName;            //�Լ�������
    AIRVector<AIRString>    vtResourceFiles;    //��Ӧ����Դ�ļ���
    bool                        bLoad;              //�Ƿ��Ѿ�����
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
    // ����:  Load
    // ����:  ����XML�ڵ�Ԫ��
    // ����:  ThisElement - XMLԪ�ؽڵ�
    // ����:  ���سɹ�?
    //---------------------------------------------------------
	//virtual bool            Load( CMarkupSTL& xml );

    virtual bool            Load(const SCHEME_DATA& scheme_data);

    //---------------------------------------------------------
    // ����:  LoadSelf
    // ����:  ��ȡ�ؼ������������
    // ����:  ThisElement - XMLԪ�ؽڵ�
    // ����:  ���سɹ�?
    //---------------------------------------------------------
    //virtual bool            LoadSelf( CMarkupSTL& xml );

    //---------------------------------------------------------
    // ����:  LoadChild
    // ����:  ��ȡ�����ɣ������ӿؼ�����
    // ����:  ThisElement - XMLԪ�ؽڵ�
    // ����:  ���سɹ�?
    //---------------------------------------------------------
    virtual bool            LoadChild( CMarkupSTL& parent );

    //---------------------------------------------------------
    // ����:  Create
    // ����:  ����XML�ڵ�������GUIԪ��
    // ����:  ThisElement - XMLԪ�ؽڵ�
    // ����:  �����ɹ�
    //---------------------------------------------------------
    virtual bool            Create( CMarkupSTL& xml );

    const char*             FindResource(const char* szResource);

    bool                    AddResource(const char* szResource);

    void                    RemoveResource(const char* szResource);

    bool                    LoadResource(const char* szResource);

    const AIRString& GetName() const { return m_data.strName; }

    //�������Ӧ��ImageSet
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

    //��ö�Ӧ��ImageSet
    CImageSet* GetImageSet(const AIRString& strImageSetName);
    CImageUnit* GetImageUnit(const AIRString& strImageSetName, const AIRString& strImageUnitName);

    CImageSet* GetImageSetByName(const char* strImageSetName);

    virtual void Clear();

    //����Ĭ�ϵ�UI
    virtual CGUIElement*    CreateDefaultElement(const AIRString& strType, CGUIElement* pParent = NULL);

    //������������
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

    //�������ֲ��ҿؼ�
    CGUIElement* FindCtrl(const char* szCtrlName);

    //�������пؼ���alignλ��
    void CaculateEntityAlignPosition();

    UILayer* GetUILayer(UILayerDefine uiLayer)
    {
        return &m_uiLayers[uiLayer];
    }

    //���ĳ�ؼ�
    void ClearEntity(CGUIElement* pEntity);
private:
    //list<string>  m_lstResource;
    //typedef list<string>::iterator  LIST_STRING_ITER;
    //string       m_strName;
    //AIRString  m_strFileName;
    SCHEME_DATA             m_data;
    //bool                    m_bIsLoad;
    bool                    m_bEditMode;  //�Ƿ�Ϊ�༭״̬
    //CImageSetManager        m_imgSetMgr;
	AIRString				m_szTextureListFile;

    IMAGESET_MAP		m_mapImageSet;
    //FileData*  m_pSchemeResource;   //scheme���ļ���Դ

    bool  m_bImageSetReady;

    UILayer  m_uiLayers[UI_Layer_Max];

    f32 m_fMinZ;
    f32 m_fMaxZ;
};
