#include "stdafx.h"
#include "ParticleSystemResource.h"
#include "BillboardSet.h"
#include "MaterialManager.h"
#include "DataStream.h"
#include "StringUtil.h"
#include "ParticleAffectorFactory.h"
#include "ParticleEmitterFactory.h"
#include "BillboardSetManager.h"
//#include "TextureUnit.h"
//#include "Technique.h"
#include "TextureMgr.h"

ParticleSystemResource::ParticleSystemResource() : //m_pMaterial(NULL), //m_pBillboardSet(NULL),
m_nPoolSize(0), 
m_nEmittedEmitterPoolSize(0),
m_fDefaultWidth(0),
m_fDefaultHeight(0), 
m_billboardType(BBT_POINT), 
m_bCullBackFace(true),
m_vCommonDir(Vector3Df::UNIT_Y),
m_vCommonUpDir(Vector3Df::UNIT_Z),
m_bCreateTrail(false),
m_nTrailSegmentCount(0),
m_nMaxChainElement(20),
m_fInitTrailWidth(10.0f),
m_fTrailWidthChange(-1.0f),
m_bTrailUseVertexColor(true),
m_bTrailUseTexCoord(true),
m_szTrailTex(""),
m_TCD(BillBoardChain::TCD_U)
, m_fEmitEndTime(0)
, m_szBlendMode("BLENDOP_ADD")
, m_szSrcBlend("BLEND_SRCALPHA")
, m_szDestBlend("BLEND_INVSRCALPHA")
, m_ParticleSystemType(ePST_QUAD)
, m_bHasTrial(false)
, m_bLocalSpace(false)
, m_pParticleTexture(NULL)
{

}

ParticleSystemResource::~ParticleSystemResource()
{

}
/*
s32 ParticleSystemResource::Release()
{
    if (DropRef() == 0)
    {
        Clear();
    }

    return GetRef();
}
*/

void ParticleSystemResource::Clear()
{
    //销毁粒子材质
    //if (m_pMaterial)
    //{
    //    MATERIAL_MANAGER->ClearMaterial(m_pMaterial);
    //}

    //销毁粒子系统的顶点,索引缓冲
    //if (m_pBillboardSet)
    //{
    //    BILLBOARDSET_MGR->ClearBillboardSet(m_pBillboardSet);
    //    m_pBillboardSet = NULL;
    //}
    if (m_pParticleTexture)
    {
        TEXTURE_MANAGER->ClearTexture(m_pParticleTexture);
    }

    if (m_pStream)
    {
        SafeDelete(m_pStream);
    }
}

bool ParticleSystemResource::Cache()
{
    if (m_pStream/* && !m_pMaterial*/)
    {
        AIRString strDoc = m_pStream->GetAsString();

        CMarkupSTL xml;
        xml.SetDoc(strDoc.c_str());

        if (!xml.FindElem())
        {
            //SafeDelete(pStream);
            return false;
        }

        IRenderer* pRenderer = ENGINE_INST->GetRenderer();
        /*
        m_pMaterial = MATERIAL_MANAGER->CreateMaterial("TemplateMaterial.xml/Particle");

        if (m_pMaterial == NULL)
        {
            //SafeDelete(pStream);
            return false;
        }
        else
        {
            if (!m_pMaterial->IsReady())
            {
                m_pMaterial->Cache();
            }
        }
        */

        //设置是否对粒子进行背面剔除
        AIRString strValue = xml.GetAttrib( "cull_back_face" ).c_str();
        if ( !strValue.empty() )
        {
            CStringUtil::ToUpperCase( strValue );
            if ( strValue == "TRUE" )
            {
                m_bCullBackFace = true;
            }
            else
            {
                m_bCullBackFace = false;
            }
        }

        strValue = xml.GetAttrib( "local_space" ).c_str();
        if ( !strValue.empty() )
        {
            CStringUtil::ToUpperCase( strValue );
            if ( strValue == "TRUE" )
            {
                m_bLocalSpace = true;
            }
            else
            {
                m_bLocalSpace = false;
            }
        }
        //SetCullBackFace( m_bCullBackFace );

        //粒子图像
        m_strParticleTexture = xml.GetAttrib("image").c_str();
        m_pParticleTexture = TEXTURE_MANAGER->CreateTextureFromFile(m_strParticleTexture.c_str());
        //if (m_pMaterial->IsReady())
        //    m_pMaterial->SetTextureUnitTex(0, m_strParticleTexture.c_str());

        //粒子宽度
        strValue = xml.GetAttrib("particle_width").c_str();
        m_fDefaultWidth = atof(strValue.c_str());

        //粒子高度
        strValue = xml.GetAttrib("particle_height").c_str();
        m_fDefaultHeight = atof(strValue.c_str());

        //粒子系统所能容纳的最大粒子数
        strValue = xml.GetAttrib("quota").c_str();
        int nQuota = atoi(strValue.c_str());
        m_nPoolSize = nQuota;

        //被发射的发射粒子数量
        strValue = xml.GetAttrib("emitted_emitter_quota").c_str();
        m_nEmittedEmitterPoolSize = atoi(strValue.c_str());

        //公告板类型
        strValue = xml.GetAttrib("billboard_type").c_str();
        //始终面向摄像机
        if (strValue == "BBT_POINT")
        {
            SetBillBoardType(BBT_POINT);
        }
        //平行于公共方向(m_vCommonDir)并朝向摄像机
        else if (strValue == "BBT_HORZ_COMMON_DIR_FACE_CAM")
        {
            SetBillBoardType(BBT_HORZ_COMMON_DIR_FACE_CAM);
        }
        //平行于公共方向(m_vCommonDir)并朝向公共上方向(m_vCommonUpDir)
        else if (strValue == "BBT_HORZ_COMMON_DIR_FACE_COMMON_UP_DIR")
        {
            SetBillBoardType(BBT_HORZ_COMMON_DIR_FACE_COMMON_UP_DIR);
        }
        //垂直于公共方向(m_vCommonDir)并朝向摄像机
        else if (strValue == "BBT_VERT_COMMON_DIR_FACE_CAM")
        {
            SetBillBoardType(BBT_VERT_COMMON_DIR_FACE_CAM);
        }
        //垂直于公共方向(m_vCommonDir)并朝向公共上方向(m_vCommonUpDir)
        else if (strValue == "BBT_VERT_COMMON_DIR_FACE_COMMON_UP_DIR")
        {
            SetBillBoardType(BBT_VERT_COMMON_DIR_FACE_COMMON_UP_DIR);
        }
        //平行于自己的运动方向(pParticle->m_direction)并朝向摄像机
        else if (strValue == "BBT_HORZ_SELF_DIR_FACE_CAM")
        {
            SetBillBoardType(BBT_HORZ_SELF_DIR_FACE_CAM);
        }
        //平行于自己的运动方向(pParticle->m_direction)并朝向公共上方向(m_vCommonUpDir)
        else if (strValue == "BBT_HORZ_SELF_DIR_FACE_COMMON_UP_DIR")
        {
            SetBillBoardType(BBT_HORZ_SELF_DIR_FACE_COMMON_UP_DIR);
        }
        //垂直于自己的运动方向(pParticle->m_direction)并朝向摄像机
        else if (strValue == "BBT_VERT_SELF_DIR_FACE_CAM")
        {
            SetBillBoardType(BBT_VERT_SELF_DIR_FACE_CAM);
        }
        //垂直于自己的运动方向(pParticle->m_direction)并朝向公共上方向(m_vCommonUpDir)
        else if (strValue == "BBT_VERT_SELF_DIR_FACE_COMMON_UP_DIR")
        {
            SetBillBoardType(BBT_VERT_SELF_DIR_FACE_COMMON_UP_DIR);
        }

        //粒子公共方向
        strValue = xml.GetAttrib( "common_dir" ).c_str();
        if ( !strValue.empty() )
        {
            sscanf( strValue.c_str(), "(%f,%f,%f)", &m_vCommonDir.x, &m_vCommonDir.y, &m_vCommonDir.z );
        }

        //粒子公共上方向向量
        strValue = xml.GetAttrib( "common_up_dir" ).c_str();
        if ( !strValue.empty())
        {
            sscanf( strValue.c_str(), "(%f,%f,%f)", &m_vCommonUpDir.x, &m_vCommonUpDir.y, &m_vCommonUpDir.z );
        }

        //是否创建尾迹
        strValue = xml.GetAttrib( "create_trail" ).c_str();
        if ( !strValue.empty() )
        {
            CStringUtil::ToUpperCase( strValue );
            if ( strValue == "TRUE" )
            {
                m_bCreateTrail = true;
            }
            else
            {
                m_bCreateTrail = false;
            }
        }

        // 尾迹段数
        strValue = xml.GetAttrib( "trail_segment_count" ).c_str();
        if ( !strValue.empty() )
        {
            sscanf( strValue.c_str(), "%d", &m_nTrailSegmentCount );
        }
        if ( m_nTrailSegmentCount > 0 )
        {
            m_bCreateTrail = false;
            m_bHasTrial = true;
        }

        // 每段尾迹的最大节点数
        strValue = xml.GetAttrib( "max_chain_elem" ).c_str();
        if ( !strValue.empty() )
        {
            sscanf( strValue.c_str(), "%d", &m_nMaxChainElement );
        }

        // 尾迹的初始宽度
        strValue = xml.GetAttrib( "init_trail_width" ).c_str();
        if ( !strValue.empty() )
        {
            sscanf( strValue.c_str(), "%f", &m_fInitTrailWidth );
        }

        // 尾迹宽度每秒的变化值
        strValue = xml.GetAttrib( "trail_width_change" ).c_str();
        if ( !strValue.empty() )
        {
            sscanf( strValue.c_str(), "%f", &m_fTrailWidthChange );
        }

        // 尾迹是否使用顶点颜色
        strValue = xml.GetAttrib( "trail_use_vertex_color" ).c_str();
        if ( !strValue.empty() )
        {
            if ( strValue == "TRUE" )
            {
                m_bTrailUseVertexColor = true;
            }
            else
            {
                m_bTrailUseVertexColor = false;
            }
        }

        // 尾迹是否使用纹理颜色
        strValue = xml.GetAttrib( "trail_use_tex_coord" ).c_str();
        if ( !strValue.empty() )
        {
            if ( strValue == "TRUE" )
            {
                m_bTrailUseTexCoord = true;
            }
            else
            {
                m_bTrailUseTexCoord = false;
            }
        }

        // 尾迹纹理使用的纹理坐标方向
        strValue = xml.GetAttrib( "trail_tex_dir" ).c_str();
        if ( !strValue.empty() )
        {
            if ( strValue == "u" )
            {
				m_TCD = BillBoardChain::TCD_U;
            }
            else
            {
				m_TCD = BillBoardChain::TCD_V;
            }
        }

        // 尾迹使用的纹理
        m_szTrailTex = xml.GetAttrib( "trail_image" ).c_str();

        // 源混合因子
        strValue = xml.GetAttrib( "src_blend" ).c_str();
        if ( !strValue.empty() )
        {
            m_szSrcBlend = strValue;
            //m_pMaterial->GetCurrentTechnique()->GetPass(0)->GetShader()->SetBlendSrc( pRenderer->GetRenderParm(m_szSrcBlend) );
        }

        // 目标混合因子
        strValue = xml.GetAttrib( "dest_blend" ).c_str();
        if ( !strValue.empty() )
        {
            m_szDestBlend = strValue;
            //m_pMaterial->GetCurrentTechnique()->GetPass(0)->GetShader()->SetBlendDest( pRenderer->GetRenderParm(m_szDestBlend) );
        }

        // 混合模式
        strValue = xml.GetAttrib( "blend_mode" ).c_str();
        if ( !strValue.empty() )
        {
            m_szBlendMode = strValue;
            //m_pMaterial->GetCurrentTechnique()->GetPass(0)->GetShader()->SetBlendOperation( pRenderer->GetRenderParm(m_szBlendMode) );
        }

        strValue = xml.GetAttrib("emit_end_time").c_str();
        if (!strValue.empty())
        {
            m_fEmitEndTime = atof(strValue.c_str());
        }

        if ( !xml.GetAttrib( "RenderType" ).empty() )
        {
            m_ParticleSystemType = ParticleSystemType( atoi( xml.GetAttrib( "RenderType" ).c_str() ) );
        }
        m_szParticleRenderableResource = xml.GetAttrib("RenderObject").c_str();
        if ( !m_szParticleRenderableResource.empty() && m_ParticleSystemType == ePST_OBJ )
        {
            //SetParticleSystemRenderableObjectResource( m_szParticleRenderableResource );
        }
    }
    
    /*
    if (m_pMaterial && m_pMaterial->IsReady())
    {
        m_pMaterial->GetCurrentTechnique()->GetPass(0)->SetBlendSrc( (RenderStateBlendValue)EngineParamSetting::GetEngineRenderParam(m_szSrcBlend) );
        m_pMaterial->GetCurrentTechnique()->GetPass(0)->SetBlendDest( (RenderStateBlendValue)EngineParamSetting::GetEngineRenderParam(m_szDestBlend) );
        m_pMaterial->GetCurrentTechnique()->GetPass(0)->SetBlendOpt( (BlendOpt)EngineParamSetting::GetEngineRenderParam(m_szBlendMode));
        SetCullBackFace( m_bCullBackFace );
        m_pMaterial->SetTextureUnitTex(0, m_strParticleTexture.c_str());
        m_bReady = true;
    }
    else
    {
        m_bReady = m_pMaterial->Cache();

        if (m_bReady)
        {
            m_pMaterial->GetCurrentTechnique()->SetBlendSrc( (RenderStateBlendValue)EngineParamSetting::GetEngineRenderParam(m_szSrcBlend)  );
            m_pMaterial->GetCurrentTechnique()->SetBlendDest( (RenderStateBlendValue)EngineParamSetting::GetEngineRenderParam(m_szDestBlend) );
            m_pMaterial->GetCurrentTechnique()->SetBlendOpt( (BlendOpt)EngineParamSetting::GetEngineRenderParam(m_szBlendMode));
            SetCullBackFace( m_bCullBackFace );
            m_pMaterial->SetTextureUnitTex(0, m_strParticleTexture.c_str());
        }
    }    
    */
    m_bReady = true;
    
    return m_bReady;
}

void ParticleSystemResource::SetBillBoardType(BillboardType type)
{
    m_billboardType = type;
}

/*
bool ParticleSystemResource::CreateBuffers()
{
    DestoryBuffers();

    if (m_pBillboardSet == NULL)
    {
        m_pBillboardSet = BILLBOARDSET_MGR->CreateBillboardSet((m_nPoolSize + m_nEmittedEmitterPoolSize) * 4 + m_nEmittedEmitterPoolSize * 4);
    }

    return true;
}

void ParticleSystemResource::DestoryBuffers()
{
    if (m_pBillboardSet)
    {
        BILLBOARDSET_MGR->ClearBillboardSet(m_pBillboardSet);
        m_pBillboardSet = NULL;
    }
}
*/
/*
bool ParticleSystemResource::SaveToFile(const char* szFilename)
{
    CMarkupSTL xml;

    xml.SetDoc("<?xml version=\"1.0\" encoding=\"gb2312\"?>\r\n");
    xml.AddElem("ParticleSystem");
    AIRString strImageFile;
    if (m_pMaterial)
    {
        CTextureUnit* pTexUnit = m_pMaterial->GetTechnique(0)->GetPass(0)->GetTextureUnit(0);
        ITexture* pTexture = pTexUnit->GetTexture();
        if (pTexture)
        {
            strImageFile = pTexture->GetName();
        }
    }
    xml.AddAttrib("image", strImageFile.c_str());
    char szTmp[256] = { 0 };
    sprintf(szTmp, "%.2f", m_fDefaultWidth);
    xml.AddAttrib("particle_width", szTmp);
    sprintf(szTmp, "%.2f", m_fDefaultHeight);
    xml.AddAttrib("particle_height", szTmp);
    xml.AddAttrib("quota", m_nPoolSize);
    xml.AddAttrib("billboard_type", GetBillBoardTypeStr().c_str());
    xml.AddAttrib("boundingbox_autoupdate", m_bBoundsAutoUpdate ? "TRUE" : "FALSE");
    sprintf(szTmp, "%.2f, %.2f, %.2f", m_boundingBox.MaxEdge.x, m_boundingBox.MaxEdge.y, m_boundingBox.MaxEdge.z);
    xml.AddAttrib("boundingbox_max", szTmp);

    sprintf(szTmp, "%.2f, %.2f, %.2f", m_boundingBox.MinEdge.x, m_boundingBox.MinEdge.y, m_boundingBox.MinEdge.z);
    xml.AddAttrib("boundingbox_min", szTmp);

    sprintf(szTmp, "(%.2f, %.2f, %.2f)", m_vCommonDir.x, m_vCommonDir.y, m_vCommonDir.z );
    xml.AddAttrib("common_dir", szTmp);

    sprintf(szTmp, "(%.2f, %.2f, %.2f)", m_vCommonUpDir.x, m_vCommonUpDir.y, m_vCommonUpDir.z );
    xml.AddAttrib("common_up_dir", szTmp);

    if ( m_bCullBackFace )
    {
        xml.AddAttrib( "cull_back_face", "true" );
    }
    else
    {
        xml.AddAttrib( "cull_back_face", "false" );
    }

    sprintf(szTmp, "%d", m_nEmittedEmitterPoolSize);
    xml.AddAttrib("emitted_emitter_quota", szTmp);


    if ( m_bCreateTrail )
    {
        xml.AddAttrib( "create_trail", "TRUE" );
    }
    else
    {
        xml.AddAttrib( "create_trail", "FALSE" );
    }

    sprintf(szTmp, "%d", m_nTrailSegmentCount );
    xml.AddAttrib( "trail_segment_count", szTmp );

    sprintf(szTmp, "%d", m_nMaxChainElement );
    xml.AddAttrib( "max_chain_elem", szTmp );

    sprintf(szTmp, "%.2f", m_fInitTrailWidth );
    xml.AddAttrib( "init_trail_width", szTmp );

    sprintf(szTmp, "%.2f", m_fTrailWidthChange );
    xml.AddAttrib( "trail_width_change", szTmp );

    sprintf(szTmp, "%.4f", m_fEmitEndTime);
    xml.AddAttrib("emit_end_time", m_fEmitEndTime);

    if ( m_bTrailUseVertexColor )
    {
        xml.AddAttrib( "trail_use_vertex_color", "TRUE" );
    }
    else
    {
        xml.AddAttrib( "trail_use_vertex_color", "FALSE" );
    }

    if ( m_bTrailUseTexCoord )
    {
        xml.AddAttrib( "trail_use_tex_coord", "TRUE" );
    }
    else
    {
        xml.AddAttrib( "trail_use_tex_coord", "FALSE" );
    }

    if ( m_TCD == BillBoardChain::TexCoordDirection::TCD_U )
    {
        xml.AddAttrib( "trail_tex_dir", "u" );
    }
    else
    {
        xml.AddAttrib( "trail_tex_dir", "v" );
    }

    if ( m_szTrailTex.length() > 0 )
    {
        xml.AddAttrib( "trail_image", m_szTrailTex.c_str() );
    }

    xml.AddAttrib( "src_blend", m_szSrcBlend.c_str() );
    xml.AddAttrib( "dest_blend", m_szDestBlend.c_str() );
    xml.AddAttrib( "blend_mode", m_szBlendMode.c_str() );

    xml.AddAttrib( "RenderType", int(m_ParticleSystemType) );
    xml.AddAttrib( "RenderObject", m_szParticleRenderableResource.c_str() );

    //进入ParticleSystem结点
    xml.IntoElem();
    xml.AddNode(CMarkupSTL::MNT_TEXT, "\r\n    ");

    
    //保存发射器信息
    for (VECTOR_EMITTER_ITER it = m_vtParticleEmitter.begin(); it != m_vtParticleEmitter.end(); ++it)
    {
        xml.AddElem("Emitter");
        CParticleEmitter* pEmitter = *it;
        pEmitter->SaveToXMLNode(xml);
    }

    //保存影响器
    for (VECTOR_AFFECTOR_ITER it = m_vtParticleAffector.begin(); it != m_vtParticleAffector.end(); ++it)
    {
        xml.AddElem("Affector");
        CParticleAffector* pAffector = *it;
        pAffector->SaveToXMLNode(xml);
    }

    xml.OutOfElem();
    
    //-----
    return xml.Save(szFilename);
}
*/
AIRString ParticleSystemResource::GetBillBoardTypeStr()
{
    switch(m_billboardType)
    {
    case BBT_POINT:
        return "BBT_POINT";
    case BBT_HORZ_COMMON_DIR_FACE_CAM:
        return "BBT_HORZ_COMMON_DIR_FACE_CAM";
    case BBT_HORZ_COMMON_DIR_FACE_COMMON_UP_DIR:
        return "BBT_HORZ_COMMON_DIR_FACE_COMMON_UP_DIR";
    case BBT_VERT_COMMON_DIR_FACE_CAM:
        return "BBT_VERT_COMMON_DIR_FACE_CAM";
    case BBT_VERT_COMMON_DIR_FACE_COMMON_UP_DIR:
        return "BBT_VERT_COMMON_DIR_FACE_COMMON_UP_DIR";
    case BBT_HORZ_SELF_DIR_FACE_CAM:
        return "BBT_HORZ_SELF_DIR_FACE_CAM";
    case BBT_HORZ_SELF_DIR_FACE_COMMON_UP_DIR:
        return "BBT_HORZ_SELF_DIR_FACE_COMMON_UP_DIR";
    case BBT_VERT_SELF_DIR_FACE_CAM:
        return "BBT_VERT_SELF_DIR_FACE_CAM";
    case BBT_VERT_SELF_DIR_FACE_COMMON_UP_DIR:
        return "BBT_VERT_SELF_DIR_FACE_COMMON_UP_DIR";
    }

    return "";
}

void ParticleSystemResource::SetCommonDir( const Vector3Df& vCommonDir )
{
    m_vCommonDir = vCommonDir;
}

void ParticleSystemResource::SetCommonUpDir( const Vector3Df& vCommonUpDir )
{
    m_vCommonUpDir = vCommonUpDir;
}

void ParticleSystemResource::SetCullBackFace( bool b )
{
    m_bCullBackFace = b;
    /*
    if ( m_bCullBackFace )
    {
        m_pMaterial->SetCullMode( CM_CCW );
    }
    else
    {
        m_pMaterial->SetCullMode( CM_None );
    }
    */
}

void ParticleSystemResource::SetTrailSegmentCount(int nSegment)
{
    m_nTrailSegmentCount = nSegment;
    m_bCreateTrail = false;
}

void ParticleSystemResource::SetMaxChainElement(int nMaxCount)
{
    m_nMaxChainElement = nMaxCount;
    m_bCreateTrail = false;
}

void ParticleSystemResource::SetParticleSystemType(ParticleSystemType pst )
{
    m_ParticleSystemType = pst;
}

void ParticleSystemResource::SetDefaultHeight(float fHeight)
{
    m_fDefaultHeight = fHeight;
}

void ParticleSystemResource::SetDefaultWidth(float fWidth)
{
    m_fDefaultWidth = fWidth;
}

void ParticleSystemResource::SetQuota(int nQuota)
{
    m_nPoolSize = nQuota;
}

void ParticleSystemResource::SetSrcBlend(const AIRString& szSrcBlend )
{
    m_szSrcBlend = szSrcBlend;
    //m_pMaterial->GetCurrentTechnique()->GetPass(0)->SetBlendSrc( (RenderStateBlendValue)EngineParamSetting::GetEngineRenderParam(m_szSrcBlend) /*ENGINE_INST->GetRenderer()->GetRenderParm(m_szSrcBlend)*/ );
}

void ParticleSystemResource::SetDestBlend(const AIRString& szDestBlend )
{
    m_szDestBlend = szDestBlend;
    //m_pMaterial->GetCurrentTechnique()->GetPass(0)->SetBlendDest( (RenderStateBlendValue)EngineParamSetting::GetEngineRenderParam(m_szDestBlend)/*ENGINE_INST->GetRenderer()->GetRenderParm(m_szDestBlend)*/ );
}

void ParticleSystemResource::SetBlendMode(const AIRString& szBlendMode )
{
    m_szBlendMode = szBlendMode;
    //m_pMaterial->GetCurrentTechnique()->GetPass(0)->SetBlendOpt( (BlendOpt)EngineParamSetting::GetEngineRenderParam(m_szBlendMode)/*ENGINE_INST->GetRenderer()->GetRenderParm(m_szBlendMode)*/ );
}

void ParticleSystemResource::SetTrailTex(const char* szTexFile )
{
    m_szTrailTex = szTexFile;
}

void ParticleSystemResource::SetTrailTexCoordDir(u32 dir)
{
    if ( dir == 0 )
    {
        m_TCD = BillBoardChain::TCD_U;
    }
    else
    {
        m_TCD = BillBoardChain::TCD_V;
    }
}

void ParticleSystemResource::OnResourceLoaded(Resource* pResource)
{

}

void ParticleSystemResource::OnResourceReady(Resource* pResource)
{
    /*
    if (pResource == m_pMaterial)
    {
        CTextureUnit* pUnit = m_pMaterial->GetTechnique(0)->GetPass(0)->GetTextureUnit(0);
        if (pUnit)
        {
            ITexture* pTexture = pUnit->GetTexture();
            if (pTexture == NULL)
            {
                pUnit->SetTexture(m_strParticleTexture.c_str(), 0);
            }
            else if (pTexture->GetFileName() != m_strParticleTexture)
            {
                pUnit->SetTexture(m_strParticleTexture.c_str(), 0);
            }
        }
    }
    */
}

void ParticleSystemResource::SetParticleSystemRenderableObjectResource(const AIRString& szRenderableObject )
{
    m_szParticleRenderableResource = szRenderableObject;
}

void ParticleSystemResource::SetEmittedEmitterSize(size_t nQuota)
{
    m_nEmittedEmitterPoolSize = nQuota;
    //m_bInitializeEmittedEmitters = false;
}

/*
void ParticleSystemResource::SetMaterial(IMaterial* pMaterial)
{
    if (m_pMaterial != pMaterial)
    {
        if (m_pMaterial != NULL)
        {
            MATERIAL_MANAGER->ClearMaterial(m_pMaterial);
        }

        m_pMaterial = pMaterial;
    }
}
*/

void ParticleSystemResource::GetEmittersAndAffectors(VECTOR_EMITTER& emitters, VECTOR_AFFECTOR& affectors)
{
    if (m_pStream)
    {
        //暂时以xml来获得，以后改成2进制
        m_pStream->Seek(0, CDataStream::AIR_SEEK_SET);
        AIRString strDoc = m_pStream->GetAsString();

        CMarkupSTL xml;
        xml.SetDoc(strDoc.c_str());

        if (!xml.FindElem())
        {
            return;
        }

        xml.IntoElem();
        while(xml.FindElem())
        {
            //加载发射器
            if (xml.GetTagName() == "Emitter")
            {
                LoadEmitter(xml, emitters);
            }
            //加载影响器
            else if (xml.GetTagName() == "Affector")
            {
                LoadAffector(xml, affectors);
            }
        }
        xml.OutOfElem();
    }
}

void ParticleSystemResource::LoadEmitter(CMarkupSTL& xml, VECTOR_EMITTER& emitters)
{
    AIRString strValue = xml.GetAttrib("type").c_str();

    CParticleEmitter* pEmitter = PARTICLEMITTER_FACTORY->CreateEmitter(strValue, xml);

    if (pEmitter != NULL)
    {
        emitters.push_back(pEmitter);
    }
}

void ParticleSystemResource::LoadAffector(CMarkupSTL& xml, VECTOR_AFFECTOR& affectors)
{
    AIRString strValue = xml.GetAttrib("type").c_str();

    CParticleAffector* pAffector = PARTICLEAFFECTOR_FACTORY->CreateAffector(strValue, xml);

    if (pAffector != NULL)
    {
        //pAffector->SetParticleSystem( this );
        affectors.push_back(pAffector);
    }
}
