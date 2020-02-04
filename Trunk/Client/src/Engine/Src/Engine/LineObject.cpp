#include "stdafx.h"
#include "LineObject.h"
#include "MaterialManager.h"
#include "RenderQueueGroup.h"

IMaterial* LineObject::m_pLineMaterial = NULL;

LineObject::LineObject()
{
    //m_strName = "LineObject";
    m_szType = "LineObject";
    //m_bShowBoundingBox = true;
    m_renderGroupType = eRGT_Line;
}

LineObject::~LineObject()
{
    
}

bool LineObject::Initialize()
{
    if (m_pLineMaterial == NULL)
    {
        m_pLineMaterial = MATERIAL_MANAGER->CreateMaterial("TemplateMaterial.xml/LineObject");
    }

    if (m_pMaterial == NULL)
    {
        m_pMaterial = m_pLineMaterial;
        m_bShareParentMaterial = true;
    }

    return Renderable::Initialize();
}
