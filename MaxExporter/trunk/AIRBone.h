/********************************************************************
	created:	2009/10/13
	created:	13:10:2009   13:45
	filename: 	d:\My Codes\MyExport\AIRBone.h
	file path:	d:\My Codes\MyExport
	file base:	AIRBone
	file ext:	h
	author:		liangairan
	
	purpose:	骨类，存放初始帧的数据
*********************************************************************/
#pragma once

//#include "MaxNode.h"
#include "max.h"
#include <string>
class CMaxNode;

class CAIRBone
{
public:
    CAIRBone(void);

	CAIRBone(int nID, int nParentID, const char* szName, const Point3& pos, const Quat& rotation);
    virtual ~CAIRBone(void);

    bool Create(CMaxNode* pNode, int nParentId, int nID);

    int GetID() const { return m_nID; }

    int GetParentID() const { return m_nParentID; }

    std::string GetName() const { return m_strName; }

    Point3 GetPosition() const { return m_Position; }

    Quat   GetQuat() const { return m_Quat; }

    Point3 GetAbsolutePosition() const { return m_PosAbsolute; }

    Quat   GetAbsoluteRotation() const { return m_QuatAbsolute; }

    CMaxNode* GetNode() const { return m_pNode; }

	void SetPosition(const Point3& pos);

	void SetRotation(const Quat& rotation);

    static int s_nNewCount;

    Point3 GetPos2Parent() const
    {
        return m_Pos2Parent;
    }

    Quat   GetRotation2Parent() const
    {
        return m_Quat2Parent;
    }

protected:
    virtual void GetTranformToParent(CMaxNode* pNode);

private:
    int m_nID;
    int m_nParentID;
    CMaxNode* m_pNode;

    Point3  m_Position;    //初始帧位置     相对MAX中的坐标空间的位置
    Quat    m_Quat;        //初始帧导出的四元数  相对MAX中的坐标空间的旋转

    Point3  m_PosAbsolute;   //绝对位置
    Quat    m_QuatAbsolute;  //绝对位置

    Point3  m_Pos2Parent;    //相对父亲的位置
    Quat    m_Quat2Parent;   //相对父亲的旋转
    std::string m_strName;

    bool    m_bMirrored;      //是否为镜像骨
};
