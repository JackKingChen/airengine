#include "stdafx.h"

#include "GEffect.h"
#include "SceneNode.h"
#include "MarkupSTL.h"
#include "FileSystem.h"
#include "ParticleSystem.h"
#include "ISceneManager.h"
#include "DataStream.h"
#include "TerrainDecal.h"
#include "ModelInstance.h"
#include "MaterialManager.h"
#include "RibbonTrail.h"
#include "Billboard.h"
#include "TextureMgr.h"
#include "FileData.h"
#include "StringUtil.h"

const AIRString ParticleSystemEffect::C_CHILD_GEFFECT_PARTICLE = "ParticleSystemEffect";
const AIRString DecalEffect::C_CHILD_GEFFECT_DECAL = "DecalEffect";
const AIRString ModelEffect::C_CHILD_GEFFECT_MODEL = "ModelEffect";
const AIRString RibbonTrailEffect::C_CHILD_GEFFECT_RIBBON_TRAIL = "RibbonTrailEffect";
const AIRString BillboardEffect::C_CHILD_GEFFECT_BILLBOARD = "BillboardEffect";

//-----------------------------------------------

void GEffectElementInfo::operator	= ( GEffectElementInfo& other ) 
{ 
	m_LifeTime = other.GetLifeTime();
	m_StartTime = other.GetStartTime();
	m_fIterativeInterval = other.GetIterativeInterval();
	m_Transform = other.GetTransform();
	m_szRenderableType = other.GetRenderableType();
	m_szRenderableFile = other.GetRenderableFile();
}

//-----------------------------------------------
/*
GEffectResource::~GEffectResource()
{
	Clear();
}

bool	GEffectResource::Cache()
{
	if ( m_pCreator == NULL )
	{
		DT_TO_MSGBOX( LT_ERROR, "You have to set the effect creator before catch resource!" );
		m_bReady = false;
		return false;
	}

	if (m_pStream)
	{
		AIRString strDoc = m_pStream->GetAsString();

		CMarkupSTL xml;
		xml.SetDoc(strDoc.c_str());

		if (!xml.FindElem())
		{
			m_pCreator = NULL;
			m_bReady = false;
			return false;
		}

		AIRString szValue;
		szValue = xml.GetAttrib( "Loop" );
		if ( szValue == "TRUE" )
		{
			m_pCreator->SetLoop( true );
		}
		else if ( szValue == "FALSE" )
		{
			m_pCreator->SetLoop( false );
		}

		szValue = xml.GetAttrib( "DelayTimeLen" );
		if ( szValue.length() )
		{
			m_pCreator->SetDelayTimeLen( atof(szValue.c_str()) );
		}

		szValue = xml.GetAttrib( "LifeTime" );
		if ( szValue.length() )
		{
			m_pCreator->GetEffectElementInfo().SetLifeTime( atof(szValue.c_str()) );
		}		

		szValue = xml.GetAttrib( "StartTime" );
		if ( szValue.length() )
		{
			m_pCreator->GetEffectElementInfo().SetStartTime( atof(szValue.c_str()) );
		}	

		szValue = xml.GetAttrib( "IterativeInterval" );
		if ( szValue.length() )
		{
			m_pCreator->GetEffectElementInfo().SetIterativeInterval( atof(szValue.c_str()) );
		}

		szValue = xml.GetAttrib( "Position" );
		if ( szValue.length() )
		{
			Vector3Df& vPos = m_pCreator->GetEffectElementInfo().GetTransform().m_Position;
			sscanf( szValue.c_str(), "(%f,%f,%f)", &vPos.x, &vPos.y, &vPos.z );
		}

		szValue = xml.GetAttrib( "Rotation" );
		if ( szValue.length() )
		{
			Quaternion& qRotate = m_pCreator->GetEffectElementInfo().GetTransform().m_Rotation;
			sscanf( szValue.c_str(), "(%f,%f,%f,%f)", &qRotate.x, &qRotate.y, &qRotate.z, &qRotate.w );
		}

		xml.IntoElem();
		while(xml.FindElem())
		{
			if (xml.GetTagName() == "GEffectElement")
			{
				GEffectElementInfo info;

				szValue = xml.GetAttrib( "LifeTime" );
				info.SetLifeTime( atof(szValue.c_str()) );

				szValue = xml.GetAttrib( "StartTime" );
				info.SetStartTime( atof(szValue.c_str()) );

				szValue = xml.GetAttrib( "IterativeInterval" );
				info.SetIterativeInterval( atof(szValue.c_str()) );

				szValue = xml.GetAttrib( "Position" );
				Vector3Df& vChildPos = info.GetTransform().m_Position;
				sscanf( szValue.c_str(), "(%f,%f,%f)", &vChildPos.x, &vChildPos.y, &vChildPos.z );

				szValue = xml.GetAttrib( "Rotation" );
				Quaternion& qChildRotate = info.GetTransform().m_Rotation;
				sscanf( szValue.c_str(), "(%f,%f,%f,%f)", &qChildRotate.x, &qChildRotate.y, &qChildRotate.z, &qChildRotate.w );

				info.SetRenderableType( xml.GetAttrib( "ChildEffectType" ).c_str() );

				info.SetRenderableFile( xml.GetAttrib( "RenderableFile" ).c_str() );

				GEffect* pChildEffect = m_pCreator->CreateChildGEffect( &info );
				pChildEffect->LoadChildFromXML( xml );
			}
		}
		xml.OutOfElem();

		m_pCreator = NULL;
		m_bReady = true;
		return true;
	}
	m_bReady = false;
	return false;
}

void	GEffectResource::Clear()
{
	if (m_pStream)
	{
		SafeDelete(m_pStream);
	}
	m_pStream = NULL;
}
*/
//-----------------------------------------------

GEffect::GEffect():m_pRenderObject(NULL)
,m_pParentGEffect(NULL)
,m_pResource(NULL)
//,m_bIsActive(false)
//,m_bIsStoped(false)
//,m_fCurrentLifeTime(0.0f)
//,m_fCurrentStartTime(0.0f)
//,m_fIterateRemainTime(0.0f)
, m_fCurrentTime(0)
,m_pLinkNode(NULL)
,m_bLoop(false)
,m_fDelayTimeLen(0)
,m_pLoopBackupNode(NULL)
, m_state(eES_Inactive)
, m_bResourceReady(false)
{
	m_szType = "GEffect";
	m_Info.SetRenderableType( m_szType.c_str() );
}

GEffect::~GEffect()
{

}

void GEffect::Clear()
{
	m_pLoopBackupNode = NULL;
	int i = 0;
	int c = m_vtChildGEffects.size();
	GEffect* pChildEffect = NULL;
	for ( ; i < c; ++i )
	{
		pChildEffect = m_vtChildGEffects[i];
		pChildEffect->Clear();
		SafeDelete( pChildEffect );
	}
	m_vtChildGEffects.clear();
	ClearSelf();

	Renderable::Clear();
}

void GEffect::ClearSelf()
{
	CSceneNode* pNode = NULL;
	if ( m_pRenderObject )
	{
		if ( m_pLinkNode && m_pLinkNode->GetParent() )
		{
			m_pLinkNode->GetParent()->RemoveChild( m_pLinkNode );
		}
		m_pLinkNode->DetachAllRenderable();
        m_pLinkNode->GetCreator()->DestroySceneNode(m_pLinkNode);
		//SafeDelete( m_pLinkNode );
		
        ISceneManager* pSceneMgr = ENGINE_INST->GetSceneMgr();
        pSceneMgr->DestroyRenderable(m_pRenderObject);
	}

	if ( m_pResource )
	{
		RESOURCE_MANAGER->ClearResource(m_pResource);
        m_pResource = NULL;
	}

	m_pLinkNode = NULL;
	m_pRenderObject = NULL;
	m_pParentGEffect = NULL;
	m_pLoopBackupNode = NULL;
}

/*
bool GEffect::CompleteStopImpl( f32 fFrameTime )
{
    //根特效会调用这个
	m_bIsCompleted = true;
	size_t i = 0;
	size_t c = m_vtChildGEffects.size();
	GEffect* pChildEffect = NULL;
	for ( ; i < c; ++i )
	{
		pChildEffect = m_vtChildGEffects[i];
		if ( false == pChildEffect->CompleteStopImpl( fFrameTime ) )
		{
			m_bIsCompleted = false;
		}
	}

	if ( m_bIsCompleted )
	{
		//assert( m_pNode );
		if ( m_pNode )
		{
			if ( m_bLoop )
			{
				m_pLoopBackupNode = m_pNode;
			}
			m_pNode->DetachRenderable( this );
			m_pNode = NULL;
		}
	}

	return m_bIsCompleted;
}
*/

void GEffect::StartRenderableObject()
{
    if (m_pRenderObject)
    {
        m_pRenderObject->SetVisable(true);
    }
}

void GEffect::Update( f32 fTimeElapsed )
{
    if (m_pParentGEffect)
    {
        if (m_pLinkNode && !m_bPrepared)
        {
            Prepare();
        }
    }
    else
    {
	    if (m_pNode && !m_bPrepared)
	    {
		    Prepare();
	    }	
    }

    /*
	//父特效的处理
    if (m_bPrepared)
    {
        if ( m_pParentGEffect == NULL )
        {
            //如果父特效已经停止播放
            if ( m_bIsStoped )
            {
                //执行停止操作
                if ( m_bIsCompleted == false )
                {
                    CompleteStopImpl( fTimeElapsed );
                    m_fDelayTimeCounter = 0.0f;
                }			
                else if ( m_bLoop && m_pLoopBackupNode )
                {
                    m_fDelayTimeCounter += fTimeElapsed;
                    if ( m_fDelayTimeCounter >= m_fDelayTimeLen )
                    {
                        Start();
                    }
                }
                return;
            }

            //如果父特效没激活,就返回
            if ( m_bIsActive == false )
            {
                return;
            }
        }

        //如果特效更新间隔时间>0
        f32 fIterativeInterval = m_Info.GetIterativeInterval();
        if ( fIterativeInterval > 0 )
        {
            //更新计时器
            m_fIterateRemainTime += fTimeElapsed;

            //如果更新计时器 >= 特效更新间隔时间
            while ( m_fIterateRemainTime >= fIterativeInterval )
            {
                //更新子特效
                UpdateChild( fTimeElapsed );

                //更新特效自身
                UpdateSelf( fTimeElapsed );			

                //重置更新计时器
                m_fIterateRemainTime -= fIterativeInterval;
            }				
        }
        else
        {
            //更新子特效
            UpdateChild( fTimeElapsed );

            //更新特效自身
            UpdateSelf( fTimeElapsed );		
        }
    }
	*/

    if (m_bPrepared)
    {
        //m_fIterateRemainTime += fTimeElapsed;
        if (m_state == eES_Inactive)
        {
            return;
        }
        m_fCurrentTime += fTimeElapsed;
        UpdateSelf(fTimeElapsed);
        UpdateChild(fTimeElapsed);
    }
}

void GEffect::UpdateSelf( f32 fTimeLength )
{
	//如果还没激活
    /*
	if ( m_bIsActive == false )
	{
		if ( m_fCurrentLifeTime < m_Info.GetLifeTime() )
		{
			//取特效的激活时间点
			f32 fStartTime = m_Info.GetStartTime();
			if ( fStartTime > 0.0f )
			{
				f32 fCurStartTime = m_fCurrentStartTime + fTimeLength;
				//如果当前时间已经到达激活时间点,激活特效
				if ( fCurStartTime > m_fCurrentStartTime )
				{					
					m_bIsActive = true;
					StartRenderableObject();
				}
				m_fCurrentStartTime = fCurStartTime;
			}
			else
			{
				m_bIsActive = true;
				StartRenderableObject();
			}
		}
		else if( !m_bIsCompleted )
		{
			UpdateRenderObjectImpl( fTimeLength );

			if ( m_pNode )
			{
				m_pNode->NeedUpdate();
				m_pNode->Update();
			}
		}
	}
	else
	{
		f32 fLifeTime = m_Info.GetLifeTime();
		//如果特效自身时间长度>0
		if ( fLifeTime > 0.0f )
		{
			f32 currentTime = m_fCurrentLifeTime + fTimeLength;
			//如果当前时间长度超过特效的生命时间,关闭特效
			if ( currentTime > fLifeTime )
			{
				m_fCurrentLifeTime = currentTime;

				Stop();

				return;
			}
			//更新特效的渲染物以及节点
			else
			{
				m_fCurrentLifeTime = currentTime;

				UpdateRenderObjectImpl( fTimeLength );

				if ( m_pNode )
				{
					m_pNode->NeedUpdate();
					m_pNode->Update();
				}
			}
		}
	}
    */

    switch (m_state)
    {
    case eES_Inactive:
        if (m_fCurrentTime >= m_Info.GetStartTime())
        {
            SetCurrentState(eES_Active);
        }
        break;
    case eES_Active:
        if (m_Info.GetLifeTime() >= 0 && m_fCurrentTime >= m_Info.GetLifeTime())
        {
            SetCurrentState(eES_Stop);
        }
        break;
    case eES_Stop:
        //在Stop状态下的更新
        if (IsCompleted())
        {
            SetCurrentState(eES_Completed);
        }
        break;
    case eES_Completed:
        break;
    }
}

void GEffect::UpdateRenderObjectImpl( float fTimeLength )
{
	if ( m_pRenderObject )
	{
		m_pRenderObject->Update( fTimeLength );
	}
}

void GEffect::UpdateChild( f32 fTimeLength )
{
	if ( !m_vtChildGEffects.empty())
	{			
		VEC_GEFFECT::iterator i = m_vtChildGEffects.begin();

		while ( i != m_vtChildGEffects.end() )
		{
			GEffect *element = *i;

			element->Update( fTimeLength );				

            if (m_fCurrentTime >= element->GetStartTime() && m_fCurrentTime < element->GetEffectElementInfo().GetLifeTime() && (element->GetEffectState() == eES_Inactive ||
                element->GetEffectState() == eES_Completed))
            {
                element->SetCurrentState(eES_Active);
            }

			++i;
		}
	}
}

void GEffect::UpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue)
{
    /*
	int i = 0;
	int c = m_vtChildGEffects.size();
	GEffect* pChildEffect = NULL;
	for ( ; i < c; ++i )
	{
		pChildEffect = m_vtChildGEffects[i];
		pChildEffect->UpdateToRenderQueue( pCamera, pQueue );
	}

	if ( m_pRenderObject && (m_bIsActive || (m_bIsStoped && !m_bIsCompleted)) )
	{
		m_pRenderObject->UpdateToRenderQueue( pCamera, pQueue );
	}
    */
}

bool GEffect::LoadFromFile( const char* szFile )
{
	if (m_pResource == NULL)
	{
		m_pResource = RESOURCE_MANAGER->CreateResource<FileData>(szFile);			

		if (m_pResource && m_pResource->IsLoaded())
		{
			Prepare();
		}

        if (m_pResource)
        {
            m_strResourceName = CStringUtil::GetFilename(szFile);
        }
	}

	return m_pResource != NULL;
}

bool GEffect::SaveToFile( const AIRString& szFile )
{
	CMarkupSTL xml;

	xml.SetDoc("<?xml version=\"1.0\" encoding=\"gb2312\"?>\r\n");
	xml.AddElem("GEffect");
	char szTmp[256] = { 0 };
	sprintf(szTmp, "%.2f", m_Info.GetLifeTime() );
	xml.AddAttrib("LifeTime", szTmp);
	sprintf(szTmp, "%.2f", m_Info.GetStartTime() );
	xml.AddAttrib("StartTime", szTmp);
	sprintf(szTmp, "%.2f", m_Info.GetIterativeInterval() );
	xml.AddAttrib("IterativeInterval", szTmp );
	Vector3Df vPos = m_Info.GetTransform().m_Position;
	sprintf(szTmp, "%.2f,%.2f,%.2f", vPos.x, vPos.y, vPos.z);
	xml.AddAttrib("Position", szTmp);
	Quaternion qRotate = m_Info.GetTransform().m_Rotation;
	sprintf(szTmp, "%.2f,%.2f,%.2f,%.2f", qRotate.x, qRotate.y, qRotate.z, qRotate.w );
	xml.AddAttrib("Rotation", szTmp);

	xml.IntoElem();

	int i = 0;
	int c = m_vtChildGEffects.size();
	for ( ; i < c; ++i )
	{
		m_vtChildGEffects[i]->SaveChildToXML( xml );
	}

	xml.OutOfElem();

	return xml.Save(szFile.c_str());
}

void GEffect::SaveChildToXML( CMarkupSTL& xml )
{
	char szTmp[256] = { 0 };
	xml.AddElem( "GEffectElement" );
	sprintf(szTmp, "%.2f", m_Info.GetLifeTime() );
	xml.AddAttrib("LifeTime", szTmp);
	sprintf(szTmp, "%.2f", m_Info.GetStartTime() );
	xml.AddAttrib("StartTime", szTmp);
	sprintf(szTmp, "%.2f", m_Info.GetIterativeInterval() );
	xml.AddAttrib("IterativeInterval", szTmp );
	Vector3Df vPos = m_Info.GetTransform().m_Position;
	sprintf(szTmp, "%.2f,%.2f,%.2f", vPos.x, vPos.y, vPos.z);
	xml.AddAttrib("Position", szTmp);
	Quaternion qRotate = m_Info.GetTransform().m_Rotation;
	sprintf(szTmp, "%.2f,%.2f,%.2f,%.2f", qRotate.x, qRotate.y, qRotate.z, qRotate.w );
	xml.AddAttrib("Rotation", szTmp);
	xml.AddAttrib("ChildEffectType", this->GetTypeString().c_str() );
	xml.AddAttrib("RenderableFile", m_Info.GetRenderableFile().c_str() );//this->GetRenderable()->GetResourceName().c_str() );
}

bool GEffect::Prepare()
{
    if (m_pNode == 0 && !m_pParentGEffect)
    {
        //如果是父结点要判断m_pNode是否为空，否则下面创建子结点需要这个node
        return false;
    }
	if (!m_bPrepared)
	{
		bool bReady = false;
		if ( !m_pParentGEffect )
		{
			if (!m_bResourceReady && m_pResource && m_pResource->IsLoaded() )
			{
                if (!m_pResource->IsReady())
                {
                    m_pResource->Cache();
                }

				if (m_pResource->IsReady())
				{
                    CDataStream* pStream = m_pResource->GetFileStream();
                    if (pStream)
                    {
                        AIRString strDoc = pStream->GetAsString();

                        CMarkupSTL xml;
                        xml.SetDoc(strDoc.c_str());

                        if (!xml.FindElem())
                        {
                            return false;
                        }

                        AIRString szValue;
                        szValue = xml.GetAttrib( "Loop" ).c_str();
                        if ( szValue == "TRUE" )
                        {
                            SetLoop( true );
                        }
                        else if ( szValue == "FALSE" )
                        {
                            SetLoop( false );
                        }

                        szValue = xml.GetAttrib( "DelayTimeLen" ).c_str();
                        if ( szValue.length() )
                        {
                            SetDelayTimeLen( atof(szValue.c_str()) );
                        }

                        szValue = xml.GetAttrib( "LifeTime" ).c_str();
                        if ( szValue.length() )
                        {
                            GetEffectElementInfo().SetLifeTime( atof(szValue.c_str()) );
                        }		

                        szValue = xml.GetAttrib( "StartTime" ).c_str();
                        if ( szValue.length() )
                        {
                            GetEffectElementInfo().SetStartTime( atof(szValue.c_str()) );
                        }	

                        szValue = xml.GetAttrib( "IterativeInterval" ).c_str();
                        if ( szValue.length() )
                        {
                            GetEffectElementInfo().SetIterativeInterval( atof(szValue.c_str()) );
                        }

                        szValue = xml.GetAttrib( "Position" ).c_str();
                        if ( szValue.length() )
                        {
                            Vector3Df& vPos = GetEffectElementInfo().GetTransform().m_Position;
                            sscanf( szValue.c_str(), "(%f,%f,%f)", &vPos.x, &vPos.y, &vPos.z );
                        }

                        szValue = xml.GetAttrib( "Rotation" ).c_str();
                        if ( szValue.length() )
                        {
                            Quaternion& qRotate = GetEffectElementInfo().GetTransform().m_Rotation;
                            sscanf( szValue.c_str(), "(%f,%f,%f,%f)", &qRotate.x, &qRotate.y, &qRotate.z, &qRotate.w );
                        }

                        xml.IntoElem();
                        while(xml.FindElem())
                        {
                            if (xml.GetTagName() == "GEffectElement")
                            {
                                GEffectElementInfo info;

                                szValue = xml.GetAttrib( "LifeTime" ).c_str();
                                info.SetLifeTime( atof(szValue.c_str()) );

                                szValue = xml.GetAttrib( "StartTime" ).c_str();
                                info.SetStartTime( atof(szValue.c_str()) );

                                szValue = xml.GetAttrib( "IterativeInterval" ).c_str();
                                info.SetIterativeInterval( atof(szValue.c_str()) );

                                szValue = xml.GetAttrib( "Position" ).c_str();
                                Vector3Df& vChildPos = info.GetTransform().m_Position;
                                sscanf( szValue.c_str(), "(%f,%f,%f)", &vChildPos.x, &vChildPos.y, &vChildPos.z );

                                szValue = xml.GetAttrib( "Rotation" ).c_str();
                                Quaternion& qChildRotate = info.GetTransform().m_Rotation;
                                sscanf( szValue.c_str(), "(%f,%f,%f,%f)", &qChildRotate.x, &qChildRotate.y, &qChildRotate.z, &qChildRotate.w );

                                info.SetRenderableType( xml.GetAttrib( "ChildEffectType" ).c_str() );

                                info.SetRenderableFile( xml.GetAttrib( "RenderableFile" ).c_str() );

                                GEffect* pChildEffect = CreateChildGEffect( &info );
                                pChildEffect->LoadChildFromXML( xml );
                            }
                        }
                        xml.OutOfElem();
                    }
					m_bResourceReady = bReady = true;
				}
			}
			else if (m_pResource == NULL)
			{
				m_bResourceReady = bReady = true;
			}
		}
        else
        {
            //如果有父节点
            m_bResourceReady = bReady = true;
        }

		if (m_bResourceReady)
		{
			bReady = PrepareSelf();
		}

		if (m_bResourceReady && bReady)
		{
			bReady = PrepareChild();
		}

		m_bPrepared = bReady;
	}
	return m_bPrepared;
}

GEffect* GEffect::CreateChildGEffect( GEffectElementInfo* pInfo )
{
	GEffect* pChildEffect = GEFFECT_FACTORY->CreateGEffect( pInfo->GetRenderableType() );
	pChildEffect->SetParentGEffect( this );
	pChildEffect->GetEffectElementInfo() = *pInfo;
	Renderable* pRenderable = pChildEffect->CreateRenderable();
    pRenderable->SetVisable(false);

    if (m_pNode)
    {
        AIRString strName = GetString("GEffectChildNode_%p_%p", this, pChildEffect);
        CSceneNode* pNode = m_pNode->CreateChild( strName, true, pInfo->GetTransform().m_Position, pInfo->GetTransform().m_Rotation);
        //CSceneNode* pNode = new CSceneNode( ENGINE_INST->GetSceneMgr(), GetString("GEffectChildNode_%p_%p", this, pChildEffect), pInfo->GetTransform().m_Position, pInfo->GetTransform().m_Rotation );

        pNode->AttachRenderable( pRenderable );
        m_vtChildGEffects.push_back( pChildEffect );
        pChildEffect->SetLinkNode( pNode );
    }
	
	return pChildEffect;
}

void GEffect::SetAttachedNode(CSceneNode* pNode)
{
    bool bUpdateChildrenNode = pNode != m_pNode;
    Renderable::SetAttachedNode(pNode);

    if (bUpdateChildrenNode)
    {
        for (size_t i = 0; i < m_vtChildGEffects.size(); ++i)
        {
            //CSceneNode* pChildNode = m_pNode->CreateChild( GetString("GEffectChildNode_%p_%p", this, m_vtChildGEffects[i]), true, pInfo->GetTransform().m_Position, pInfo->GetTransform().m_Rotation);
            //m_vtChildGEffects[i]->GetRenderable()
        }
    }
    
}

bool GEffect::PrepareSelf()
{
	if ( m_pRenderObject )
	{
		return m_pRenderObject->Prepare();
	}
	return true;
}

bool GEffect::PrepareChild()
{
	bool bReady = true;
	int i = 0;
	int c = m_vtChildGEffects.size();
	for ( ; i < c; ++i )
	{
		if ( m_vtChildGEffects[i]->Prepare() == false )
		{
			bReady = false;
		}
	}
	return bReady;
}

void GEffect::PreRender(CCamera* pCamera)
{
	Renderable::PreRender( pCamera );
}

//bool GEffect::IsStoped( void )
//{
//	return m_bIsStoped;
//}

bool GEffect::IsCompleted( void )
{
	//return m_bIsCompleted;

    if (m_state == eES_Stop)
    {
        return IsChildEffectCompleted();
    }

    return m_state == eES_Completed;
}

bool GEffect::IsChildEffectCompleted()
{
    bool bIsCompleted = false;
    size_t nChildsNum = m_vtChildGEffects.size();
    for (size_t i = 0; i < nChildsNum; ++i)
    {
        bIsCompleted = m_vtChildGEffects[i]->IsCompleted();
        if (!bIsCompleted)
        {
            return false;
        }
    }
    return true;
}
/*
void GEffect::StartSelf()
{
	//m_fCurrentLifeTime = 0.0f;
	//m_fCurrentStartTime = 0.0f;	
	//m_fIterateRemainTime = 0.0f;
    SetCurrentState(eES_Active);
    m_fCurrentTime = 0;

	//子特效
	if ( m_pParentGEffect && m_pRenderObject )
	{
		//m_pParentGEffect->GetNodeParent()->AddChild( m_pLinkNode );
		//m_pLinkNode->AttachRenderable( m_pRenderObject );
        m_pRenderObject->SetVisable(true);
		//m_bIsActive = false;
		//m_bIsStoped = true;
		//m_bIsCompleted = true;
	}
	//父特效
	else
	{
		//m_bIsActive = true;
		//m_bIsStoped = false;
		//m_bIsCompleted = false;
	}
}

void GEffect::Start()
{
	//if ( !m_bPrepared )
	//{
	//	Prepare();
	//}   

    
	if ( !m_pNode && !m_pParentGEffect )
	{
		if ( !m_pLoopBackupNode )
		{
			DT_TO_MSGBOX( LT_ERROR, "You did not attach the effect to scenenode!" );
			return;
		}
		m_pLoopBackupNode->AttachRenderable( this );
		m_pNode = m_pLoopBackupNode;
	}

	int i = 0;
	int c = m_vtChildGEffects.size();
	GEffect* pChildEffect = NULL;
	Renderable* pChildRenderable = NULL;
	for ( ; i < c; ++i )
	{
		pChildEffect = m_vtChildGEffects[i];
		pChildEffect->Start();
	}

	StartSelf();
}

void GEffect::Stop()
{
	//m_bIsActive = false;
	//m_bIsStoped = true;

	//if ( m_pNode )
	{
		//关闭子特效
		int i = 0;
		int c = m_vtChildGEffects.size();
		GEffect* pChildEffect = NULL;
		Renderable* pChildRenderable = NULL;
		for ( ; i < c; ++i )
		{
			pChildEffect = m_vtChildGEffects[i];
			pChildEffect->Stop();
		}
	}
	//关闭特效自身
	StopSelf();
}
/*
void GEffect::StopSelf()
{
	//m_bIsActive = false;
	//m_bIsStoped = true;
	//m_bIsCompleted = false;
    SetCurrentState(eES_Stop);

	StopRenderableObject();
}
*/
//void GEffect::AddChildGEffect( GEffect* pChildEffect )
//{
//	Stop();
//	m_vtChildGEffects.push_back( pChildEffect );
//}

void GEffect::DelChildGEffect( u32 index )
{
	//Stop();
	if ( index < 0 || index >= m_vtChildGEffects.size() )
	{
		return;
	}
	VEC_GEFFECT::iterator itb = m_vtChildGEffects.begin();
	std::advance( itb, index );
	if ( itb != m_vtChildGEffects.end() )
	{
		(*itb)->Clear();
		SafeDelete( (*itb) );
		m_vtChildGEffects.erase( itb );
	}
}

void GEffect::SetInfoToNode()
{
	int i = 0;
	int c = m_vtChildGEffects.size();
	for ( ; i < c; ++i )
	{
		m_vtChildGEffects[i]->SetInfoToNode();
	}
	if ( m_pLinkNode )
	{
		m_pLinkNode->SetPosition( m_Info.GetTransform().m_Position );
		m_pLinkNode->SetRotation( m_Info.GetTransform().m_Rotation );
		m_pLinkNode->NeedUpdate();
		m_pLinkNode->Update();
	}
}

GEffect* GEffect::GetChildGEffect( u32 index )
{
	if ( index < 0 || index >= m_vtChildGEffects.size() )
	{
		return NULL;
	}
	return m_vtChildGEffects[ index ];
}

void GEffect::SetCurrentState(EffectState state)
{
    if (m_state != state)
    {
        OnStateChange(state);
    }
    m_state = state;
}

void GEffect::OnStateChange(EffectState state)
{
    switch (state)
    {
    case eES_Active:
        //m_fCurrentTime = 0;
        StartRenderableObject();
        break;
    case eES_Stop:
        StopRenderableObject();
        if (m_pParentGEffect == NULL)
        {
            size_t nChildrenNum = m_vtChildGEffects.size();
            for (size_t i = 0;i < nChildrenNum; ++i)
            {
                m_vtChildGEffects[i]->SetCurrentState(eES_Stop);
            }
        }
        break;
    case eES_Completed:
        if (m_pParentGEffect == NULL)
        {
            size_t nChildrenNum = m_vtChildGEffects.size();
            for (size_t i = 0;i < nChildrenNum; ++i)
            {
                m_vtChildGEffects[i]->SetCurrentState(eES_Inactive);
            }
        }
        if (m_bLoop && m_pParentGEffect == NULL)
        {
            SetCurrentState(eES_Active);
        }
        m_fCurrentTime = 0;
        break;
    case eES_Inactive:
        m_fCurrentTime = 0;
        break;
    }
}

//-----------------------------------------------

Renderable* ParticleSystemEffect::CreateRenderable()
{
    ISceneManager* pScene = ENGINE_INST->GetSceneMgr();

	CParticleSystem* pParticleSystem = pScene->CreateRenderable<CParticleSystem>(m_Info.GetRenderableFile().c_str()); //new CParticleSystem;
	if( pParticleSystem )
	{
		//pParticleSystem->LoadFromFile( m_Info.GetRenderableFile().c_str() );
		pParticleSystem->Pause();
	}
	m_pRenderObject = pParticleSystem;
	return m_pRenderObject;
}

void ParticleSystemEffect::StartRenderableObject()
{
    GEffect::StartRenderableObject();
	if ( m_pRenderObject )
	{
		CParticleSystem* pParticleSystem = static_cast<CParticleSystem*>(m_pRenderObject);
		pParticleSystem->SetPauseEmitParticle(false);
		pParticleSystem->Reset();
		pParticleSystem->Play();
	}
}

void ParticleSystemEffect::StopRenderableObject()
{
	if ( m_pRenderObject )
	{
		CParticleSystem* pParticleSystem = static_cast<CParticleSystem*>(m_pRenderObject);
		pParticleSystem->SetPauseEmitParticle(true);
	}
}

bool ParticleSystemEffect::IsRenderObjectCompleted()
{
	if ( m_pRenderObject )
	{
		bool isCompleted = (static_cast<CParticleSystem*>(m_pRenderObject))->IsCompleted();
		return isCompleted;
	}
	return true;
}

/*
bool ParticleSystemEffect::CompleteStopImpl( f32 fFrameTime )
{
	//如果已经停止了特效但还没完成停止操作
	if ( m_bIsStoped && m_bIsCompleted == false )
	{
		//检查渲染物是否完成停止操作
		if ( IsRenderObjectCompleted() )
		{
			m_bIsCompleted = true;
			m_pLinkNode->DetachAllRenderable();
			m_pParentGEffect->GetNodeParent()->RemoveChild( m_pLinkNode );
		}
		else
		{
			UpdateRenderObjectImpl( fFrameTime );
		}
	}
	return m_bIsCompleted;
}
*/

//-----------------------------------------------

Renderable* DecalEffect::CreateRenderable()
{
    ISceneManager* pScene = ENGINE_INST->GetSceneMgr();
	TerrainDecal* pDecal = pScene->CreateRenderable<TerrainDecal>(); //new TerrainDecal;
	//if( pDecal && m_Info.GetRenderableFile().length() )
	//{
	//	pDecal->LoadFromFile( m_Info.GetRenderableFile().c_str() );
	//}
	m_pRenderObject = pDecal;
	return m_pRenderObject;
}

/*
bool DecalEffect::CompleteStopImpl( f32 fFrameTime )
{
	//如果已经停止了特效但还没完成停止操作
	if ( m_bIsStoped && m_bIsCompleted == false )
	{
		//检查渲染物是否完成停止操作
		if ( IsRenderObjectCompleted() )
		{
			m_bIsCompleted = true;
			m_pLinkNode->DetachAllRenderable();
			m_pParentGEffect->GetNodeParent()->RemoveChild( m_pLinkNode );
		}
		else
		{
			UpdateRenderObjectImpl( fFrameTime );
		}
	}
	return m_bIsCompleted;
}
*/

void DecalEffect::UpdateRenderObjectImpl( f32 fFrameTime )
{
	TerrainDecal* pDecal = static_cast<TerrainDecal*>(m_pRenderObject);
	if ( pDecal )
	{
		pDecal->GetNodeParent()->NeedUpdate();
		pDecal->GetNodeParent()->Update();
		pDecal->ShowDecal( pDecal->GetNodeParent()->GetDerivedPosition() );
	}
}

void DecalEffect::SetDecalRadius( f32 fRadius ) 
{ 
	m_fDecalRadius = fRadius; 

	if ( m_pRenderObject )
	{
		TerrainDecal* pDecal = (TerrainDecal*)m_pRenderObject;
		f32 fSize = 2 * fRadius;
		pDecal->SetWidth( fSize );
		pDecal->SetHeight( fSize );
	}
}

void DecalEffect::LoadChildFromXML( CMarkupSTL& xml ) 
{
	GEffect::LoadChildFromXML( xml );

	TerrainDecal* pDecal = (TerrainDecal*)(m_pRenderObject);
	if ( pDecal )
	{
		AIRString szValue = xml.GetAttrib( "DecalRadius" ).c_str();
		if ( szValue.length() )
		{
			SetDecalRadius( atof(szValue.c_str()) );
		}
		szValue = xml.GetAttrib( "DecalImage" ).c_str();
		if ( szValue.length() )
		{
			pDecal->SetDecalTexture( szValue.c_str() );
		}
	}
}

void DecalEffect::SaveChildToXML( CMarkupSTL& xml )
{
	GEffect::SaveChildToXML( xml );

	char szTmp[256] = { 0 };
	sprintf(szTmp, "%.2f", m_fDecalRadius );
	xml.AddAttrib("DecalRadius", szTmp);
}

//-----------------------------------------------

Renderable* ModelEffect::CreateRenderable()
{
    ISceneManager* pScene = ENGINE_INST->GetSceneMgr();
	CModelInstance* pModel = pScene->CreateRenderable<CModelInstance>(m_Info.GetRenderableFile().c_str()); //new CModelInstance;
	if( pModel )
	{
		pModel->StopAnimation();
		pModel->SetVisable( false );
	}
	m_pRenderObject = pModel;
	return m_pRenderObject;
}

void ModelEffect::StartRenderableObject()
{
    GEffect::StartRenderableObject();
	CModelInstance* pModel = (CModelInstance*)m_pRenderObject;
	if ( pModel && pModel->GetModel() )
	{
		pModel->SetCurrentAnimation( m_szAnimName.c_str() );
		pModel->GetCurrentAnimation()->SetLoop( m_bLoop );
		pModel->SetVisable( true );
	}
}


void ModelEffect::UpdateRenderObjectImpl( f32 fFrameTime )
{
	if ( m_pRenderObject )
	{
		CModelInstance* pModel = (CModelInstance*)m_pRenderObject;
		if ( !pModel->GetCurrentAnimation() && m_szAnimName.length() )
		{
			pModel->SetCurrentAnimation( m_szAnimName.c_str() );
		}
		m_pRenderObject->Update( fFrameTime );
	}
}

void ModelEffect::LoadChildFromXML( CMarkupSTL& xml )
{
	GEffect::LoadChildFromXML( xml );

	AIRString szValue = xml.GetAttrib( "AnimName" ).c_str();
	SetAnimName( szValue );

	szValue = xml.GetAttrib( "Loop" ).c_str();
	if ( szValue == "TRUE" )
	{
		SetLoop( true );
	}
	else
	{
		SetLoop( false );
	}
}

void ModelEffect::SaveChildToXML( CMarkupSTL& xml )
{
	GEffect::SaveChildToXML( xml );

	xml.AddAttrib("AnimName", m_szAnimName.c_str());

	if ( m_bLoop )
	{
		xml.AddAttrib( "Loop", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "Loop", "FALSE" );
	}
}

//-----------------------------------------------

Renderable* RibbonTrailEffect::CreateRenderable()
{
    ISceneManager* pScene = ENGINE_INST->GetSceneMgr();
	RibbonTrail* pTrail = pScene->CreateRenderable<RibbonTrail>(); //new RibbonTrail();
	if( pTrail )
	{
		//if ( m_Info.GetRenderableFile().length() )
		//{
		//	pTrail->LoadFromFile( m_Info.GetRenderableFile().c_str() );
		//}		
		//else
		{
			// 读入拖尾的材质
			IMaterial* pMaterial = MATERIAL_MANAGER->CreateFromTemplate("RibbonTrail");
			pTrail->SetMaterial(pMaterial);
		}
		pTrail->SetVisable( false );
	}
	m_pRenderObject = pTrail;
	return m_pRenderObject;
}

void RibbonTrailEffect::StartRenderableObject()
{
    GEffect::StartRenderableObject();
	RibbonTrail* pTrail = (RibbonTrail*)m_pRenderObject;
	if ( pTrail )
	{
		pTrail->SetTrackNode( 0, m_pLinkNode );
		//pTrail->SetVisable( true );
	}
}
/*
bool RibbonTrailEffect::CompleteStopImpl( f32 fFrameTime )
{
	//如果已经停止了特效但还没完成停止操作
	if ( m_bIsStoped && m_bIsCompleted == false )
	{
		//检查渲染物是否完成停止操作
		if ( IsRenderObjectCompleted() )
		{
			m_bIsCompleted = true;
			m_pLinkNode->DetachAllRenderable();
			m_pParentGEffect->GetNodeParent()->RemoveChild( m_pLinkNode );
		}
		else
		{
			UpdateRenderObjectImpl( fFrameTime );
		}
	}
	return m_bIsCompleted;
}
*/
void RibbonTrailEffect::UpdateRenderObjectImpl( f32 fFrameTime )
{
	RibbonTrail* pTrail = static_cast<RibbonTrail*>(m_pRenderObject);
	if ( pTrail )
	{
		pTrail->GetNodeParent()->NeedUpdate();
		pTrail->GetNodeParent()->Update();

		pTrail->Update( fFrameTime );
	}
}

void RibbonTrailEffect::LoadChildFromXML( CMarkupSTL& xml )
{
	GEffect::LoadChildFromXML( xml );
	RibbonTrail* pTrail = static_cast<RibbonTrail*>(m_pRenderObject);
	if ( !pTrail )
	{
		return;
	}

	AIRString szValue = xml.GetAttrib( "ChainCount" ).c_str();
	s32 nv = 0;
	if ( szValue.length() )
	{	
		nv = atoi( szValue.c_str() );
		pTrail->SetNumberOfChains( nv );
	}

	szValue = xml.GetAttrib( "MaxElemPerChain" ).c_str();
	if ( szValue.length() )
	{
		nv = atoi( szValue.c_str() );
		pTrail->SetMaxChainElements( nv );
	}
	
	szValue = xml.GetAttrib( "UseVertexColor" ).c_str();
	if ( szValue.length() )
	{
		if ( szValue == "TRUE" )
		{
			pTrail->SetUseVertexColours( true );
		}
		else if( szValue == "FALSE" )
		{
			pTrail->SetUseVertexColours( false );
		}
	}

	szValue = xml.GetAttrib( "UseTexCoord" ).c_str();
	if ( szValue.length() )
	{
		if ( szValue == "TRUE" )
		{
			pTrail->SetUseTextureCoords( true );
		}
		else if( szValue == "FALSE" )
		{
			pTrail->SetUseTextureCoords( false );
		}
	}

	szValue = xml.GetAttrib( "DrawWithBillBoard" ).c_str();
	if ( szValue.length() )
	{
		if ( szValue == "TRUE" )
		{
			pTrail->SetDrawWithBillboard( true );
		}
		else if( szValue == "FALSE" )
		{
			pTrail->SetDrawWithBillboard( false );
		}
	}

	szValue = xml.GetAttrib( "TexCoordRange" ).c_str();
	if ( szValue.length() )
	{
		Vector2Df v;
		sscanf( szValue.c_str(), "(%f,%f)", &v.x, &v.y );
		pTrail->SetOtherTextureCoordRange( v.x, v.y );
	}

	szValue = xml.GetAttrib( "TexCoordDirection" ).c_str();
	if ( szValue.length() )
	{
		nv = atoi( szValue.c_str() );
		pTrail->SetTextureCoordDirection( BillBoardChain::TexCoordDirection(nv) );
	}

	szValue = xml.GetAttrib( "TrailTexImage" ).c_str();
	if ( szValue.length() )
	{
		pTrail->SetTrailImage( szValue );
	}

	xml.IntoElem();
	if( xml.FindElem("ChainInfo") )
	{
		szValue = xml.GetAttrib( "InitColor" ).c_str();
		CColorValue v;
		sscanf( szValue.c_str(), "(%f,%f,%f,%f)", &v.r, &v.g, &v.b, &v.a );
		pTrail->SetInitialColour( 0, v );

		szValue = xml.GetAttrib( "InitWidth" ).c_str();
		float f = atof( szValue.c_str() );
		pTrail->SetInitialWidth( 0, f );

		szValue = xml.GetAttrib( "ChangeColor" ).c_str();
		sscanf( szValue.c_str(), "(%f,%f,%f,%f)", &v.r, &v.g, &v.b, &v.a );
		pTrail->SetColourChange( 0, v );

		szValue = xml.GetAttrib( "ChangeWidth" ).c_str();
		f = atof( szValue.c_str() );
		pTrail->SetWidthChange( 0, f );
	}
	xml.OutOfElem();

}

void RibbonTrailEffect::SaveChildToXML( CMarkupSTL& xml )
{
	GEffect::SaveChildToXML( xml );
	RibbonTrail* pTrail = static_cast<RibbonTrail*>(m_pRenderObject);
	if ( !pTrail )
	{
		return;
	}

	xml.SetAttrib( "ChainCount", s32(pTrail->GetNumberOfChains()) );

	xml.SetAttrib( "MaxElemPerChain", s32(pTrail->GetMaxChainElements()) );

	if ( pTrail->GetUseVertexColours() )
	{
		xml.SetAttrib( "UseVertexColor", "TRUE" );
	}
	else
	{
		xml.SetAttrib( "UseVertexColor", "FALSE" );
	}

	if ( pTrail->GetUseTextureCoords() )
	{
		xml.SetAttrib( "UseTexCoord", "TRUE" );
	}
	else
	{
		xml.SetAttrib( "UseTexCoord", "FALSE" );
	}

	if ( pTrail->IsDrawWithBillboard() )
	{
		xml.SetAttrib( "DrawWithBillBoard", "TRUE" );
	}
	else
	{
		xml.SetAttrib( "DrawWithBillBoard", "FALSE" );
	}

	const float* fptr = pTrail->GetOtherTextureCoordRange();
	char buf[256];
	snprintf( buf, 256, "(%.4f,%.4f)", *fptr, *(++fptr) );
	xml.SetAttrib( "TexCoordRange", buf );

	xml.SetAttrib( "TexCoordDirection", s32(pTrail->GetTextureCoordDirection()) );

	xml.SetAttrib( "TrailTexImage", pTrail->GetTrailImage().c_str() );

	xml.IntoElem();
	if( xml.AddElem("ChainInfo") )
	{
		SColor sc = pTrail->GetInitialColour( 0 );
		CColorValue c;
		c.SetAsRGBA( sc.color );
		snprintf( buf, 255, "(%.4f,%.4f,%.4f,%.4f)", c.r, c.g, c.b, c.a );
		xml.SetAttrib( "InitColor", buf );

		snprintf( buf, 255, "%f", pTrail->GetInitialWidth( 0 ) );
		xml.SetAttrib( "InitWidth", buf );

		c = pTrail->GetColourChange( 0 );
		snprintf( buf, 255, "(%.4f,%.4f,%.4f,%.4f)", c.r, c.g, c.b, c.a );
		xml.SetAttrib( "ChangeColor", buf );

		snprintf( buf, 255, "%f", pTrail->GetWidthChange( 0 ) );
		xml.SetAttrib( "ChangeWidth", buf );
	}
	xml.OutOfElem();
}

//-----------------------------------------------

Renderable* BillboardEffect::CreateRenderable()
{
    ISceneManager* pScene = ENGINE_INST->GetSceneMgr();
	CBillboard* pBB = pScene->CreateRenderable<CBillboard>(); //new CBillboard;
	if( pBB )
	{
		pBB->SetName( "MoonObject" );
		pBB->SetComColor( SColor::White );
		pBB->SetUseComColor( true );
		pBB->SetBillBoardCreateType( eBBCT_Center );
		pBB->SetBillBoardFaceType( eBBFT_AlwaysFaceCamera );
		pBB->SetBBSize( 50, 50 );
		pBB->SetUseRenderer( true );
		
		//IMaterial* pMat = MATERIAL_MANAGER->CreateMaterial( "TemplateMaterial.xml/Particle" );
		//pBB->SetMaterial( pMat );
		//if ( !pMat->IsReady() )
		//{
		//	pMat->Cache();	
		//}
	}
	m_pRenderObject = pBB;
	return m_pRenderObject;
}
/*
bool BillboardEffect::CompleteStopImpl( f32 fFrameTime )
{
	//如果已经停止了特效但还没完成停止操作
	if ( m_bIsStoped && m_bIsCompleted == false )
	{
		//检查渲染物是否完成停止操作
		if ( IsRenderObjectCompleted() )
		{
			m_bIsCompleted = true;
			m_pLinkNode->DetachAllRenderable();
			m_pParentGEffect->GetNodeParent()->RemoveChild( m_pLinkNode );
		}
		else
		{
			UpdateRenderObjectImpl( fFrameTime );
		}
	}
	return m_bIsCompleted;
}

void BillboardEffect::StartRenderableObject()
{
    GEffect::StartRenderableObject();
	CBillboard* pBB = (CBillboard*)m_pRenderObject;
	if ( pBB && m_pLinkNode )
	{
		m_pLinkNode->AttachRenderable( pBB );
		pBB->SetVisable( true );
	}
}
*/
void BillboardEffect::SetBillboardSize( f32 fSize )
{
	CBillboard* pBB = (CBillboard*)m_pRenderObject;
	if ( pBB )
	{
		m_fBillboardSize = fSize;
		pBB->SetBBSize( m_fBillboardSize, m_fBillboardSize );
	}
}

void BillboardEffect::SetBillboardTexture( const char* szTexFile )
{
	CBillboard* pBB = (CBillboard*)m_pRenderObject;
	if ( pBB )
	{
		if ( szTexFile )
		{
			ITexture* pTex = TEXTURE_MANAGER->CreateTextureFromFile(szTexFile);
			if ( pTex )
			{
				pBB->SetTexture( pTex );
			}
		}		
	}
}

const AIRString& BillboardEffect::GetBillboardTexture() const
{
	CBillboard* pBB = (CBillboard*)m_pRenderObject;
	if ( pBB )
	{
		if ( pBB->GetTexture() )
		{
			return pBB->GetTexture()->GetFileName();
		}
	}
    return CStringUtil::BLANK;
}

void BillboardEffect::LoadChildFromXML( CMarkupSTL& xml )
{
	GEffect::LoadChildFromXML( xml );
	CBillboard* pBB = static_cast<CBillboard*>(m_pRenderObject);
	if ( !pBB )
	{
		return;
	}

	AIRString szValue = xml.GetAttrib( "Size" ).c_str();
	if ( szValue.length() )
	{
		SetBillboardSize( atof(szValue.c_str()) );
	}

	szValue = xml.GetAttrib( "Image" ).c_str();
	if ( szValue.length() )
	{
		SetBillboardTexture( szValue.c_str() );
	}
}

void BillboardEffect::SaveChildToXML( CMarkupSTL& xml )
{
	GEffect::SaveChildToXML( xml );
	CBillboard* pBB = static_cast<CBillboard*>(m_pRenderObject);
	if ( !pBB )
	{
		return;
	}

	xml.SetAttrib( "Size", GetString("%f",GetBillboardSize()) );

	ITexture* pTex = pBB->GetTexture();
	if ( pTex )
	{
		xml.SetAttrib( "Image", pTex->GetFileName().c_str() );
	}
}

//-----------------------------------------------

GEffectFactory::GEffectFactory()
{
}

GEffect* GEffectFactory::CreateGEffect( const AIRString& strType, const AIRString& szFile )
{
	GEffect* pEffect = CreateGEffect( strType );

	if (!pEffect)
	{
		return NULL;
	}

	if ( !pEffect->LoadFromFile( szFile.c_str() ) )
	{
		pEffect->Clear();
		SafeDelete(pEffect);
	}

	pEffect->GetEffectElementInfo().SetRenderableFile( szFile.c_str() );

	return pEffect;
}

GEffect* GEffectFactory::CreateGEffect(const AIRString& strType)
{
	GEffect* pEffect = NULL;
	if ( strType == ParticleSystemEffect::C_CHILD_GEFFECT_PARTICLE )
	{
		pEffect = new ParticleSystemEffect;
	}
	else if ( strType == DecalEffect::C_CHILD_GEFFECT_DECAL )
	{
		pEffect = new DecalEffect;
	}
	else if ( strType == ModelEffect::C_CHILD_GEFFECT_MODEL )
	{
		pEffect = new ModelEffect;
	}
	else if ( strType == RibbonTrailEffect::C_CHILD_GEFFECT_RIBBON_TRAIL )
	{
		pEffect = new RibbonTrailEffect;
	}
	else if ( strType == BillboardEffect::C_CHILD_GEFFECT_BILLBOARD )
	{
		pEffect = new BillboardEffect;
	}
	return pEffect;
}



//-----------------------------------------------