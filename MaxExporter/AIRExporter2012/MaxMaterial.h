#pragma once
#include "Max.h"
#include <stdmat.h>
#include <string>
#include <vector>

//��TexLayer��һ����װ��
//�ڵ�����������ҪһЩ��������ݣ�������ͼ�ı任����
typedef struct sMaxTexLayer
{
    //sModelTexLayer m_MTL;
    char              m_szTexName[64];   //�����ļ������� 
    //eTexLayerUsage    m_Usage;         //�������;
    unsigned short    m_UVChannel;     //ʹ�õ���һ��UVChannel��ʵ���п���û������
    Matrix3        m_UVTransMat;      //UVƫ�ƾ���

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

    //��ղ���
    void Clear();

    // member functions
public:
    //�������ʣ�
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

    int  m_nMatID;    //�ڲ����б��е����

    bool m_bAlphaTest;

    static int s_nNewCount;
private:
    std::vector<LPMAXTEXLAYER> m_vtTexLayers;
    typedef std::vector<LPMAXTEXLAYER>::iterator MAXTEXLAYER_ITER;
};
