#pragma once
#include <string>
#include <vector>
#include "MaxMaterial.h"

class CAIRMaterial
{
public:
    typedef struct tagMap 
    {
        std::string m_strFilename;
    }MAP, *LPMAP;
public:
    CAIRMaterial(void);
    virtual ~CAIRMaterial(void);

    size_t GetMapCount() const { return m_vtTextures.size(); }

    bool Create(CMaxMaterial* pMaterial);

    std::string GetName() const { return m_pMaterial->GetName(); }

    std::string GetTextureFileName(int nMapID = 0) const 
    {
        return m_pMaterial->GetMapFilename(nMapID);
    }

    void GetAmbientColor(float *pColor);
    void GetDiffuseColor(float *pColor);
    CMaxMaterial* GetMaterial();
    float GetShininess();
    void GetSpecularColor(float *pColor);

    bool IsAlphaTest() const;

    bool IsAlphaBlend() const;

    //决定cullmode形式
    bool IsTwoSide() const;

    void Clear();

protected:
    CMaxMaterial*   m_pMaterial;
    bool            m_bAlphaBlend;

    std::vector<MAP>   m_vtTextures;

};
