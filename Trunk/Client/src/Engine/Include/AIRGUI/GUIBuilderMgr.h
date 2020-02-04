// ***************************************************************
//  File: GUIBuilderMgr.h
//  Date: 2010-2-12
//  Author: liangairan
//  Description: �ؼ�builder������������е������ؼ����������øù�����
//               ע���Լ��Ŀؼ�
// ***************************************************************


#pragma once
#include "GUIBuilder.h"
#include "Singleton.h"


// �ؼ����ƹ�����
class AIRGuiExport CGUIBuilderMgr : public Singleton<CGUIBuilderMgr>
{

    BE_SINGLETON(CGUIBuilderMgr);
public:
    ~CGUIBuilderMgr();

    // ע��ؼ��Ķ�����
    bool                    Register(const char* szBuilder, CGUIBuilder* pBuilder);

    // ��ȡ�ؼ��Ķ�����
    CGUIBuilder*            GetBuilder(const AIRString& strName);

    // ���
    void                    CleanUp();



    //static void KillSelf();
protected:

    typedef  AIRMap<AIRString, CGUIBuilder*>  BUILDER_MAP;
    typedef  AIRMap<AIRString, CGUIBuilder*>::iterator BUILDER_ITER;

    BUILDER_MAP             m_mapBuilder;
private:
    //static CGUIBuilderMgr*  m_pInstance;
};






//=============================================================
//! [��Ӵ���] - [HJJ] - [2009-4-22] - ��ʼ :
//! [����]: 
#define	UIBUILDERMGR	CGUIBuilderMgr::GetInstance()
//! [��Ӵ���] - [HJJ] - [2009-4-22] - ���� !
//=============================================================