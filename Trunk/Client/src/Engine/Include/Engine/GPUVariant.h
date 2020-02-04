#pragma once 

#include "MemoryAlloter.h"
#include "EngineDefination.h"
#include "Vector2D.h"
#include "Vector3D.h"
#include "Vector4.h"
#include "Matrix4.h"
#include "Color.h"

class ITexture;

class EngineExport GPUVariant// : public EngineAllocator(GPUVariant)
{
	EngineAllocator(GPUVariant)
public:
    GPUVariant();
    GPUVariant(const GPUVariant& var);
    GPUVariant(bool b);
    GPUVariant(s32 i);
    GPUVariant(f32 f);
    GPUVariant(const Vector2Df& v);
    GPUVariant(const Vector3Df& v);
    GPUVariant(const Vector4f& v);
    GPUVariant(const CColorValue& col);
    GPUVariant(const Matrix4f& mat);
    GPUVariant(ITexture* pTexture);

    ~GPUVariant();

    GPUVariant& operator = (const GPUVariant& var)
    {
        Clear();
        m_type = var.m_type;

        if (m_type == VAR_FLOAT4X4)
        {
            m_pMatrix = AIR_NEW_ARRAY_T(f32, 16);
            memcpy(m_pMatrix, var.m_pMatrix, sizeof(f32) * 16);
        }
        else
        {
            memcpy(m_vec, var.m_vec, sizeof(m_vec));
        }
        return *this;
    }

    void SetBool(bool b);
    void SetInt(s32 i);
    void SetFloat(f32 f);
    void SetVector2(const Vector2Df& vec);
    void SetVector3(const Vector3Df& vec);
    void SetVector4(const Vector4f& vec);
    void SetColor(const CColorValue& color);
    void SetMatrix(const Matrix4f& mat);
    void SetPointer(void* pPointer);
    void SetTexture(ITexture* pTexture);
    void Set(VARIANT_T type, const char* pValue);

    const void* GetAddress() const
    {
        if (m_type == VAR_FLOAT4X4)
        {
            return m_pMatrix;
        }
        else if(m_type == VAR_POINTER)
        {
            return m_p;
        }
        else if (m_type == VAR_TEXTURE)
        {
            return m_pTexture;
        }
        return &m_f;
    }

    bool GetBool() const
    {
        return m_b;
    }

    f32 GetFloat() const
    {
        return m_f;
    }

    void* GetPointer() const
    {
        return m_p;
    }

    ITexture* GetTexture()
    {
        return m_pTexture;
    }

    s32 GetInt() const
    {
        return m_i;
    }

    VARIANT_T GetType() const
    {
        return m_type;
    }
protected:
private:
    void Clear()
    {
        if (m_type == VAR_FLOAT4X4)
        {
            if (m_pMatrix)
            {
                AIR_DELETE_ARRAY_T(m_pMatrix, f32, 16);
            }
        }
    }

    void SetValue(const char* pValue);

    VARIANT_T m_type;
    union
    {
        bool m_b;
        s32  m_i;
        f32  m_f;
        f32  m_vec[4];
        f32* m_pMatrix;
        void* m_p;
        ITexture* m_pTexture;
    };
};



class EngineExport GPUVariantArray// : public EngineAllocator(GPUVariantArray)
{
	EngineAllocator(GPUVariantArray)
public:
    struct stVarValue
    {
        f32 vec[4];
    };
public:
    GPUVariantArray();
    GPUVariantArray(const GPUVariantArray& rhs);

    virtual ~GPUVariantArray() {}

    GPUVariantArray& operator = (const GPUVariantArray& rhs)
    {
        return *this;
    }

    void Push(const GPUVariant& variant)
    {
        stVarValue value;
        memcpy(&value, variant.GetAddress(), 16);
        m_vtGPUVariants.push_back(value);
    }

    size_t GetArraySize() const
    {
        return m_vtGPUVariants.size();
    }

    void* GetAddress()
    {
        return &m_vtGPUVariants[0];
    }
private:
    AIRVector<stVarValue> m_vtGPUVariants;
};
