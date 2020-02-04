/********************************************************************
	created:	2010/05/22
	created:	22:5:2010   15:39
	filename: 	BillboardSetManager.h
	author:		liangairan
	
	purpose:	billboardset管理器，相同顶点数的共用一个billboardset
*********************************************************************/

#pragma once 

#include "BillboardSet.h"
#include "Singleton.h"

class EngineExport BillboardSetManager : public Singleton<BillboardSetManager>
{
    BE_SINGLETON(BillboardSetManager);
public:
    ~BillboardSetManager();

    BillboardSet* CreateBillboardSet(int nBillboardsNum);

    //BillboardSet* FindBillboardSet(int nBillboardsNum);

    void ClearBillboardSet(BillboardSet*& pBillboardSet);

    //清空所有billboardset
    void Clear();

    BillboardChainData* CreateBillboardChainData(u32 nChainsNum);

    //BillboardChainData* FindBillboardChainData(u32 nChainsNum);

    void ClearBillboardChainData(BillboardChainData*& pBillboardChainData);
protected:
private:

    //typedef std::map<int, BillboardSet*> BillboardSetList;
    //typedef BillboardSetList::iterator   BillboardSetList_Iter;

    //typedef std::map<u32, BillboardChainData*> BillboardChainDataList;
    //typedef BillboardChainDataList::iterator   BillboardChainDataListIter;
	typedef AIRList<BillboardSet*> BillboardSetList;
	typedef BillboardSetList::iterator   BillboardSetList_Iter;

	typedef AIRList<BillboardChainData*> BillboardChainDataList;
	typedef BillboardChainDataList::iterator   BillboardChainDataListIter;

    BillboardSetList  m_lstBillboardSet;
    BillboardChainDataList m_lstBillboardChain;
};

#define BILLBOARDSET_MGR BillboardSetManager::GetInstance()
