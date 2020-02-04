/********************************************************************
	created:	2010/10/19
	created:	19:10:2010   11:41
	filename: 	BillboardComposer.h
	author:		liangairan
	
	purpose:	多个billboard的组合，有行和列
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
    int m_nRow;   //行数
    int m_nCol;   //列数

    typedef AIRVector<CBillboard*> BillboardArray;
    typedef BillboardArray::iterator BillboardArrayIter;
    //这个array可能有问题，到底交给谁去释放？
    BillboardArray  m_vtBillboards;
    //BillboardArray  
    
};
