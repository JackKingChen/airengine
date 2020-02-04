#pragma once 

#include "Matrix4.h"
#include "Plane3D.h"

using namespace AIR;

class CFrustum
{
    enum VFPLANES
    {
        //! Far plane of the frustum. That is the plane farest away from the eye.
        VF_FAR_PLANE = 0,
        //! Near plane of the frustum. That is the plane nearest to the eye.
        VF_NEAR_PLANE,
        //! Left plane of the frustum.
        VF_LEFT_PLANE,
        //! Right plane of the frustum.
        VF_RIGHT_PLANE,
        //! Bottom plane of the frustum.
        VF_BOTTOM_PLANE,
        //! Top plane of the frustum.
        VF_TOP_PLANE,

        //! Amount of m_Planes enclosing the view frustum. Should be 6.
        VF_PLANE_COUNT
    };
public:
    CFrustum() : m_fNearDist(0), m_fFarDist(0) {}
    CFrustum(const Matrix4f& matProj) : m_ProjMatrix(matProj) {}
    virtual ~CFrustum() {}

    void SetPerspective(float fNear, float fFar, float fov, float fWidth, float fHeight)
    {
#ifdef LEFT_HAND_COORDINATE
        m_ProjMatrix.BuildProjectionMatrixPerspectiveFovLH(fov, fWidth / fHeight, fNear, fFar);
#else 
        m_ProjMatrix.BuildProjectionMatrixPerspectiveFovRH(fov, fWidth / fHeight, fNear, fFar);
#endif
        
        m_fNearDist = fNear;
        m_fFarDist = fFar;
    }

    void SetOrtho(float w, float h, float zNear, float zFar)
    {
#ifdef LEFT_HAND_COORDINATE
        m_ProjMatrix.BuildProjectionMatrixOrthoLH(w, h, zNear, zFar);
#else
        m_ProjMatrix.BuildProjectionMatrixOrthoRH(w, h, zNear, zFar);
#endif
        
    }

    void CalculateFrustum(const Matrix4f& matView)
    {
        m_matView = matView;
        Matrix4f mat = matView * m_ProjMatrix;
        
#ifdef LEFT_HAND_COORDINATE
        
        // left clipping plane
        m_Planes[VF_LEFT_PLANE].Normal.x = mat[3 ] - mat[0];
        m_Planes[VF_LEFT_PLANE].Normal.y = mat[7 ] - mat[4];
        m_Planes[VF_LEFT_PLANE].Normal.z = mat[11] - mat[8];
        m_Planes[VF_LEFT_PLANE].D =        mat[15] - mat[12];

        // right clipping plane
        m_Planes[VF_RIGHT_PLANE].Normal.x = mat[3 ] + mat[0];
        m_Planes[VF_RIGHT_PLANE].Normal.y = mat[7 ] + mat[4];
        m_Planes[VF_RIGHT_PLANE].Normal.z = mat[11] + mat[8];
        m_Planes[VF_RIGHT_PLANE].D =        mat[15] + mat[12];

        // top clipping plane
        m_Planes[VF_TOP_PLANE].Normal.x = mat[3 ] + mat[1];
        m_Planes[VF_TOP_PLANE].Normal.y = mat[7 ] + mat[5];
        m_Planes[VF_TOP_PLANE].Normal.z = mat[11] + mat[9];
        m_Planes[VF_TOP_PLANE].D =        mat[15] + mat[13];

        // bottom clipping plane
        m_Planes[VF_BOTTOM_PLANE].Normal.x = mat[3 ] - mat[1];
        m_Planes[VF_BOTTOM_PLANE].Normal.y = mat[7 ] - mat[5];
        m_Planes[VF_BOTTOM_PLANE].Normal.z = mat[11] - mat[9];
        m_Planes[VF_BOTTOM_PLANE].D =        mat[15] - mat[13];

        // far clipping plane
        m_Planes[VF_FAR_PLANE].Normal.x = mat[3 ] + mat[2];
        m_Planes[VF_FAR_PLANE].Normal.y = mat[7 ] + mat[6];
        m_Planes[VF_FAR_PLANE].Normal.z = mat[11] + mat[10];
        m_Planes[VF_FAR_PLANE].D =        mat[15] + mat[14];

        // near clipping plane
        m_Planes[VF_NEAR_PLANE].Normal.x = mat[3 ] - mat[2];
        m_Planes[VF_NEAR_PLANE].Normal.y = mat[7 ] - mat[6];
        m_Planes[VF_NEAR_PLANE].Normal.z = mat[11] - mat[10];
        m_Planes[VF_NEAR_PLANE].D =        mat[15] - mat[14];
        
        /*
        // left clipping plane
        m_Planes[VF_LEFT_PLANE].Normal.x = mat[3 ] + mat[0];
        m_Planes[VF_LEFT_PLANE].Normal.y = mat[7 ] + mat[4];
        m_Planes[VF_LEFT_PLANE].Normal.z = mat[11] + mat[8];
        m_Planes[VF_LEFT_PLANE].D =        mat[15] + mat[12];

        // right clipping plane
        m_Planes[VF_RIGHT_PLANE].Normal.x = mat[3 ] - mat[0];
        m_Planes[VF_RIGHT_PLANE].Normal.y = mat[7 ] - mat[4];
        m_Planes[VF_RIGHT_PLANE].Normal.z = mat[11] - mat[8];
        m_Planes[VF_RIGHT_PLANE].D =        mat[15] - mat[12];

        // top clipping plane
        m_Planes[VF_TOP_PLANE].Normal.x = mat[3 ] - mat[1];
        m_Planes[VF_TOP_PLANE].Normal.y = mat[7 ] - mat[5];
        m_Planes[VF_TOP_PLANE].Normal.z = mat[11] - mat[9];
        m_Planes[VF_TOP_PLANE].D =        mat[15] - mat[13];

        // bottom clipping plane
        m_Planes[VF_BOTTOM_PLANE].Normal.x = mat[3 ] + mat[1];
        m_Planes[VF_BOTTOM_PLANE].Normal.y = mat[7 ] + mat[5];
        m_Planes[VF_BOTTOM_PLANE].Normal.z = mat[11] + mat[9];
        m_Planes[VF_BOTTOM_PLANE].D =        mat[15] + mat[13];

        // far clipping plane
        m_Planes[VF_FAR_PLANE].Normal.x = mat[3 ] - mat[2];
        m_Planes[VF_FAR_PLANE].Normal.y = mat[7 ] - mat[6];
        m_Planes[VF_FAR_PLANE].Normal.z = mat[11] - mat[10];
        m_Planes[VF_FAR_PLANE].D =        mat[15] - mat[14];

        // near clipping plane
        m_Planes[VF_NEAR_PLANE].Normal.x = mat[3 ];
        m_Planes[VF_NEAR_PLANE].Normal.y = mat[7 ];
        m_Planes[VF_NEAR_PLANE].Normal.z = mat[11];
        m_Planes[VF_NEAR_PLANE].D =        mat[15];
        */
#else
        // left clipping plane
        m_Planes[VF_LEFT_PLANE].Normal.x = mat[3 ] + mat[0];
        m_Planes[VF_LEFT_PLANE].Normal.y = mat[7 ] + mat[4];
        m_Planes[VF_LEFT_PLANE].Normal.z = mat[11] + mat[8];
        m_Planes[VF_LEFT_PLANE].D =        mat[15] + mat[12];

        // right clipping plane
        m_Planes[VF_RIGHT_PLANE].Normal.x = mat[3 ] - mat[0];
        m_Planes[VF_RIGHT_PLANE].Normal.y = mat[7 ] - mat[4];
        m_Planes[VF_RIGHT_PLANE].Normal.z = mat[11] - mat[8];
        m_Planes[VF_RIGHT_PLANE].D =        mat[15] - mat[12];

        // top clipping plane
        m_Planes[VF_TOP_PLANE].Normal.x = mat[3 ] - mat[1];
        m_Planes[VF_TOP_PLANE].Normal.y = mat[7 ] - mat[5];
        m_Planes[VF_TOP_PLANE].Normal.z = mat[11] - mat[9];
        m_Planes[VF_TOP_PLANE].D =        mat[15] - mat[13];

        // bottom clipping plane
        m_Planes[VF_BOTTOM_PLANE].Normal.x = mat[3 ] + mat[1];
        m_Planes[VF_BOTTOM_PLANE].Normal.y = mat[7 ] + mat[5];
        m_Planes[VF_BOTTOM_PLANE].Normal.z = mat[11] + mat[9];
        m_Planes[VF_BOTTOM_PLANE].D =        mat[15] + mat[13];

        // far clipping plane
        m_Planes[VF_FAR_PLANE].Normal.x = mat[3 ] - mat[2];
        m_Planes[VF_FAR_PLANE].Normal.y = mat[7 ] - mat[6];
        m_Planes[VF_FAR_PLANE].Normal.z = mat[11] - mat[10];
        m_Planes[VF_FAR_PLANE].D =        mat[15] - mat[14];

        // near clipping plane
        m_Planes[VF_NEAR_PLANE].Normal.x = mat[2];
        m_Planes[VF_NEAR_PLANE].Normal.y = mat[6];
        m_Planes[VF_NEAR_PLANE].Normal.z = mat[10];
        m_Planes[VF_NEAR_PLANE].D =        mat[14];
#endif
        

        // normalize normals
        u32 i;
        for ( i = 0; i < 6; ++i)
        {
            //const f32 len = AIR::reciprocal_squareroot(
            //    m_Planes[i].Normal.GetLengthSQ());
            const f32 len = 1.0f / m_Planes[i].Normal.GetLength();
            m_Planes[i].Normal *= len;
            m_Planes[i].D *= len;
        }
    }

    // 判断一个3D点是否在平截头体内
    bool IsPointIn(const Vector3Df& vPos) const
    {
        for(int i = 0; i < 6; i++ )
        {
            //D3DXVECTOR3 vNormal(m_Planes[i].Normal.x, m_Planes[i].Normal.y, m_Planes[i].Normal.z);
            if(m_Planes[i].Normal.DotProduct(vPos) + m_Planes[i].D <= 0)
            {
                return false;
            }
        }
        return true;
    }

    // 判断一个3D球是否在平截头体内
    bool IsSphereIn(const Vector3Df& vPos, f32 radius) const
    {
        for(int i = 0; i < 6; i++ )	
        {
            //D3DXVECTOR3 vNormal(m_Planes[i].Normal.x, m_Planes[i].Normal.y, m_Planes[i].Normal.z);
            if(m_Planes[i].Normal.DotProduct(vPos) + m_Planes[i].D <= -radius )
            {
                return false;
            }
        }
        return true;
    }

    // 判断一个包围盒是否在平截头体内
    bool IsAABBIn(const AABBBox3Df& vAABB) const
    {
        /* --------------------------------------------------------
        * 注: 两种方法效率未经分析 ??????
        ---------------------------------------------------------*/

        /* 第一种方法: 判断面与视锥体的相交性
        for(int i = 0; i < 6; i++ )	
        {
        int nRet = vAABB.ClassifyPlane(m_Planes[i]);
        if (nRet < 0) //在平面的背面
        {
        return FALSE;
        }
        }

        return TRUE;
        */

        /*第二种方法: 八个点对一个面, 依次搞6次 */
        int nEight = 0;

        for(int i = 0; i < 6; i++ )
        {
            nEight = 0; //计数还原为0

            if(m_Planes[i].Normal.x * vAABB.MinEdge.x + m_Planes[i].Normal.y * vAABB.MinEdge.y + m_Planes[i].Normal.z * vAABB.MinEdge.z + m_Planes[i].D <= 0)
            {
                nEight++; //在负面
            }

            if(m_Planes[i].Normal.x * vAABB.MaxEdge.x + m_Planes[i].Normal.y * vAABB.MaxEdge.y + m_Planes[i].Normal.z * vAABB.MaxEdge.z + m_Planes[i].D <= 0)
            {
                nEight++; //在负面
            }

            if(m_Planes[i].Normal.x * vAABB.MaxEdge.x + m_Planes[i].Normal.y * vAABB.MinEdge.y + m_Planes[i].Normal.z * vAABB.MinEdge.z + m_Planes[i].D <= 0)
            {
                nEight++; //在负面
            }

            if(m_Planes[i].Normal.x * vAABB.MaxEdge.x + m_Planes[i].Normal.y * vAABB.MinEdge.y + m_Planes[i].Normal.z * vAABB.MaxEdge.z + m_Planes[i].D <= 0)
            {
                nEight++; //在负面
            }

            if(m_Planes[i].Normal.x * vAABB.MaxEdge.x + m_Planes[i].Normal.y * vAABB.MaxEdge.y + m_Planes[i].Normal.z * vAABB.MinEdge.z + m_Planes[i].D <= 0)
            {
                nEight++; //在负面
            }

            if(m_Planes[i].Normal.x * vAABB.MinEdge.x + m_Planes[i].Normal.y * vAABB.MaxEdge.y + m_Planes[i].Normal.z * vAABB.MinEdge.z + m_Planes[i].D <= 0)
            {
                nEight++; //在负面
            }

            if(m_Planes[i].Normal.x * vAABB.MinEdge.x + m_Planes[i].Normal.y * vAABB.MaxEdge.y + m_Planes[i].Normal.z * vAABB.MaxEdge.z + m_Planes[i].D <= 0)
            {
                nEight++; //在负面
            }

            if(m_Planes[i].Normal.x * vAABB.MinEdge.x + m_Planes[i].Normal.y * vAABB.MinEdge.y + m_Planes[i].Normal.z * vAABB.MaxEdge.z + m_Planes[i].D <= 0)
            {
                nEight++; //在负面
            }

            if (nEight == 8) //说明四个点都在一个面的负面, 那就删除它
            {
                return false;
            }


			//if( m_Planes[i].Normal.x * vAABB.MinEdge.x + 
			//	m_Planes[i].Normal.y * vAABB.MinEdge.y + 
			//	m_Planes[i].Normal.z * vAABB.MinEdge.z + 
			//	m_Planes[i].D > 0)
			//{
			//	return TRUE;
			//}

			//if( m_Planes[i].Normal.x * vAABB.MaxEdge.x + 
			//	m_Planes[i].Normal.y * vAABB.MaxEdge.y + 
			//	m_Planes[i].Normal.z * vAABB.MaxEdge.z + 
			//	m_Planes[i].D > 0)
			//{
			//	return TRUE;
			//}

			//if( m_Planes[i].Normal.x * vAABB.MaxEdge.x + 
			//	m_Planes[i].Normal.y * vAABB.MinEdge.y + 
			//	m_Planes[i].Normal.z * vAABB.MinEdge.z + 
			//	m_Planes[i].D > 0)
			//{
			//	return TRUE;
			//}

			//if( m_Planes[i].Normal.x * vAABB.MaxEdge.x + 
			//	m_Planes[i].Normal.y * vAABB.MinEdge.y + 
			//	m_Planes[i].Normal.z * vAABB.MaxEdge.z + 
			//	m_Planes[i].D > 0)
			//{
			//	return TRUE;
			//}

			//if( m_Planes[i].Normal.x * vAABB.MaxEdge.x + 
			//	m_Planes[i].Normal.y * vAABB.MaxEdge.y + 
			//	m_Planes[i].Normal.z * vAABB.MinEdge.z + 
			//	m_Planes[i].D > 0)
			//{
			//	return TRUE;
			//}

			//if( m_Planes[i].Normal.x * vAABB.MinEdge.x + 
			//	m_Planes[i].Normal.y * vAABB.MaxEdge.y + 
			//	m_Planes[i].Normal.z * vAABB.MinEdge.z + 
			//	m_Planes[i].D > 0)
			//{
			//	return TRUE;
			//}

			//if( m_Planes[i].Normal.x * vAABB.MinEdge.x + 
			//	m_Planes[i].Normal.y * vAABB.MaxEdge.y + 
			//	m_Planes[i].Normal.z * vAABB.MaxEdge.z + 
			//	m_Planes[i].D > 0)
			//{
			//	return TRUE;
			//}

			//if( m_Planes[i].Normal.x * vAABB.MinEdge.x + 
			//	m_Planes[i].Normal.y * vAABB.MinEdge.y + 
			//	m_Planes[i].Normal.z * vAABB.MaxEdge.z + 
			//	m_Planes[i].D > 0)
			//{
			//	return TRUE;
			//}

        }
        return true;
		//return FALSE;
    }

    // 判断与长方形是否相交
    bool IsRectangleIn(const Vector3Df &vPos1, const Vector3Df &vPos2, const Vector3Df &vPos3, const Vector3Df &vPos4) const
    {
        int nFour = 0;
        //四个点对一个面, 依次搞6次
        for(int i = 0; i < 6; i++ )
        {
            nFour = 0; //计数还原为0

            if(m_Planes[i].Normal.x * vPos1.x + m_Planes[i].Normal.y * vPos1.y + m_Planes[i].Normal.z * vPos1.z + m_Planes[i].D <= 0)
            {
                nFour++; //在负面
            }

            if(m_Planes[i].Normal.x * vPos2.x + m_Planes[i].Normal.y * vPos2.y + m_Planes[i].Normal.z * vPos2.z + m_Planes[i].D <= 0)
            {
                nFour++; //在负面
            }

            if(m_Planes[i].Normal.x * vPos3.x + m_Planes[i].Normal.y * vPos3.y + m_Planes[i].Normal.z * vPos3.z + m_Planes[i].D <= 0)
            {
                nFour++; //在负面
            }

            if(m_Planes[i].Normal.x * vPos4.x + m_Planes[i].Normal.y * vPos4.y + m_Planes[i].Normal.z * vPos4.z + m_Planes[i].D <= 0)
            {
                nFour++; //在负面
            }

            if (nFour == 4) //说明四个点都在一个面的负面, 那就剃删除它
            {
                return false;
            }
        }
        return true;	
    }

    const Vector3Df* GetWorldSpaceCorners() const
    {
        UpdateWorldCorners();

        return m_WorldSpaceCorners;
    }

    void GetViewSpaceCorners(Vector3Df* pCoords) const;

    f32 GetNear() const
    {
        return m_fNearDist;
    }

    f32 GetFar() const
    {
        return m_fFarDist;
    }

    
   
protected:
    void UpdateWorldCorners() const;

    void CalcProjectionParameters(float& left, float& right, float& bottom, float& top) const;
private:
    Plane3Df m_Planes[6];		// 保存平截头体6个平面

    Matrix4f  m_ProjMatrix;
    Matrix4f  m_matView;

    float     m_fNearDist;
    float     m_fFarDist;

    bool      m_bPerspertive;   // 是否为透视投影

    mutable Vector3Df m_WorldSpaceCorners[8];  //顺序是近截面的左上，右上，左下，右下，远截面的左上，右上，左下，右下
    //f32  m_fNear;
    //f32  m_fFar;
    //f32  m_fFOVY;
    //f32  m_fAspect;

    //D3DXMATRIXA16  m_matProj;
};

