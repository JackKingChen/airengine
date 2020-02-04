#pragma once

#include "max.h"
#include <string>

class CMaxNode;
class CMaxMesh;
class CMaxMaterial;

class CMaxInterface
{
protected:
    ExpInterface *m_pExpInterface;
    Interface *m_pInterface;

public:
    CMaxInterface();
    virtual ~CMaxInterface();
    bool Create(ExpInterface *pExpInterface, Interface *pInterface);

    Interface* GetInterface() { return m_pInterface; }

    CMaxNode* GetNode(const std::string& strName);
    int GetSelectedNodeCount();
    CMaxNode* GetSelectedNode(int nodeId);

    CMaxMaterial* GetMaterial(int nMaterialID);

    int GetMaterialCount();

    //创建
    //bool BuildMaxMaterials();

    //返回当前MAX中的时间
    int GetTime();

    void SetTime(int nTime);

	void SetProgressInfo(int percentage);
	void StartProgressInfo(char* szText);
	void StopProgressInfo();

    Matrix3 ConvertToDXMatrix(Matrix3& mat);

    Matrix3 ConvertToYUpMatrix(const Matrix3& mat);

    //判断是否为mirror矩阵
    bool IsMatrixMirrored(const Matrix3& tm);

    //清空数据
    void Clear();

protected:
private:
};
