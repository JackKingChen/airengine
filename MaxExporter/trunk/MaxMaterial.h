#pragma once
#include "Max.h"
#include <stdmat.h>
#include <string>
#include <vector>

//对TexLayer的一个包装，
//在导出过程中需要一些额外的数据，比如贴图的变换矩阵
typedef struct sMaxTexLayer
{
    //sModelTexLayer m_MTL;
    char              m_szTexName[64];   //纹理文件的名字 
    //eTexLayerUsage    m_Usage;         //纹理的用途
    unsigned short    m_UVChannel;     //使用的哪一个UVChannel，实际中可能没有用上
    Matrix3        m_UVTransMat;      //UV偏移矩阵

}MAXTEXLAYER, *LPMAXTEXLAYER;

class CMaxMaterial
{
    // member variables
protected:
    StdMat *m_pIStdMat;

    // constructors/destructor
public:
    CMaxMaterial();
    virtual ~CMaxMaterial();

    //清空材质
    void Clear();

    // member functions
public:
    //创建材质，
    bool Create(StdMat *pIStdMat);
    void GetAmbientColor(float *pColor);
    void GetDiffuseColor(float *pColor);
    int  GetMapCount();
    std::string GetMapFilename(int mapId);
    std::string GetName();
    void GetSpecularColor(float *pColor);
    float GetShininess();
    StdMat* GetMaxMaterial() { return m_pIStdMat; };

    float GetOpacity(int nTime)
    {
        return m_pIStdMat->GetOpacity(nTime);
    }

    bool GetTwoSide()
    {
        return m_pIStdMat->GetTwoSided();
    }

    bool IsAlphaTest() const
    {
        return m_bAlphaTest;
    }

    int  m_nMatID;    //在材质列表中的序号

    bool m_bAlphaTest;

    static int s_nNewCount;
private:
    std::vector<LPMAXTEXLAYER> m_vtTexLayers;
    typedef std::vector<LPMAXTEXLAYER>::iterator MAXTEXLAYER_ITER;
};
