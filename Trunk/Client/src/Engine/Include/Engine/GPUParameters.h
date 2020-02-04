// ***************************************************************
//  �ļ�: GPUParameters.h
//  ����: 2010-3-11
//  ����: liangairan
//  ˵��: GPU�����࣬ר�Ŵ���GPU�Ĳ���
// ***************************************************************

#pragma once 
#include "EngineConfig.h"
#include "Vector3D.h"
#include "Vector4.h"
#include "Matrix4.h"
#include "ColorValue.h"
#include <vector>
#include <map>
#include <string>
#include "GPUDataParam.h"
#include "MemoryAlloter.h"
#include "FixedString.h"

using namespace AIR;

class CShaderParameter;
//class ManualGPUDataParamSource;

enum GpuConstantType
{
    eGCT_FLOAT1 = 1,
    eGCT_FLOAT2 = 2,
    eGCT_FLOAT3 = 3,
    eGCT_FLOAT4 = 4,
    eGCT_SAMPLER1D = 5,
    eGCT_SAMPLER2D = 6,
    eGCT_SAMPLER3D = 7,
    eGCT_SAMPLERCUBE = 8,
    eGCT_SAMPLER1DSHADOW = 9,
    eGCT_SAMPLER2DSHADOW = 10,
    eGCT_MATRIX_2X2 = 11,
    eGCT_MATRIX_2X3 = 12,
    eGCT_MATRIX_2X4 = 13,
    eGCT_MATRIX_3X2 = 14,
    eGCT_MATRIX_3X3 = 15,
    eGCT_MATRIX_3X4 = 16,
    eGCT_MATRIX_4X2 = 17,
    eGCT_MATRIX_4X3 = 18,
    eGCT_MATRIX_4X4 = 19,
    eGCT_INT1 = 20,
    eGCT_INT2 = 21,
    eGCT_INT3 = 22,
    eGCT_INT4 = 23,
    eGCT_BOOL = 24,
    eGCT_UNKNOWN = 99
};

enum AutoConstantType
{
    eACT_WORLDMATRIX,
    eACT_VIEWMATRIX,
    eACT_PROJECTIONMATRIX,
    eACT_WVPMATRIX,
	eACT_INVWVPMATRIX,
    eACT_WORLDVIEWMATIRX,   //
    eACT_VIEWPROJMATRIX,
    eACT_TEXPROJMATRIX,     //ͶӰ�������
    eACT_CAMERAPOSITION,
    eACT_AMBIENT_COLOR,
    eACT_FOG_COLOR,
    eACT_FOG_START,
    eACT_FOG_END,
    eACT_FOG_DENSITY,      //��ָ��ֵ
    eACT_LIGHT_POSITION,
    eACT_LIGHT_DIRECTION,
    eACT_SUN_LIGHT_DIRECTION,
    eACT_SUN_LIGHT_COLOR,       //̫������ɫ
    eACT_LIGHT_DIFFUSE_COLOR,
    eACT_LIGHT_SPECULAR_COLOR,
    eACT_LIGHT_AMBIENT_COLOR,
    eACT_CLIPPLANE,
    //eACT_UVSCALE_ARRAY,     //����������������
    //eACT_UVSCALE,           //����������������
    eACT_MATERIAL_AMBIENT,  //���ʱ��滷�������
    eACT_MATERIAL_DIFFUSE,  //���ʱ������������
    eACT_MATERIAL_SPECULAR, //���ʱ��澵�淴�����
    eACT_MATERIAL_SHINESS,  //���ʱ��澵�淴��ǿ��
	eACT_FRAME_TIME,
    eACT_BONE_MATRIX,       //��������
    eACT_WORLDMATRIX_ARRAY, //�����������
    eACT_POSITION_ARRAY,
    eACT_LIGHT_RADIUS,
    eACT_VIEWPORT_WIDTH,    //�ӿڿ�
    eACT_VIEWPORT_HEIGHT,   //�ӿڸ�
    eACT_LIGHTPOS_INVIEW,   //����ͼ�ռ�ĵ��Դ����
    eACT_INVVIEWPROJ_MATRIX,   //inverse(view * project)
    eACT_CAMERA_FARPLANE,    //�����Զ����
    eACT_FARPLANE_CORNERS,   //Զ������ĸ���
    eACT_HALFPIXEL,          //�������
    eACT_COLOR_DIFFUSE,
    eACT_FOG_TYPE,
    eACT_CAMERA_RIGHT,       //�����������
    eACT_CAMERA_LOOK,        //���������
    eACT_CAMERA_UP,          //�����������
	eACT_CLIPENABLE,         //�Ƿ�Ҫ�òý���
    eACT_JITTERSCALE,
    eACT_FILTTERSIZE,
    eACT_JITTERDELTA,
    eACT_LightSpaceProjMatrix,
    eACT_LightSpaceViewProjMatrix,
    //eACT_AlphaRef,
    eACT_UNKNOWN,
};

struct GPUConstantDefinition 
{
    FixedString      strName;      //����
    GpuConstantType  ConstantType; //��������
    AutoConstantType UsageType;    //��;����
    u32    nHandle;          //��Ӧ��handle
    size_t nPhysicalIndex;   //��GPUParameters�����������
    size_t nRegisterIndex;   //sm3.0�����ǼĴ���������sm5.0��cbuffer�е���ʼ�ֽ�����
    size_t nRegisterCount;    //�Ĵ�����������ogre�е�elementSize
    size_t nElementSize;     //D3D9�е�λΪ4��float�Ĵ�С��SM5������һ��ֵ�Ĵ�С
    size_t nSizeInBytes;
    size_t nArraySize;   //���鳤��
    int    nManualUsageType;   //�ֶ�������;
    void*  pDefaultValue;
    //ManualGPUDataParamSource* pManaulGPUSource;
    u32 nRealValueOffset;   //һ��ֵ��������С byte
    void* pValue;
    u32 nConstantBuffer;    //�����ĸ�ConstantBuffer

    bool IsFloat() const
    {
        return IsFloat(ConstantType);
    }

    static bool IsFloat(GpuConstantType c)
    {
        switch(c)
        {
        case eGCT_INT1:
        case eGCT_INT2:
        case eGCT_INT3:
        case eGCT_INT4:
        case eGCT_SAMPLER1D:
        case eGCT_SAMPLER2D:
        case eGCT_SAMPLER3D:
        case eGCT_SAMPLERCUBE:
        case eGCT_SAMPLER1DSHADOW:
        case eGCT_SAMPLER2DSHADOW:
            return false;
        default:
            return true;
        };

    }

    static bool IsInt(GpuConstantType c)
    {
        switch(c)
        {
        case eGCT_INT1:
        case eGCT_INT2:
        case eGCT_INT3:
        case eGCT_INT4:
            return true;
        case eGCT_SAMPLER1D:
        case eGCT_SAMPLER2D:
        case eGCT_SAMPLER3D:
        case eGCT_SAMPLERCUBE:
        case eGCT_SAMPLER1DSHADOW:
        case eGCT_SAMPLER2DSHADOW:
            return false;
        default:
            return false;
        };

    }

    bool IsInt() const
    {
        return IsInt(ConstantType);
    }

    bool IsSampler() const
    {
        return IsSampler(ConstantType);
    }

    static bool IsSampler(GpuConstantType c)
    {
        switch(c)
        {
        case eGCT_SAMPLER1D:
        case eGCT_SAMPLER2D:
        case eGCT_SAMPLER3D:
        case eGCT_SAMPLERCUBE:
        case eGCT_SAMPLER1DSHADOW:
        case eGCT_SAMPLER2DSHADOW:
            return true;
        default:
            return false;
        };
    }

    /** Get the element size of a given type, including whether to pad the 
    elements into multiples of 4 (e.g. SM1 and D3D does, GLSL doesn't)
    */
    static size_t GetElementSize(GpuConstantType ctype, bool padToMultiplesOf4 = false)
    {
        if (padToMultiplesOf4)
        {
            switch(ctype)
            {
            case eGCT_FLOAT1:
            case eGCT_INT1:
            case eGCT_SAMPLER1D:
            case eGCT_SAMPLER2D:
            case eGCT_SAMPLER3D:
            case eGCT_SAMPLERCUBE:
            case eGCT_SAMPLER1DSHADOW:
            case eGCT_SAMPLER2DSHADOW:
            case eGCT_FLOAT2:
            case eGCT_INT2:
            case eGCT_FLOAT3:
            case eGCT_INT3:
            case eGCT_FLOAT4:
            case eGCT_INT4:
                return 4;
            case eGCT_MATRIX_2X2:
            case eGCT_MATRIX_2X3:
            case eGCT_MATRIX_2X4:
                return 8; // 2 float4s
            case eGCT_MATRIX_3X2:
            case eGCT_MATRIX_3X3:
            case eGCT_MATRIX_3X4:
                return 12; // 3 float4s
            case eGCT_MATRIX_4X2:
            case eGCT_MATRIX_4X3:
            case eGCT_MATRIX_4X4:
                return 16; // 4 float4s
            default:
                return 4;
            };
        }
        else
        {
            switch(ctype)
            {
            case eGCT_FLOAT1:
            case eGCT_INT1:
            case eGCT_SAMPLER1D:
            case eGCT_SAMPLER2D:
            case eGCT_SAMPLER3D:
            case eGCT_SAMPLERCUBE:
            case eGCT_SAMPLER1DSHADOW:
            case eGCT_SAMPLER2DSHADOW:
                return 1;
            case eGCT_FLOAT2:
            case eGCT_INT2:
                return 2;
            case eGCT_FLOAT3:
            case eGCT_INT3:
                return 3;
            case eGCT_FLOAT4:
            case eGCT_INT4:
                return 4;
            case eGCT_MATRIX_2X2:
                return 4;
            case eGCT_MATRIX_2X3:
            case eGCT_MATRIX_3X2:
                return 6;
            case eGCT_MATRIX_2X4:
            case eGCT_MATRIX_4X2:
                return 8; 
            case eGCT_MATRIX_3X3:
                return 9;
            case eGCT_MATRIX_3X4:
            case eGCT_MATRIX_4X3:
                return 12; 
            case eGCT_MATRIX_4X4:
                return 16; 
            default:
                return 4;
            };

        }
    }

    GPUConstantDefinition()
        : ConstantType(eGCT_UNKNOWN)
        , nHandle(0)
        , nPhysicalIndex((std::numeric_limits<size_t>::max)())
        , nManualUsageType(-1)
        , pDefaultValue(NULL)
        //, pManaulGPUSource(NULL)
        , nRealValueOffset(0)
        , nArraySize(0)
        , pValue(0)
        , nConstantBuffer(0)
        //, elementSize(0)
        //, arraySize(1)
        //, variability(GPV_GLOBAL) 
    {

    }
};

class EngineExport GPUParameters// : public EngineAllocator(GPUParameters)
{
	EngineAllocator(GPUParameters)
public:
    typedef AIRVector<GPUConstantDefinition> GPUConstDef_Vector;
    typedef GPUConstDef_Vector::iterator  GPUConstDef_Vector_Iter;

    //ȡ���ֶ����²���
    GPUConstDef_Vector* GetManualGPUConstants()
    {
        return &m_vtManualGPUConstDef;
    }

    GPUConstDef_Vector  m_vtGPUConstDef;         //�Զ�����
    GPUConstDef_Vector  m_vtManualGPUConstDef;   //�ֶ�����

    s32 GetGPUVariantIndex(const FixedString& strName) const; 
    
public:
    GPUParameters();
    virtual ~GPUParameters();

 

    virtual void AutoUpdateParameters(CGPUDataParamSource* pGPUDataParam) {};

    //size_t GetFloatPhysicalIndex(size_t requestedSize);

    //size_t GetIntPhysicalIndex(size_t requestedSize);

    virtual void AddConstantDef(const FixedString& strName, GPUConstantDefinition& def) {};

    //virtual void BuildManualConstantType(ManualGPUDataParamSource* pSource);

    //�����ֶ�GPU����
    //virtual void ManualUpdateGPUParameters() = 0;

	//inline void SetManualConstantTypeBuilded( bool builded )
	//{
	//	m_bManualConstantTypeBuilded = builded;
	//}
    
    //AIRVector<int>      m_vtIntConstants;
    //AIRVector<float>    m_vtFloatConstants;

    //bool m_bManualConstantTypeBuilded;    //�Ƿ��Ѿ��������ֶ�constant������

    static AutoConstantType GetAutoConstantTypeByName(const FixedString& strName);
    //ManualGPUDataParamSource* m_pManualGPUParamSource;
private:
};
