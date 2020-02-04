#pragma once
#include "exporter.h"
#include "MaxNode.h"
#include "MaxInterface.h"
#include "AIRMaterial.h"
#include "AIRMesh.h"
#include "resource.h"
#include <vector>

//动画类型
enum MeshAnimationType
{
    eMAT_NoAnimation = 0,                //静态网格
    eMAT_SkeletionAnimation = 1,			//骨骼动画
    eMAT_VertexAnimation = 2,				//顶点动画				
};

enum VertexAnimationMask
{
    eVAM_None = 0, //0的话不导出
    eVAM_Position = 1,
    eVAM_UV = 2,    
    eVAM_Color = 4,
};

class CAIRExporter :
    public CExporter
{
public:
    
    virtual ~CAIRExporter(void);

protected:
    CAIRExporter(void);

public:
    static CAIRExporter* GetInstance()
    {
        if (m_pInterface == NULL)
        {
            m_pInterface = new CAIRExporter;
        }
        return m_pInterface;
    }

    //virtual bool Create(CMaxInterface *pInterface);
    virtual bool ExportAnimation(CAIRSkeleton* pSkeleton, const std::string& strFilename);
    virtual bool ExportMaterial(const std::string& strFilename);
    virtual bool ExportMesh(const std::string& strFilename);

    virtual bool ExportSkeleton(CAIRSkeleton& skeletonOut, const std::string& strFilename);
    virtual bool ExportStaticModel(CAIRSkeleton* pSkeleton, const std::string& strFilename);
    virtual bool Export(const char* szFilename);

    //导出顶点动画
    virtual bool ExportVertexAnimation(const std::string& strFilename);

    virtual void Clear();

    int   GetMaxNodesCount() const;

    CMaxNode* GetNode(int nNodeID);

	CMaxNode* GetNode(const char* szName);

    CAIRMaterial* GetMaterial(int nMaterialID);

    const std::string& GetVersion() const { return m_strVersion; }

    bool IsLeftHand() { return m_bLeftHand; }

    void SetLeftHand(bool bLeft)
    {
        m_bLeftHand = bLeft;
    }

    void SetDlgHWnd(HWND hWnd)
    {
        m_hWndDlg = hWnd;
    }

    void SetStartFrame(int nFrame)
    {
        m_nStartFrame = nFrame;
    }

    void SetEndFrame(int nFrame)
    {
        m_nEndFrame = nFrame;
    }

    int  GetStartFrame() 
    { 
        return m_nStartFrame;
    };

    int  GetEndFrame()
    {
        return m_nEndFrame;
    }

    int GetTotalFrame() const
    {
        return m_nTotalFrame;
    };

    void SetTotalFrame(int nTotalFrame)
    {
        m_nTotalFrame = nTotalFrame;
    }

	bool CreateSelectedSkeleton(const char* szFilename);

	HWND GetDlgHWnd() { return m_hWndDlg; }

    CAIRSkeleton* GetSelectedSkeleton() { return m_pSelectedSkeleton; }

    void SetAnimationName(const char* szName)
    {
        m_strAnimationName = szName;
    }

    std::string GetAnimationName()
    {
        return m_strAnimationName;
    }

    const std::string& GetSelectedSkeletonFile() const
    {
        return m_strSkeletonFile;
    }

    void BuildSkeleton(CAIRSkeleton& skeletonOut);

    void SetUpY(bool bUpY);

    void InitExportConfig();

    Matrix3 GetFinalMatrix() const
    {
        return m_matFinal;
    }

    bool IsYUp() const 
    {
        return m_bYIsUp;
    }

    bool IsExportMaterial() const
    {
        return m_bExportMaterial;
    }

    void SetExportMaterial(bool bExportMaterial)
    {
        m_bExportMaterial = bExportMaterial;
    }

    std::string GetMaterialFileName() const
    {
        return m_strMaterialFile;
    }

    void SetMaterialFileName(const char* szFilename)
    {
        m_strMaterialFile = szFilename;
    }

    void AddExportMaterials(CAIRMaterial* pMaterial)
    {
        m_vectorMaterials.push_back(pMaterial);
    }

    static void KillSelf()
    {
        if (m_pInterface)
        {
            delete m_pInterface;
            m_pInterface = NULL;
        }
    }

    bool IsExportSelected()
    {
        return m_bExportSelected;
    }

    void SetExportSelected(bool bExportSelected)
    {
        m_bExportSelected = bExportSelected;
    }

    void SetKeyframeFile(const char* szFilename)
    {
        m_strKeyFrameFile = szFilename;
    }

    const std::string& GetKeyframeFile() const
    {
        return m_strKeyFrameFile;
    }

    void SetSaveAsBinary(bool bBinary)
    {
        m_bSaveAsBinary = bBinary;
    }

    CAIRMesh*  GetExportMesh(const std::string& strName);

    bool IsShareSkeleton() const
    {
        return m_bShareSkeleton;
    }

    void SetShareSkeleton(bool bShare)
    {
        m_bShareSkeleton = bShare;
    }

    /*
    void SetExportVertexAnimation(bool bExport)
    {
        m_bExportVertexAnimation = bExport;
    }

    bool IsExportVertexAnimation() const
    {
        return m_bExportVertexAnimation;
    }
    */

    void SetVertexAniExportMask(VertexAnimationMask nMask, bool bUse)
    {
        if (bUse)
        {
            m_nVertexAnimationMask |= nMask;
        }
        else
        {
            m_nVertexAnimationMask ^= nMask;
        }
    }

    void SetMeshAnimationType(MeshAnimationType type, bool bUse)
    {
        if (bUse)
        {
            m_nAnimationsMark |= type;
        }
        else
        {
            m_nAnimationsMark ^= type;
        }
    }

    BYTE GetMeshAnimationType() const
    {
        return m_nAnimationsMark;
    }
private:
    //拆分结点，放到m_vtMaxNodes中
    void ExtractAllNodes(CMaxNode* pParent, INode* pNode);

    //void BuildMaxMeshesFromNodes();

    void BuildSkeletonsFromNodes();

    void BuildMaterials();

    void BuildMaterial(Mtl* pMat);

    //清空材质
    void ClearMaterials();

    //保存mesh到文件
    bool SaveMeshToFile(const char* szFilename);

    bool SaveMeshToFileBinary(const char* szFilename);

    //保存
    bool SaveVertexAnimation(const char* szFilename);

    bool SaveVertexAnimationBin(const char* szFilename);

    bool BuildMeshVertexAnimation();

    //CMaxInterface* m_MaxInterface;
    std::string    m_strVersion;
    std::string    m_strCoord;     //坐标系
    std::string    m_strUpVec;     //指向模型上方的向量
    static CAIRExporter* m_pInterface;

    std::vector<CMaxNode*> m_vtMaxNodes;
    //std::vector<CMaxMesh*> m_vtMaxMeshes;  

    std::vector<CAIRMaterial*> m_vectorMaterials;
    std::vector<CAIRMesh*>     m_vtExportMeshes;

	HWND  m_hWndDlg;
    bool  m_bLeftHand;

    int   m_nStartFrame;    //动画的起始帧
    int   m_nEndFrame;      //动画的结束帧

    int   m_nTotalFrame;    //总帧数

	CAIRSkeleton*  m_pSelectedSkeleton;    //已经选择的骨骼动画

    std::string m_strAnimationName;
    std::string m_strSkeletonFile;     //骨骼文件名
    std::string m_strMaterialFile;
    std::string m_strKeyFrameFile;     //顶点动画关键帧文件名

    bool        m_bYIsUp;          //向上的轴，在max中默认是Z
    bool        m_bExportMaterial;    //是否要导出材质
    bool        m_bExportSelected;    //是否只导出选择的模型
    Matrix3     m_matFinal;     //顶点要X的矩阵，根据向上的轴和左右手坐标系，求得最终的矩阵

    bool        m_bSaveAsBinary;
    bool        m_bShareSkeleton;   //是否导出共享骨骼动画
    //bool        m_bExportVertexAnimation;    //是否导出顶点动画

    int         m_nVertexAnimationMask;      //导出顶点动画的mask
    BYTE        m_nAnimationsMark;           //mesh包含的顶点类型
    bool        m_bExportStaticMesh;
    //CMaxNode  m_rootNode;
};

#define AIREXPORTER CAIRExporter::GetInstance()
