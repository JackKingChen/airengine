/********************************************************************
	created:	2010/10/19
	created:	19:10:2010   11:41
	filename: 	BillboardComposer.h
	author:		liangairan
	
	purpose:	���billboard����ϣ����к���
*********************************************************************/

#pragma once 

#include "Billboard.h"

class EngineExport BillboardComposer : public CBillboard
{
public:
    BillboardComposer();
    virtual ~BillboardComposer();

    virtual void PreRender(CCamera* pCamera);

    virtual void Render(CCamera* pCamera);

    //void SetRow(int nRow);

    //void SetCol(int nCol);

    void SetSize(int nRow, int nCol);

    //virtual void CaculatePosition();
    void AddBillboard(int nRow, int nCol, CBillboard* pBillboard);

    virtual bool Prepare();
protected:
private:
    int m_nRow;   //����
    int m_nCol;   //����

    typedef AIRVector<CBillboard*> BillboardArray;
    typedef BillboardArray::iterator BillboardArrayIter;
    //���array���������⣬���׽���˭ȥ�ͷţ�
    BillboardArray  m_vtBillboards;
    //BillboardArray  
    
};
