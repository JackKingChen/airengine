#include "stdafx.h"
#include "FlareSpot.h"
#include "Engine.h"
#include "TextureMgr.h"

CFlareSpot::CFlareSpot(void) : m_pTexture(NULL), m_nScreenX(0), m_nScreenY(0), m_nWidth(0), m_clrBlend(0xffffffff)
{
   
}

CFlareSpot::~CFlareSpot(void)
{
}

bool CFlareSpot::Create(const char* szTextureFile)
{
    if (!LoadTexture(szTextureFile))
        return false;

    /*
    if (!CreateVertexBuffer())
    {
    return false;
    }
    */

    return true;
}

bool CFlareSpot::LoadTexture(const char* szFilename)
{
    //IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    m_pTexture = TEXTURE_MANAGER->CreateTextureFromFile(szFilename);

    /*
    LPDIRECT3DDEVICE9 pDevice = CRenderSystem::GetInstance()->GetDevice();
    HRESULT hr = D3DXCreateTextureFromFile(pDevice, szFilename, &m_pTexture);
    if (FAILED(hr))
    {
    return false;
    }
    */
    return (m_pTexture != NULL);
}

void CFlareSpot::Clear()
{
    TEXTURE_MANAGER->ClearTexture(m_pTexture);
    m_pTexture = NULL;
    //SAFE_RELEASE(m_pVertexBuffer);
    //SAFE_RELEASE(m_pTexture);
}

void CFlareSpot::SetColor(const SColor& dwColor)
{
    m_clrBlend = dwColor;
}

void CFlareSpot::Render()
{
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    //pRenderer->Draw2DImage(m_Rect, Rectf(0.0f, 0.0f, 1.0f, 1.0f), m_pTexture, m_clrBlend);
}

void CFlareSpot::GetScreenPos(int& nX, int& nY)
{
    nX = m_nScreenX;
    nY = m_nScreenY;
}

void CFlareSpot::SetScreenPos(int nX, int nY)
{
    m_nScreenX = nX;
    m_nScreenY = nY;

    m_Rect.SetRect(m_nScreenX - m_nWidth / 2, m_nScreenY - m_nWidth / 2, m_nWidth, m_nWidth);

    
}

void CFlareSpot::SetScreenPos(const Vector2Df& pos)
{
    m_nScreenX = pos.x;
    m_nScreenY = pos.y;

    m_Rect.SetRect(m_nScreenX - m_nWidth / 2, m_nScreenY - m_nWidth / 2, m_nWidth, m_nWidth);

}

void CFlareSpot::SetWidth(int nWidth)
{
    m_nWidth = nWidth;
}
