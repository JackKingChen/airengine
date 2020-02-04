#include "stdafx.h"
#include "BillboardSetManager.h"

BillboardSetManager::BillboardSetManager()
{

}

BillboardSetManager::~BillboardSetManager()
{

}

BillboardSet* BillboardSetManager::CreateBillboardSet(int nBillboardsNum)
{
    //BillboardSet* pBillboardSet = FindBillboardSet(nBillboardsNum);
    //if (pBillboardSet)
    //{
    //    pBillboardSet->AddRef();
    //    return pBillboardSet;
    //}
    //pBillboardSet = new BillboardSet;
    //if (pBillboardSet->Create(nBillboardsNum))
    //{
    //    m_lstBillboardSet.insert(std::make_pair(nBillboardsNum, pBillboardSet));
    //}
	BillboardSet* pBillboardSet = new BillboardSet;
	if (pBillboardSet->Create(nBillboardsNum))
	{
		m_lstBillboardSet.push_back( pBillboardSet );
	}

    return pBillboardSet;
}

//BillboardSet* BillboardSetManager::FindBillboardSet(int nBillboardsNum)
//{
//    BillboardSetList_Iter it = m_lstBillboardSet.find(nBillboardsNum);
//    if (it != m_lstBillboardSet.end())
//    {
//        return it->second;
//    }
//
//    return NULL;
//}

void BillboardSetManager::ClearBillboardSet(BillboardSet*& pBillboardSet)
{
    //if (pBillboardSet->Release() == 0)
    //{
    //    BillboardSetList_Iter it = m_lstBillboardSet.find(pBillboardSet->GetBillboardsNum());
    //    if (it != m_lstBillboardSet.end())
    //    {
    //        m_lstBillboardSet.erase(it);
    //    }
    //    pBillboardSet->Clear();
    //    SafeDelete(pBillboardSet);
    //}
	if (pBillboardSet->Release() == 0)
	{
		BillboardSetList_Iter it = m_lstBillboardSet.begin(), it_end = m_lstBillboardSet.end();
		for (; it != it_end; ++it )
		{
			if( *it == pBillboardSet )
			{
				m_lstBillboardSet.erase(it);
				break;
			}
		}
		pBillboardSet->Clear();
		SafeDelete(pBillboardSet);
	}
}

void BillboardSetManager::Clear()
{
	BillboardSet* pBillboardSet = NULL;
    BillboardSetList_Iter itEnd = m_lstBillboardSet.end();
    for (BillboardSetList_Iter it = m_lstBillboardSet.begin(); it != itEnd; ++it)
    {
        BillboardSet* pBillboardSet = *it;//it->second;
        pBillboardSet->Clear();
        SafeDelete(pBillboardSet);
    }
    m_lstBillboardSet.clear();

	BillboardChainData* pChainData = NULL;
    BillboardChainDataListIter itEndChain = m_lstBillboardChain.end();
    for (BillboardChainDataListIter it = m_lstBillboardChain.begin(); it != itEndChain; ++it)
    {
        BillboardChainData* pChainData = *it;// it->second;
        pChainData->Clear();
        SafeDelete(pChainData);
    }
}

BillboardChainData* BillboardSetManager::CreateBillboardChainData(u32 nChainsNum)
{
    //BillboardChainData* pBillboardChainData = FindBillboardChainData(nChainsNum);
    //if (pBillboardChainData)
    //{
    //    pBillboardChainData->AddRef();
    //    return pBillboardChainData;
    //}

    //pBillboardChainData = new BillboardChainData;
    //if (pBillboardChainData->Create(nChainsNum))
    //{
    //    m_lstBillboardSet.insert(std::make_pair(nChainsNum, pBillboardChainData));
    //}

	BillboardChainData* pBillboardChainData = new BillboardChainData;
	if (pBillboardChainData->Create(nChainsNum))
	{
		m_lstBillboardSet.push_back( pBillboardChainData );
	}

    return pBillboardChainData;
}

//BillboardChainData* BillboardSetManager::FindBillboardChainData(u32 nChainsNum)
//{
//    BillboardChainDataListIter it = m_lstBillboardChain.find(nChainsNum);
//
//    if (it != m_lstBillboardChain.end())
//    {
//        return it->second;
//    }
//
//    return NULL;
//}

void BillboardSetManager::ClearBillboardChainData(BillboardChainData*& pBillboardChainData)
{
    //if (pBillboardChainData->Release() == 0)
    //{
    //    BillboardSetList_Iter it = m_lstBillboardSet.find(pBillboardChainData->GetChainsNum());
    //    if (it != m_lstBillboardSet.end())
    //    {
    //        m_lstBillboardSet.erase(it);
    //    }
    //    pBillboardChainData->Clear();
    //    SafeDelete(pBillboardChainData);
    //}

	if (pBillboardChainData->Release() == 0)
	{
		BillboardSetList_Iter it = m_lstBillboardSet.begin(), it_end = m_lstBillboardSet.end();
		for (; it != it_end; ++it )
		{
			if( *it == pBillboardChainData )
			{
				m_lstBillboardSet.erase(it);
				break;
			}
		}
		pBillboardChainData->Clear();
		SafeDelete(pBillboardChainData);
	}
}
