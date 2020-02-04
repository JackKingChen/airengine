#include "stdafx.h"
#include "SunLight.h"
#include "Engine.h"
#include "ISceneManager.h"

CSunLight::CSunLight(void)
{
}

CSunLight::~CSunLight(void)
{
}

void CSunLight::Initialize(const Vector3Df& pos, const Vector3Df& camPos)
{
    //m_vSunLight = vLight;
    m_posSun = pos;
    Vector3Df vCam = camPos;
    // CAMERAINST->GetEvePos(&vCam);

    m_vSunLight = -m_posSun;
    m_fSunToCam = m_vSunLight.GetLength();//D3DXVec3Length(&m_vSunLight);
    m_vSunLight.Normalize();
    //D3DXVec3Normalize(&m_vSunLight, &m_vSunLight);

    m_posSun = m_posSun + (-m_vSunLight) * 1000000.0f;

    //m_spots[0].SetPos(m_posSun + 50 * m_vSunLight);
    //m_spots[1].SetPos(m_posSun + 150 * m_vSunLight);
    //m_spots[2].SetPos(m_posSun + 250 * m_vSunLight);
    //m_spots[3].SetPos(m_posSun + 500 * m_vSunLight);
    m_sun.Create("Sun.dds");
    m_sun.SetWidth(300);

    m_spots[0].Create("Flare1.dds");
    m_spots[1].Create("Flare2.dds");
    m_spots[2].Create("Flare1.dds");
    m_spots[3].Create("Flare2.dds");
    m_spots[4].Create("Flare3.dds");


    m_spots[0].SetWidth(80);
    m_spots[1].SetWidth(50);
    m_spots[2].SetWidth(100);
    m_spots[3].SetWidth(80);
    m_spots[4].SetWidth(230);

    for (int i = 0; i < SPOT_COUNT; i++)
    {
        m_spots[i].SetColor(SColor(120, 200, 20, 170));
    }

}

void CSunLight::SetSunPos(const Vector3Df& pos)
{
    m_posSun = pos;
}

void CSunLight::SetSunLight(const Vector3Df& vLight)
{
    m_vSunLight = vLight;
}

void CSunLight::Render(CCamera* pCamera)
{
    //D3DXVECTOR3 vCam;
    //CAMERAINST->GetEvePos(&vCam);

    //m_vSunLight = vCam - m_posSun;
    //m_fSunToCam = D3DXVec3Length(&m_vSunLight);
    //D3DXVec3Normalize(&m_vSunLight, &m_vSunLight);
    int nScreenX, nScreenY;
    bool bClip = false;

    Vector2D viewport;
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    pRenderer->GetBackBufferSize(viewport);

    Translate3Dto2D(viewport, m_posSun, nScreenX, nScreenY, &bClip);

    if (bClip)
    {
        return;
    }

    Vector2Df vScreenCenter(viewport.x * 0.5, viewport.y * 0.5);
    Vector2Df  vSun2D(nScreenX, nScreenY);

    Vector2Df vLight = vScreenCenter - vSun2D;
    float fSunToCenter = vLight.GetLength();//D3DXVec2Length(&vLight);
    //D3DXVec2Normalize(&vLight, &vLight);
    vLight.Normalize();


    int nAlpha = 255 - 255 * (fSunToCenter / (1.5 * (float)viewport.x));

    if (nAlpha <= 0)
    {
        return;
    }

    for (int i = 0; i < SPOT_COUNT; i++)
    {
        m_spots[i].SetColor(SColor((u32)nAlpha, 220, 180, 50));
    }


    m_sun.SetScreenPos(vSun2D);
    m_spots[0].SetScreenPos(vSun2D + vLight * 0.1f * fSunToCenter);
    m_spots[1].SetScreenPos(vSun2D + vLight * 0.2f * fSunToCenter);
    m_spots[2].SetScreenPos(vSun2D + vLight * 0.5f * fSunToCenter);
    m_spots[3].SetScreenPos(vSun2D + vLight * 0.8f * fSunToCenter);
    m_spots[4].SetScreenPos(vSun2D + vLight * 1.1f * fSunToCenter);

    //D3DXVECTOR3  pos;
    //int nScreenX, nScreenY;
    //LPDIRECT3DDEVICE9 pDevice = CRenderSystem::GetInstance()->GetDevice();
    //pDevice->SetTexture(0, m_pSunTexture);
    //m_spots[0].Render();
    //pRenderer->BeginGUIRenderStates();

    /*
    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
    pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    */
    m_sun.Render();


    for (int i = 0; i < SPOT_COUNT; i++)
    {
        m_spots[i].Render();
    }

    //pRenderer->EndGUIRenderStates();
    //pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
}

void CSunLight::Update(CCamera* pCamera)
{
    Vector3Df vCam;
    //CAMERAINST->GetEvePos(&vCam);
    pCamera->GetPosition(vCam);

    //m_vSunLight = vCam - m_posSun;
    m_fSunToCam = m_vSunLight.GetLength(); //D3DXVec3Length(&m_vSunLight);
    //D3DXVec3Normalize(&m_vSunLight, &m_vSunLight);

   
}

void CSunLight::Translate3Dto2D(const Vector2D& viewport, const Vector3Df& vPos, int& nX, int& nY, bool* bClip)
{
    //LPDIRECT3DDEVICE9 p3DDevice9 = CRenderSystem::GetInstance()->GetDevice();
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();

    Matrix4f matProj, matView, matWorld, matWVP;
    pRenderer->GetViewMatrix(&matView);
    pRenderer->GetProjectionMatrix(&matProj);
    
    ISceneManager* pScene = ENGINE_INST->GetSceneMgr();
    matWorld = pScene->GetSkybox()->GetWorldMatrix();

    matWVP = matWorld * matView * matProj;

    float fX,fY,fZ,fW;

    fX = (matWVP[0] * vPos.x) + (matWVP[4] * vPos.y) + (matWVP[8] * vPos.z) + matWVP[12];

    fY = (matWVP[1] * vPos.x) + (matWVP[5] * vPos.y) 
        + (matWVP[9] * vPos.z) + matWVP[13];

    fZ = (matWVP[2] * vPos.x) + (matWVP[6] * vPos.y) 
        + (matWVP[10] * vPos.z) + matWVP[14];

    //如果被投影到后面，也就是Z为负的，就要剪裁掉。

    if(fZ <= -1)
    {
        *bClip = true;
    }

    else
    {
        *bClip = false;
    }

    fW = (matWVP[3] * vPos. x) + (matWVP[7] * vPos.y) 
        + (matWVP[11] * vPos.z) + matWVP[15];

    float fWpInv = 1.0f / fW;

    u32 dwScreenWidth = viewport.x;
    u32 dwScreenHeight = viewport.y;

    //视口大小一半

    float fClip_x = (float)(dwScreenWidth  >> 1);
    float fClip_y = (float)(dwScreenHeight >> 1);

    //投影坐标范围为[-1,1]要变成[0,2]并且屏幕坐标y方向向下,需要反转

    nX = (s32)( (1.0f + (fX * fWpInv)) * fClip_x );
    nY = (s32)( ((fY * fWpInv) - 1.0f ) * (-fClip_y) );

}

void CSunLight::SetupMatrix()
{
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    pRenderer->SetWorldTransform(&m_matWorld);
    //CRenderSystem::GetInstance()->GetDevice()->SetTransform(D3DTS_WORLD, &m_matWorld);
}

void CSunLight::Clear()
{

    for (int i = 0; i < SPOT_COUNT; i++)
    {
        m_spots[i].Clear();
    }
}

void CSunLight::GetSunLightVec(Vector3Df& vLight) const
{
    vLight = m_vSunLight;
}
