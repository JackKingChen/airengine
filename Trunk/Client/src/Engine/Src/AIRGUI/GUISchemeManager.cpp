//#include "StdAfx.h"
#include "GUISchemeManager.h"
#include "GUIFactory.h"
#include "GUITemplateManager.h"
#include "GUIBuilderMgr.h"
#include "GUISystem.h"
#include "FileSystem.h"
#include "GUIAnimation.h"
#include "GUIButton.h"
#include "GUICheckBox.h"
#include "GUIEditBox.h"
#include "GUIImage.h"
#include "GUIListCtrl.h"
#include "GUIScrollBar.h"
#include "GUIListItem.h"
#include "GUISlider.h"
#include "GUIWindow.h"
#include "GUIStaticText.h"
#include "DebugTools.h"
#include "GUIDragImage.h"
#include "ImageSet.h"
//#include "InputSystem.h"

#include "StringUtil.h"
#include "FileData.h"

CGUISchemeManager::CGUISchemeManager() : m_bEditMode(false), m_szTextureListFile("")
, m_bImageSetReady(false)
, m_fMinZ(1)
, m_fMaxZ(10)
{
    for (int i = 0; i < UI_Layer_Max; ++i)
    {
        m_uiLayers[i].SetSchemeManager(this);
    }
}

CGUISchemeManager::~CGUISchemeManager(void)
{
}

void CGUISchemeManager::Clear()
{
    CleanUp();

    IMAGESET_ITER itEnd = m_mapImageSet.end();
    for (IMAGESET_ITER it = m_mapImageSet.begin(); it != itEnd; ++it)
    {
        CImageSet* pImageSet = it->second;
        RESOURCE_MANAGER->ClearResource(pImageSet);
    }

    m_mapImageSet.clear();
    //m_imgSetMgr.Clear();
}

bool CGUISchemeManager::Load(const SCHEME_DATA& scheme_data)
{
    for (unsigned int i = 0; i < scheme_data.vtResourceFiles.size(); i++)
    {
        if (!LoadResource(scheme_data.vtResourceFiles[i].c_str()))
        {
            return false;
        }
    }
    m_data = scheme_data;
    //m_bIsLoad = true;
    SetAfterLoad();
    return true;
}

/*
bool CGUISchemeManager::LoadFromXMLFile(const char* szFile)
{
    m_strFileName = FILESYSTEM->GetFileAbsolutePath(szFile);

    if (m_strFileName.length() == 0)
    {
        return false;
    }
    CMarkupSTL xml;
    if (!xml.Load(m_strFileName.c_str()))
    {
        return false;
    }
    //��һ������������
    if (!xml.FindElem())
    {
        return false;
    }

    return Load(xml);
}


bool CGUISchemeManager :: Load( CMarkupSTL& xml )
{
    
    if( !LoadSelf( xml ) )
    {
        return false;
    }

    xml.IntoElem();
    if( !LoadChild( xml ) )
    {
        return false;
    }
    xml.OutOfElem();

    m_bIsLoad = true;
    return true;
}

//��ȡ�ؼ������������
bool CGUISchemeManager :: LoadSelf( CMarkupSTL& xml )
{
    // ����GUI���ֹ�����������
    //if( ThisElement->Attribute( "Name" ) )
    //    this->m_strName = ThisElement->Attribute( "Name" );
    //m_strName = xml.GetAttrib("Name");

    return true;
}
*/
//��ȡ�����ɣ������ӿؼ�����
bool CGUISchemeManager :: LoadChild(CMarkupSTL& parent )
{
    //��õ�ǰ�ڵ�ĵ�һ���ӽڵ�
    //TiXmlElement* pChildElement = ThisElement->FirstChildElement();

    while(parent.FindElem())
    {
        //parent.IntoElem();
        if (parent.GetTagName() == "CGUIResource")
        {
            if( !Create( parent ) )
                return false;
        }
        

        //parent.OutOfElem();
        //�����һ���ڵ�
        //pChildElement = pChildElement->NextSiblingElement();
    }
    return true;
}

bool CGUISchemeManager :: Create( CMarkupSTL& xml )
{
    
    //CGUIWindow* m_pGUIWindow = NULL;
    AIRString temp_value = "";
    //const char* szFileName ;
    //const char* szElementName = ThisElement->Value();

    
    if( xml.GetTagName() == "CGUIResource" )
    {
        string strFileName = xml.GetAttrib( "File" );
        if( strFileName.length() != 0 )
        {
            if( AddResource( strFileName.c_str() ) )
            {
                if( !LoadResource( strFileName.c_str() ) )
                {
                    //ErrorMsg( STR_(4), FileName );
                    RemoveResource( strFileName.c_str() );
                    return false;
                }

                SetAfterLoad();
                return true;
            }
            else
            {
                //WarningMsg( STR_(5), FileName );
            }
        }
    }
    
    
    return false;
}


bool CGUISchemeManager::AddResource(const char* szResource)
{
    if (!FindResource(szResource))
    {
        m_data.vtResourceFiles.push_back(AIRString(szResource));
        return true;
    }
    return false;
}

const char* CGUISchemeManager::FindResource(const char* szResource)
{
    for (int i = 0; i != m_data.vtResourceFiles.size(); i++)
    {
        if (strcmp(szResource, m_data.vtResourceFiles[i].c_str()) == 0)
        {
            return m_data.vtResourceFiles[i].c_str();
        }
    }

    return NULL;
}

void CGUISchemeManager::RemoveResource(const char* szResource)
{
    for (AIRVector<AIRString>::iterator it = m_data.vtResourceFiles.begin(); it != m_data.vtResourceFiles.end(); it++)
    {
        if (strcmp(szResource, (*it).c_str()) == 0)
        {
            m_data.vtResourceFiles.erase(it);
            break;
        }
    }
}

bool CGUISchemeManager::LoadResource(const char* szResource)
{	
    return LoadFromFile(szResource, ENGINE_INST->GetConfig()->m_bBackgroundThread);
}

bool CGUISchemeManager::OnLButtonDown(int x, int y)
{
  //  if (m_pFocusElement)
  //  {
  //      m_pFocusElement->SetFocus(false);
		//m_pFocusElement = NULL;
  //  }
    CGUIElement* pFocusElement = GUISYSTEM->GetFocusElement();
    if (pFocusElement)
    {
        pFocusElement->SetFocus(false);
        GUISYSTEM->SetFocusElement(NULL);
        
    }

    CGUIElement* pDragElement = GUISYSTEM->GetDragElement();
    if (pDragElement)
    {
        GUISYSTEM->SetDragElement(NULL);
    }

    if (GUISYSTEM->GetMouseDragImage())
    {
        GUISYSTEM->SetMouseDragImage(NULL);
        return true;
    }
    
    /*
    CGUIElement* pEntity = NULL;
    //�ȴ������
    for (GUIELEMENT_RESITER it = m_lstElementTop.rbegin(); it != m_lstElementTop.rend(); ++it)
    {
        pEntity = *it;
        if (pEntity->IsVisable() && pEntity->IsInRect(x, y) && pEntity->IsEnable())
        {
            //GUIELEMENT_ITER curIt(it);
            m_lstElementTop.erase((++it).base());          //���������Ŀؼ�Ҫ����Ϸ���ʾ��
            m_lstElementTop.push_back(pEntity);
            //if (m_bEditMode)
            //{
            //    CGUIEditCtrl* pCtrl = GUISYSTEM->GetEditCtrl();
            //    pCtrl->AttachEditElement(pEntity);
            //}
            return pEntity->OnLButtonDown(x, y);
            //break;
        }
    }


    //�м��
    for (GUIELEMENT_RESITER it = m_lstElement.rbegin(); it != m_lstElement.rend(); ++it)
    {
        pEntity = *it;
        if (pEntity->IsVisable() && pEntity->IsInRect(x, y) && pEntity->IsEnable())
        {
            //GUIELEMENT_ITER curIt(it);
            m_lstElement.erase((++it).base());          //���������Ŀؼ�Ҫ����Ϸ���ʾ��
            m_lstElement.push_back(pEntity);
            //if (m_bEditMode)
            //{
            //    CGUIEditCtrl* pCtrl = GUISYSTEM->GetEditCtrl();
            //    pCtrl->AttachEditElement(pEntity);
            //}
	        return pEntity->OnLButtonDown(x, y);
        }
    }

    //��׶�
    for (GUIELEMENT_RESITER it = m_lstElementBottom.rbegin(); it != m_lstElementBottom.rend(); ++it)
    {
        pEntity = *it;
        if (pEntity->IsVisable() && pEntity->IsInRect(x, y) && pEntity->IsEnable())
        {
            //GUIELEMENT_ITER curIt(it);
            m_lstElementBottom.erase((++it).base());          //���������Ŀؼ�Ҫ����Ϸ���ʾ��
            m_lstElementBottom.push_back(pEntity);
            //if (m_bEditMode)
            //{
            //    CGUIEditCtrl* pCtrl = GUISYSTEM->GetEditCtrl();
            //    pCtrl->AttachEditElement(pEntity);
            //}
            return pEntity->OnLButtonDown(x, y);
        }
    }
    */
    for (size_t i = 1; i < UI_Layer_Max; ++i)
    {
        if (!m_uiLayers[i].OnLButtonDown(x, y))
        {
            return false;
        }
    }

	return false;
}

bool CGUISchemeManager::OnLButtonUp(int x, int y)
{ 
    CGUIElement* pDragElement = GUISYSTEM->GetDragElement();
    if (pDragElement)
    {
        return EndDragElement(GUISYSTEM->GetDragElement(), x, y);
    }

    /*
    CGUIElement* pEntity = NULL;
	
    //���
    for (GUIELEMENT_RESITER it = m_lstElementTop.rbegin(); it != m_lstElementTop.rend(); ++it)
    {
        pEntity = *it;
        if (pEntity->IsVisable() && pEntity->IsInRect(x, y) && pEntity->IsEnable())
        {
            return pEntity->OnLButtonUp(x, y);

        }
    }

    //�м�
    for (GUIELEMENT_RESITER it = m_lstElement.rbegin(); it != m_lstElement.rend(); ++it)
    {
        pEntity = *it;
        if (pEntity->IsVisable() && pEntity->IsInRect(x, y) && pEntity->IsEnable())
        {
            return pEntity->OnLButtonUp(x, y);
			
        }
    }

    //��׶�
    for (GUIELEMENT_RESITER it = m_lstElementBottom.rbegin(); it != m_lstElementBottom.rend(); ++it)
    {
        pEntity = *it;
        if (pEntity->IsVisable() && pEntity->IsInRect(x, y) && pEntity->IsEnable())
        {
            return pEntity->OnLButtonUp(x, y);

        }
    }
    */

    CGUIDragImage* pDragImage = GUISYSTEM->GetMouseDragImage();
    if (pDragImage)
    {
        //����ûdrop�ɹ���pDragImage
        pDragImage->OnDragToNull();
        GUISYSTEM->GetGUIMouse()->SetImageSrc("");
    }

    for (size_t i = 1; i < UI_Layer_Max; ++i)
    {
        if (!m_uiLayers[i].OnLButtonUp(x, y))
        {
            return false;
        }
    }

	return false;
}

bool CGUISchemeManager::OnRButtonDown(int x, int y)
{
    /*
    CGUIElement* pEntity = NULL;
    //���
    for (GUIELEMENT_RESITER it = m_lstElementTop.rbegin(); it != m_lstElementTop.rend(); ++it)
    {
        pEntity = *it;
        if (pEntity->IsInRect(x, y) && pEntity->IsVisable() && pEntity->IsEnable())
        {
            return pEntity->OnRButtonDown(x, y);

        }
    }

    for (GUIELEMENT_RESITER it = m_lstElement.rbegin(); it != m_lstElement.rend(); ++it)
    {
        pEntity = *it;
        if (pEntity->IsInRect(x, y) && pEntity->IsVisable() && pEntity->IsEnable())
        {
            return pEntity->OnRButtonDown(x, y);
			
        }
    }

    //��׶�
    for (GUIELEMENT_RESITER it = m_lstElementBottom.rbegin(); it != m_lstElementBottom.rend(); ++it)
    {
        pEntity = *it;
        if (pEntity->IsInRect(x, y) && pEntity->IsVisable() && pEntity->IsEnable())
        {
            return pEntity->OnRButtonDown(x, y);
        }
    }
    */

    for (size_t i = 1; i < UI_Layer_Max; ++i)
    {
        if (!m_uiLayers[i].OnRButtonDown(x, y))
        {
            return false;
        }
    }
	return false;
}

bool CGUISchemeManager::OnRButtonUp(int x, int y)
{
    /*
    CGUIElement* pEntity = NULL;
    //���
    for (GUIELEMENT_RESITER it = m_lstElementTop.rbegin(); it != m_lstElementTop.rend(); ++it)
    {
        pEntity = *it;
        if (pEntity->IsVisable() && pEntity->IsInRect(x, y) && pEntity->IsEnable())
        {
            return pEntity->OnRButtonUp(x, y);
        }
    }

    for (GUIELEMENT_RESITER it = m_lstElement.rbegin(); it != m_lstElement.rend(); ++it)
    {
        pEntity = *it;
        if (pEntity->IsVisable() && pEntity->IsInRect(x, y) && pEntity->IsEnable())
        {
            return pEntity->OnRButtonUp(x, y);
        }
    }

    //��׶�
    for (GUIELEMENT_RESITER it = m_lstElementBottom.rbegin(); it != m_lstElementBottom.rend(); ++it)
    {
        pEntity = *it;
        if (pEntity->IsVisable() && pEntity->IsInRect(x, y) && pEntity->IsEnable())
        {
            return pEntity->OnRButtonUp(x, y);
        }
    }
    */

    for (size_t i = 1; i < UI_Layer_Max; ++i)
    {
        if (!m_uiLayers[i].OnRButtonUp(x, y))
        {
            return false;
        }
    }
	return false;
}

bool CGUISchemeManager::OnMouseMove(int x, int y)
{
    CGUIElement* pHoverElement = GUISYSTEM->GetHoverElement();
    //if (pHoverElement && !pHoverElement->IsInRect(x, y))
    //{
    //    pHoverElement->SetUICurrentState(UI_NORMAL);
    //    GUISYSTEM->SetHoverElement(NULL);
    //}
	if ( pHoverElement && pHoverElement->IsVisable() && pHoverElement->IsEnable() )
	{
		if ( !pHoverElement->IsInRect(x, y) )
		{
		    pHoverElement->SetCurrentUIState(UI_NORMAL);
		    GUISYSTEM->SetHoverElement(NULL);
		}
	}

    static int nLastX = x;
    static int nLastY = y;

    CGUIElement* pDragElement = GUISYSTEM->GetDragElement();
    if (pDragElement)
    {
        
        int dx = x - nLastX;
        int dy = y - nLastY;
        
        if (pDragElement == GUISYSTEM->GetEditCtrl())
        {
            //CGUIEditCtrl* pEditCtrl = GUISYSTEM->GetEditCtrl();
            //if (pEditCtrl->GetClickSide() == CGUIEditCtrl::Side_Inner)
            //{
            //    pEditCtrl->Move(dx, dy);
            //}
            //else
            //    pEditCtrl->Resize(dx, dy);
        }
        else    //cut by liangairan at 2010-7-2
        {
            //OutputDebugString("move drag element!\n");
            pDragElement->Move(dx, dy);
        }
        nLastX = x;
        nLastY = y;
        return true;
    }
    nLastX = x;
    nLastY = y;

    /*
    CGUIElement* pEntity = NULL;

    //���
    for (GUIELEMENT_RESITER it = m_lstElementTop.rbegin(); it != m_lstElementTop.rend(); ++it)
    {
        pEntity = *it;
        if (pEntity->IsVisable() && pEntity->IsInRect(x, y) && pEntity->IsEnable())
        {
            return pEntity->OnMouseMove(x, y);
            //m_pHoverElement = pEntity->OnMouseMove(x, y);
        }
        else if (pEntity == GUISYSTEM->GetDragElement())
        {
            //EndDragElement();
        }
    }

    for (GUIELEMENT_RESITER it = m_lstElement.rbegin(); it != m_lstElement.rend(); ++it)
    {
        pEntity = *it;
        if (pEntity->IsVisable() && pEntity->IsInRect(x, y) && pEntity->IsEnable())
        {
            return pEntity->OnMouseMove(x, y);
            //m_pHoverElement = pEntity->OnMouseMove(x, y);
        }
		else if (pEntity == GUISYSTEM->GetDragElement())
        {
			//EndDragElement();
        }
    }

    //��׶�
    for (GUIELEMENT_RESITER it = m_lstElementBottom.rbegin(); it != m_lstElementBottom.rend(); ++it)
    {
        pEntity = *it;
        if (pEntity->IsVisable() && pEntity->IsInRect(x, y) && pEntity->IsEnable())
        {
            return pEntity->OnMouseMove(x, y);
            //m_pHoverElement = pEntity->OnMouseMove(x, y);
        }
        else if (pEntity == GUISYSTEM->GetDragElement())
        {
            //EndDragElement();
        }
    }
    */
    for (size_t i = 1; i < UI_Layer_Max; ++i)
    {
        if (!m_uiLayers[i].OnMouseMove(x, y))
        {
            return false;
        }
    }

	return false;
}

bool CGUISchemeManager::OnMouseWheel(int dz)
{
    CGUIElement* pFocusElement = GUISYSTEM->GetFocusElement();
    if (pFocusElement)
    {
        return pFocusElement->OnMouseWheel(dz);
    }

    return false;
}

void CGUISchemeManager::Update()
{
    for (size_t i = 0; i < UI_Layer_Max; ++i)
    {
        m_uiLayers[i].Update();
    }
}

void CGUISchemeManager :: DragElement(int dx, int dy)
{
	//if(m_pDragElement)
	//{
	//	m_pDragElement->Move(dx, dy);

	//}
}
bool CGUISchemeManager :: StartDragElement(CGUIElement* e)
{
	//if(m_pDragElement == e)
	//	return false;

	//m_pDragElement = e;
	//m_pDragElement->SetFocus(false);
	return true;
}

bool CGUISchemeManager :: EndDragElement(CGUIElement* e, int x, int y)
{
    GUISYSTEM->SetDragElement(NULL);

    CGUIElement* pEntity = NULL;

    /*
    for (GUIELEMENT_RESITER it = m_lstElement.rbegin(); it != m_lstElement.rend(); it++)
    {
        pEntity = *it;
        if (pEntity->IsInRect(x, y) && pEntity->IsVisable())
        {
            return pEntity->OnDrag(x, y);

        }
    }
    */
    return false;
	//m_pDragElement = NULL;
}

bool CGUISchemeManager::ProcessInput(const LPINPUTEVENT pEvent)
{
    switch(pEvent->nType)
    {
    case eINPUT_MOUSE_LBUTTONDOWN:
        return OnLButtonDown(pEvent->x, pEvent->y);
        break;
    case eINPUT_MOUSE_LBUTTONUP:
        return OnLButtonUp(pEvent->x, pEvent->y);
        break;
    case eINPUT_MOUSE_RBUTTONDOWN:
        return OnRButtonDown(pEvent->x, pEvent->y);
        break;
    case eINPUT_MOUSE_RBUTTONUP:
        return OnRButtonUp(pEvent->x, pEvent->y);
        break;
    case eINPUT_MOUSE_MOVE:
        return OnMouseMove(pEvent->x, pEvent->y);
        break;
    case eINPUT_MOUSE_WHEEL:
        return OnMouseWheel(pEvent->nWheel);
        break;
    case eINPUT_KEYDOWN:
        return OnKeyPress(pEvent->nKey);
        break;
    case eINPUT_KEYUP:
		return OnKeyUp(pEvent->nKey);
        break;
    case eINPUT_MOUSE_LBUTTONDBCLICK:
        return OnLButtonDBClick(pEvent->x, pEvent->y);
        break;
    }
    return false;

}

bool CGUISchemeManager::OnLButtonDBClick(int x, int y)
{
    /*
    CGUIElement* pEntity = NULL;
    //���
    for (GUIELEMENT_RESITER it = m_lstElementTop.rbegin(); it != m_lstElementTop.rend(); ++it)
    {
        pEntity = *it;
        if (pEntity->IsVisable() && pEntity->IsInRect(x, y) && pEntity->IsEnable())
        {
            return pEntity->OnLButtonDBClick(x, y);
        }
    }

    for (GUIELEMENT_RESITER it = m_lstElement.rbegin(); it != m_lstElement.rend(); ++it)
    {
        pEntity = *it;
        if (pEntity->IsVisable() && pEntity->IsInRect(x, y) && pEntity->IsEnable())
        {
            return pEntity->OnLButtonDBClick(x, y);
        }
    }

    //��׶�
    for (GUIELEMENT_RESITER it = m_lstElementBottom.rbegin(); it != m_lstElementBottom.rend(); ++it)
    {
        pEntity = *it;
        if (pEntity->IsVisable() && pEntity->IsInRect(x, y) && pEntity->IsEnable())
        {
            return pEntity->OnLButtonDBClick(x, y);
        }
    }
    */

    for (size_t i = 1; i < UI_Layer_Max; ++i)
    {
        if (m_uiLayers[i].OnLButtonDBClick(x, y))
        {
            return true;
        }
    }
    return false;
}

bool CGUISchemeManager::OnKeyPress(int nKey)
{
    CGUIElement* pFocusElement = GUISYSTEM->GetFocusElement();
    if (pFocusElement)
    {
        return pFocusElement->OnKeyPress(nKey);
    }
    return false;
}

bool CGUISchemeManager::OnKeyUp(int nKey)
{
    /*
    CGUIElement* pEntity = NULL;
    //���
    for (GUIELEMENT_RESITER it = m_lstElementTop.rbegin(); it != m_lstElementTop.rend(); it++)
    {
        pEntity = *it;
        if (pEntity->IsVisable() && pEntity->IsEnable())
        {
            return pEntity->OnKeyUp(nKey);
            //m_pHoverElement = pEntity->OnMouseMove(x, y);
        }

    }

    for (GUIELEMENT_RESITER it = m_lstElement.rbegin(); it != m_lstElement.rend(); it++)
    {
        pEntity = *it;
        if (pEntity->IsVisable() && pEntity->IsEnable())
        {
            return pEntity->OnKeyUp(nKey);
            //m_pHoverElement = pEntity->OnMouseMove(x, y);
        }

    }

    //��׶�
    for (GUIELEMENT_RESITER it = m_lstElementBottom.rbegin(); it != m_lstElementBottom.rend(); it++)
    {
        pEntity = *it;
        if (pEntity->IsVisable() && pEntity->IsEnable())
        {
            return pEntity->OnKeyUp(nKey);
            //m_pHoverElement = pEntity->OnMouseMove(x, y);
        }

    }
    */

    for (size_t i = 1; i < UI_Layer_Max; ++i)
    {
        if (m_uiLayers[i].OnKeyUp(nKey))
        {
            return true;
        }
    }
    return false;
}


CImageSet* CGUISchemeManager::LoadImageSet(const char* szFilename)
{
    //return m_imgSetMgr.LoadFromXML(szFilename);
    CImageSet* pImageSet = GetImageSet(szFilename);

    if (pImageSet == NULL)
    {
        //pImageSet = static_cast<CImageSet*>(RESOURCE_MANAGER->CreateResource(Resource::Res_ImageSet, szFilename));
        pImageSet = RESOURCE_MANAGER->CreateResource<CImageSet>(szFilename);

        if (pImageSet)
        {
            m_mapImageSet.insert(std::make_pair(pImageSet->GetFilename(), pImageSet));
            //if (!pImageSet->IsLoaded())
            //{
            //    pImageSet->LoadFromFile(szFilename, ENGINE_INST->GetConfig()->m_bBackgroundThread);
            //}

            m_bReady = false;
        }
    }
    

    return pImageSet;
}

CImageSet* CGUISchemeManager::GetImageSet(const AIRString& strImageSetName)
{
    //return m_imgSetMgr.GetImageSet(strImageSetName);
    IMAGESET_ITER it = m_mapImageSet.find(strImageSetName);

    if (it != m_mapImageSet.end())
    {
        return it->second;
    }

    return NULL;
}

CImageSet* CGUISchemeManager::GetImageSetByName(const char* strImageSetName)
{
    IMAGESET_ITER itEnd = m_mapImageSet.end();
    for (IMAGESET_ITER it = m_mapImageSet.begin(); it != itEnd; ++it)
    {
        CImageSet* pImageSet = it->second;
        if (strcmp(pImageSet->GetName().c_str(), strImageSetName) == 0)
        {
            return pImageSet;
        }
    }

    return NULL;
}

CImageUnit* CGUISchemeManager::GetImageUnit(const AIRString& strImageSetName, const AIRString& strImageUnitName)
{
    //return m_imgSetMgr.GetImageUnit(strImageSetName, strImageUnitName);
    IMAGESET_ITER it = m_mapImageSet.find(strImageSetName);

    if (it != m_mapImageSet.end())
    {
        CImageSet* pImageSet = it->second;
        return pImageSet->GetImgUnit(strImageUnitName);
    }

    return NULL;
}

/*
bool CGUISchemeManager::IsLoad() const
{
    return m_bIsLoad;
}
*/

CGUIElement* CGUISchemeManager::CreateDefaultElement(const AIRString& strType, CGUIElement* pParent)
{
    CGUIElement* pElement = NULL;
    
    CGUIBuilder* pBuilder = UIBUILDERMGR->GetBuilder(strType);
    
    pElement = pBuilder->BuildElement(this);
    if (pElement && !pParent)
    {
        AddEntity(pElement);
    }
    else if (pElement && pParent)
    {
        pParent->AddChild(pElement);
    }

    return pElement;
}

void CGUISchemeManager::SetAfterLoad()
{
    f32 fZDelta = (m_fMaxZ - m_fMinZ) / UI_Layer_Max;
    f32 fMin = m_fMinZ + fZDelta;
    for (int i = 0; i < UI_Layer_Max; i++)
    {
        m_uiLayers[i].SetMinZ(fMin);
        m_uiLayers[i].SetMaxZ(fMin + fZDelta);
        fMin += fZDelta;
        m_uiLayers[i].SetAfterLoad();
        m_uiLayers[i].CaculateLayerDepth();
    }
}

void CGUISchemeManager::SetEditMode(bool bEdit)
{
    m_bEditMode = bEdit;
    CGUIEditCtrl* pEditCtrl = GUISYSTEM->GetEditCtrl();
    if (m_bEditMode)
    {

        if (pEditCtrl == NULL)
        {
            pEditCtrl = new CGUIEditCtrl;
            //AddEntity(pEditCtrl);
            GUISYSTEM->SetEditCtrl(pEditCtrl);
        }
    }
    else
    {
        if (pEditCtrl)
        {
            pEditCtrl->Hide();
            pEditCtrl->CleanUp();
            SafeDelete(pEditCtrl);
            GUISYSTEM->SetEditCtrl(NULL);
        }
    }
}


bool	CGUISchemeManager::SaveToXML()
{
	for (unsigned int i = 0; i < m_data.vtResourceFiles.size(); i++)
	{
		CMarkupSTL xml;
		xml.SetDoc("<?xml version=\"1.0\" encoding=\"gb2312\"?>\r\n");
		xml.AddElem("UICONFIG");
		//// ps:�ɵı���imageset��texturelist�ļ��ķ�ʽ
		//if( m_szTextureListFile.length() > 0 )
		//{
		//	xml.AddAttrib( "TextureListFile", m_szTextureListFile.c_str() );
		//	m_imgSetMgr.SaveToXML( m_szTextureListFile.c_str() );
		//}
		xml.IntoElem();

		// ����imageset��scheme�ļ�
		//m_imgSetMgr.SaveToXML( xml );

        for (IMAGESET_ITER it = m_mapImageSet.begin(); it != m_mapImageSet.end(); ++it)
        {
            CImageSet* pImageSet = it->second;
            xml.AddElem( "ImageSet" );
            //xml.AddAttrib( "Name", itor->first.c_str() );
            xml.AddAttrib( "File", pImageSet->GetFilename().c_str() );
        }

		// ����ؼ���Ϣ
		//CGUIEntityList::SaveToXML( xml );

        for (int j = 0; i < UI_Layer_Max; ++j)
        {
            m_uiLayers[j].SaveToXML(xml);
        }

		xml.OutOfElem();
		AIRString file("E:\\");////////////////////////////////////////////////////////////
		file += m_data.vtResourceFiles[i];
		xml.Save(file.c_str());
	}
	return true;
}


bool	CGUISchemeManager::SaveToXML(const char* szFileName)
{
	CMarkupSTL xml;
	xml.SetDoc("<?xml version=\"1.0\" encoding=\"gb2312\"?>\r\n");
	xml.AddElem("UICONFIG");
	//// ps:�ɵı���imageset��texturelist�ļ��ķ�ʽ
	//if( m_szTextureListFile.length() > 0 )
	//{
	//	xml.AddAttrib( "TextureListFile", m_szTextureListFile.c_str() );
	//	m_imgSetMgr.SaveToXML( m_szTextureListFile.c_str() );
	//}
	xml.IntoElem();

	// ����imageset��scheme�ļ�
    for ( IMAGESET_ITER itor = m_mapImageSet.begin(); itor != m_mapImageSet.end(); ++itor )
    {
        xml.AddElem( "ImageSet" );
        //xml.AddAttrib( "Name", itor->first.c_str() );
        xml.AddAttrib( "File", itor->second->GetFilename().c_str() );
    }
	//m_imgSetMgr.SaveToXML( xml );
	// ����ؼ���Ϣ
	//CGUIEntityList::SaveToXML( xml );
    for (int i = 0; i < UI_Layer_Max; ++i)
    {
        m_uiLayers[i].SaveToXML(xml);
    }

	xml.OutOfElem();
	xml.Save(szFileName);
	return true;
}

/*
CImageSetManager*		CGUISchemeManager::GetImageSetMgr( void )
{
	return &m_imgSetMgr;
}
*/

void CGUISchemeManager::MoveElementToTop(CGUIElement* pElement)
{
    /*
    UILayerDefine viewState = pElement->GetViewState();
    switch (viewState)
    {
    case UI_VIEW_NORMAL:
        {
            GUIELEMENT_RESITER it = std::find(m_lstElement.rbegin(), m_lstElement.rend(), pElement);
            if (it != m_lstElement.rend())
            {
                m_lstElement.erase((++it).base());
                m_lstElement.push_back(pElement);
            }

        }
        break;
    case UI_ALWAYS_TOP:
        {
            GUIELEMENT_RESITER it = std::find(m_lstElementTop.rbegin(), m_lstElementTop.rend(), pElement);
            if (it != m_lstElementTop.rend())
            {
                m_lstElementTop.erase((++it).base());
                m_lstElementTop.push_back(pElement);
            }
        }
        break;
    case UI_ALWAYS_BOTTOM:
        {
            GUIELEMENT_RESITER it = std::find(m_lstElementBottom.rbegin(), m_lstElementBottom.rend(), pElement);
            if (it != m_lstElementBottom.rend())
            {
                m_lstElementBottom.erase((++it).base());
                m_lstElementBottom.push_back(pElement);
            }
        }
        break;
    }
    */
}

bool CGUISchemeManager::Cache()
{
    // ���scheme���ļ�������
    //CDataStream* pStream = m_pSchemeResource->GetFileStream(); //FILESYSTEM->GetFileStream(szResource);
    // ��ȡ����ʧ��,����
    if (m_pStream && m_mapImageSet.size() == 0)
    {
        // ��ȡXML�ڵ���
        AIRString strDoc = m_pStream->GetAsString();
        CMarkupSTL xml(strDoc.c_str());

        // ���XMLû����Ϣ,����
        if (!xml.FindElem())
        {
            //SafeDelete(pStream);
            return false;
        }

        AIRString value, szImgSet;

        // ps:ʹ�þɵķ�ʽ
        // ��texturelist�ļ�����imageset��Ϣ
        value = xml.GetAttrib("TextureListFile").c_str();
        if (value.length() > 0)
        {
            m_szTextureListFile = value;
            LoadImageSet(value.c_str());
        }

        xml.IntoElem();
        while (xml.FindElem())
        {
            // ��ýڵ������
            szImgSet = value = xml.GetTagName().c_str();
            CStringUtil::ToUpperCase( szImgSet );
            // �����ǰ�ڵ���Imageset,��ʾ����Imageset��Ϣ
            if ( szImgSet == "IMAGESET" )
            {
                // ���ص�ǰ��Imageset
                /*
                if ( !m_imgSetMgr.LoadSingleImageSet( xml ) )
                {
                    return false;
                }
                */
                AIRString strImageSetFile = xml.GetAttrib("File").c_str();
                LoadImageSet(strImageSetFile.c_str());
            }
        }

        value = xml.GetAttrib("MinZ").c_str();
        m_fMinZ = atof(value.c_str());

        value = xml.GetAttrib("MaxZ").c_str();
        m_fMaxZ = atof(value.c_str());
    }

    if (!m_bImageSetReady)
    {
        bool bImageSetOK = true;
        //����imageset
        IMAGESET_ITER itEnd = m_mapImageSet.end();
        for (IMAGESET_ITER it = m_mapImageSet.begin(); it != itEnd; ++it)
        {
            CImageSet* pImageSet = it->second;
            if (!pImageSet->IsReady())
            {
                if (pImageSet->IsLoaded())
                {
                    if (!pImageSet->Cache())
                    {
                        bImageSetOK = false;
                    }
                }
                else
                {
                    bImageSetOK = false;
                }
            }
        }
        m_bImageSetReady = bImageSetOK;
    }

    if (m_bImageSetReady && m_pStream)
    {
        // ��ȡXML�ڵ���
        AIRString strDoc = m_pStream->GetAsString();
        CMarkupSTL xml(strDoc.c_str());

        // ���XMLû����Ϣ,����
        if (!xml.FindElem())
        {
            //SafeDelete(pStream);
            return false;
        }

        AIRString value, szImgSet;

        xml.IntoElem();
        while (xml.FindElem())
        {
            // ��ýڵ������
            szImgSet = value = xml.GetTagName().c_str();
            CStringUtil::ToUpperCase( szImgSet );
            // �����ǰ�ڵ���Imageset,��ʾ����Imageset��Ϣ
            if ( szImgSet != "IMAGESET" )
            {
                CGUIBuilder* pBuilder = CGUIBuilderMgr::GetInstance()->GetBuilder(value.c_str());
                if (!pBuilder)
                {
                    DT_TO_MSGBOX_FMT( LT_ERROR, GetString("�Ҳ����ؼ�Builder:%s",value.c_str()) );
                    continue;
                }
                CGUIElement* ctrl = pBuilder->BuildElement(NULL, xml, this);
                if (ctrl)
                {
                    AddEntity(ctrl);
                }
                else
                {
                    DT_TO_MSGBOX_FMT( LT_ERROR, GetString("�����ؼ�ʧ��!",value.c_str()) );
                    continue;
                }
            }
        }

        xml.OutOfElem();

        SetAfterLoad();

        SafeDelete(m_pStream);

        m_bReady = true;
        NotifyAllListenerReady();
    }
    
    return m_bReady;
}

void CGUISchemeManager::CleanUp()
{
    for (size_t i = 0; i < UI_Layer_Max; ++i)
    {
        m_uiLayers[i].CleanUp();
    }
}

void CGUISchemeManager::AddEntity(CGUIElement* pElement)
{
    m_uiLayers[(int)pElement->GetLayerDef()].AddEntity(pElement);
}

void CGUISchemeManager::RemoveEntity(CGUIElement* pEntity)
{
    m_uiLayers[(int)pEntity->GetLayerDef()].RemoveEntity(pEntity);
}

void CGUISchemeManager::Render()
{
    for (size_t i = 0; i < UI_Layer_Max; ++i)
    {
        m_uiLayers[i].Render();
    }

    GUIRENDERER->RenderImmediatelyWithDepth();
}

//�������ֲ��ҿؼ�
CGUIElement* CGUISchemeManager::FindCtrl(const char* szCtrlName)
{
    CGUIElement* pElement = NULL;

    for (int i = 0; i < UI_Layer_Max; ++i)
    {
        pElement = m_uiLayers[i].FindCtrl(szCtrlName);
        if (pElement)
        {
            return pElement;
        }
    }
    return pElement;
}

void CGUISchemeManager::CaculateEntityAlignPosition()
{
    for (size_t i = 0; i < UI_Layer_Max; ++i)
    {
        m_uiLayers[i].CaculateEntityAlignPosition();
    }
}

void CGUISchemeManager::ClearEntity(CGUIElement* pEntity)
{
    m_uiLayers[pEntity->GetLayerDef()].ClearEntity(pEntity);
}
