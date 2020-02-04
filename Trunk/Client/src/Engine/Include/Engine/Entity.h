#pragma once
#include "EngineConfig.h"
#include "Common.h"
#include "AABBBox3D.h"
#include "Quaternion.h"
#include "RTTI.h"
#include "EngineDefination.h"

class COctree;
class CCamera;

class EngineExport Entity
{
	EngineAllocator(Entity)
		DECLARE_ROOT_RTTI;
public:
	Entity();
	virtual ~Entity();

	inline const Vector3Df& GetLocalPosition() const;

	inline const Quaternion& GetLocalRotatioin() const;

	inline const Vector3Df& GetLocalScale() const;

	inline void SetLocalPosition(const Vector3Df& position);

	inline void SetLocalRotation(const Quaternion& rotate);

	inline void SetLocalScale(const Vector3Df& scale);

	inline void SetWorldPosition(const Vector3Df& position);

	inline void SetWorldRotation(const Quaternion& rotate);

	inline void SetWorldScale(const Vector3Df& scale);

	Entity* GetParent()
	{
		return m_pParent;
	}

	virtual void Update();

	bool IsActive() const
	{
		return m_bActive;
	}

	void SetActive(bool bActive);

	u32 GetCullFlag() const
	{
		return m_nCullFlag;
	}

	void SetCullFlag(u32 nFlag)
	{
		m_nCullFlag = nFlag;
	}

	const AABBBox3D& GetWorldBox() const
	{
		return m_worldBox;
	}

	COctree* GetOctree() 
	{
		return m_pAttachedTree;
	}

	void SetAttachedTree(COctree* pTree)
	{
		m_pAttachedTree = pTree;
	}

	const AABBBox3D& GetWorldBoundingBox() const
	{
		return m_worldBox;
	}

	void Clear();

	//��䵽��Ⱦ����
	void FillRenderContext(CCamera* pCamera, RenderContextQueue* pQueue, eRenderFlow flow);

	IMaterial* GetMaterial()
	{
		return m_pMaterial;
	}
protected:
	//��Ը��׵�λ�ã�û�и��׾�������λ��
	Vector3Df  m_position;
	//��Ը��׵����ţ�û�и��׾���������������
	Vector3Df  m_scale;
	//��Ը��׵���ת��û�и��׾�����������
	Quaternion m_rotation;

	//�̳����и��׵�λ�ã�������λ��
	Vector3Df m_WorldPosition;

	//�̳����и��׵���ת
	Quaternion m_WorldRotation;

	//�̳����и��׵�����
	Vector3Df m_WorldScale;

	AABBBox3D m_worldBox;


	mutable Matrix4f m_matTransform;   //���յ��������任

	bool m_bNeedUpdate;

	inline void NeedUpdate();

	void UpdateWorldTransform();

	Entity* m_pParent;

	COctree* m_pAttachedTree;   //�����������
	
	bool m_bActive;

	void _GetPrimitive();
private:
	void ResetLocalTransform();

	u32 m_nCullFlag;

	void UpdateAttachedTree();

	IMaterial* m_pMaterial;
};

