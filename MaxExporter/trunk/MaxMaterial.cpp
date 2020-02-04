#include "MaxMaterial.h"

using namespace std;

int CMaxMaterial::s_nNewCount = 0;

//取得文件名
std::string getTextureName(std::string& Name)
{
    string::size_type pos;
    string fileName ;
    pos = Name.find_last_of("\\");
    if(pos == string::npos) 
        fileName = Name;

    fileName = Name.substr(pos + 1);
    return fileName;
}

//----------------------------------------------------------------------------//
// Constructors                                                               //
//----------------------------------------------------------------------------//

CMaxMaterial::CMaxMaterial() : m_pIStdMat(NULL), m_bAlphaTest(false)
{
    m_vtTexLayers.reserve(12);
    s_nNewCount++;
}

//----------------------------------------------------------------------------//
// Destructor                                                                 //
//----------------------------------------------------------------------------//

CMaxMaterial::~CMaxMaterial()
{
    s_nNewCount--;
    Clear();
}

void CMaxMaterial::Clear()
{
    for (MAXTEXLAYER_ITER it = m_vtTexLayers.begin(); it != m_vtTexLayers.end(); it++)
    {
        LPMAXTEXLAYER pTexLayer = *it;
        delete pTexLayer;
        pTexLayer = NULL;
    }
    m_vtTexLayers.clear();
}

bool CMaxMaterial::Create(StdMat *pIStdMat)
{
    if (pIStdMat == NULL)
    {
        return false;
    }

    if(pIStdMat->ClassID() != Class_ID(DMTL_CLASS_ID, 0) && pIStdMat->ClassID() != Class_ID(MULTI_CLASS_ID, 0) )
        return false;
    //循环12次，目的是把所有texture遍历一次，实际上是由ID_AM到ID_DP，参考3dmax List of Texture Map Indices
    for(int tex_layer = 0 ; tex_layer < 12 ; ++tex_layer)  
    {
        Texmap* pTexMap = pIStdMat->GetSubTexmap(tex_layer);
        
        //贴图是不是被激活
        if(pTexMap != NULL && pIStdMat->MapEnabled(tex_layer))
        {
            if(pTexMap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0))
            {
                // get the full filepath
                LPMAXTEXLAYER pTexLayer = new MAXTEXLAYER;
                std::string strFilename;
                BitmapTex *pBMPMap = (BitmapTex *)pTexMap;
                
                strFilename = getTextureName( string(pBMPMap->GetMapName()) );
                strncpy(pTexLayer->m_szTexName, strFilename.c_str(), 64);
                //texLayer.m_MTL.m_Usage = (eTexLayerUsage)tex_layer;

                //先将其设置为
                int ch = pBMPMap->GetMapChannel();
                pTexLayer->m_UVChannel = 0; //__find_used_map_channel_index(ch);

                pTexMap->GetUVTransform(pTexLayer->m_UVTransMat);                            
                m_vtTexLayers.push_back(pTexLayer);
            }
            if (tex_layer == ID_OP)
            {
                m_bAlphaTest = true;
            }
        }
    }

    m_pIStdMat = pIStdMat;

    return true;
}

void CMaxMaterial::GetAmbientColor(float *pColor)
{
    // get color from the internal standard material
    Color color;
    color = m_pIStdMat->GetAmbient(0);

    pColor[0] = color.r;
    pColor[1] = color.g;
    pColor[2] = color.b;
    pColor[3] = 0.0f;
}

void CMaxMaterial::GetDiffuseColor(float *pColor)
{
    // get color from the internal standard material
    Color color;
    color = m_pIStdMat->GetDiffuse(0);

    pColor[0] = color.r;
    pColor[1] = color.g;
    pColor[2] = color.b;
    pColor[3] = m_pIStdMat->GetOpacity(0);
}

int CMaxMaterial::GetMapCount()
{
    int mapCount;
    mapCount = 0;

    // loop through all maps of the material
    int mapId;
    for(mapId = 0; mapId < m_pIStdMat->NumSubTexmaps(); mapId++)
    {
        // get the texture map
        Texmap *pTexMap;
        pTexMap = m_pIStdMat->GetSubTexmap(mapId);

        // check if the map is valid
        if((pTexMap != 0) && (m_pIStdMat->MapEnabled(mapId)))
        {
            mapCount++;
        }
    }

    return mapCount;
}

std::string CMaxMaterial::GetMapFilename(int mapId)
{
    int mapCount;
    mapCount = 0;

    // loop through all maps of the material
    int materialMapId;
    for(materialMapId = 0; materialMapId < m_pIStdMat->NumSubTexmaps(); materialMapId++)
    {
        // get the texture map
        Texmap *pTexMap;
        pTexMap = m_pIStdMat->GetSubTexmap(materialMapId);

        // check if the map is valid
        if((pTexMap != 0) && (m_pIStdMat->MapEnabled(materialMapId)))
        {
            // check if we reached the wanted map
            if(mapId == mapCount)
            {
                if(pTexMap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0))
                {
                    // get the full filepath
                    std::string strFilename;
                    strFilename = ((BitmapTex *)pTexMap)->GetMapName();

                    // extract the filename
                    std::string::size_type pos;
                    pos = strFilename.find_last_of("\\/:");
                    if(pos == std::string::npos) return strFilename;

                    return strFilename.substr(pos + 1);
                }
                else return "<none>";
            }

            mapCount++;
        }
    }

    return "";
}

std::string CMaxMaterial::GetName()
{
    if (m_pIStdMat)
    {
        std::string name = m_pIStdMat->GetName();
        return name;
    }
    return "<none>";
}

float CMaxMaterial::GetShininess()
{
    return m_pIStdMat->GetShinStr(0);
}

void CMaxMaterial::GetSpecularColor(float *pColor)
{
    Color color;
    color = m_pIStdMat->GetSpecular(0);

    pColor[0] = color.r;
    pColor[1] = color.g;
    pColor[2] = color.b;
    pColor[3] = 0.0f;
}
