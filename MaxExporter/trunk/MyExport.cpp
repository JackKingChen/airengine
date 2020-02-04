/**********************************************************************
 *<
	FILE: MyExport.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2003, All Rights Reserved.
 **********************************************************************/


#include "MyExport.h"
#include "stdmat.h"
#include "AirStaticMeshFile.h"
#include "MaxInterface.h"
#include "AIRExporter.h"

//#include "Math/Matrix4.h"
//#include "Math/Vector3D.h"

#define MyExport_CLASS_ID	Class_ID(0xb45d8133, 0x87980f83)

DWORD WINAPI fn(LPVOID arg)
{
    return(0);
}

bool g_bExport = true;

const char* GetFilename(const char* szFilename)
{
    const char* p = strrchr(szFilename, '\\');
    if (p == NULL)
    {
        p = strrchr(szFilename, '/');
    }

    if (p != NULL)
    {
        return p + 1;
    }

    return szFilename;
}

class MyExport : public SceneExport 
{
	public:
		
		static HWND hParams;
		
		int				ExtCount();					// Number of extensions supported
		const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
		const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
		const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
		const TCHAR *	AuthorName();				// ASCII Author name
		const TCHAR *	CopyrightMessage();			// ASCII Copyright message
		const TCHAR *	OtherMessage1();			// Other message #1
		const TCHAR *	OtherMessage2();			// Other message #2
		unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
		void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box

		BOOL SupportsOptions(int ext, DWORD options);
		int				DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);

		//Constructor/Destructor
		MyExport();
		~MyExport();		

        //设置左手坐标
        void SetLHCoord(bool bLH);

		//界面相关
        void OnInitDialog(HWND hWnd);

		void OnChooseSkeletonFile();

        bool OnBtnExportAnimation();

        bool OnBtnExport();

        void SetExportAnimateOnly(bool bExportAni);

        void SetExportStaticMesh(bool bExportStatic);

        void SetExportVertexAnimation(bool bExportVertexAnimation)
        {
            m_bExportVertexAnimation = bExportVertexAnimation;
        }

        bool IsExportAnimateOnly() { return m_bExportAnimateOnly; }

        bool IsExportStaticMesh() { return m_bExportStaticMesh; }

        bool IsExportVertexAnimation() 
        {
            return m_bExportVertexAnimation;
        }

        void SetExportSkeletalAnimation(bool bExportSkelAnim)
        {
            m_bExportSkeletalAnimation = bExportSkelAnim;
        }

        bool IsExportSkeletalAnimation() const
        {
            return m_bExportSkeletalAnimation;
        }

        void SetFileInputName(const char* szName)
        {
            m_strFileInput = szName;
        }

        const std::string& GetFileInputName() const
        {
            return m_strFileInput;
        }
private:
    void ClearMeshData();
    int ExportGlobalInfo();
    bool ExportGeomObject(INode* node, int indentLevel);
    //bool ExportMesh(INode* pNode, int nFrame);

    //导出材质
    bool ExportMaterial(INode* pNode, CAIRStaticMesh& mesh);

    bool ExportMaterial(INode* pNode, Mtl* pMtl, CAIRStaticMesh& mesh);

    bool NodeEnum(INode* pNode);

    BOOL	TMNegParity(Matrix3 &m);

    //对单个结点预处理
    void PreProcess(INode* pNode);
    
    vector<Mtl*>  m_vtMtl;    //材质列表

private:
    FILE*      m_fStream;
    Interface* m_ip;
    //std::vector<LPAIRMESH>  m_myMeshes;
    //LPAIRMESH    m_myMeshes;
    //CAIRStaticMeshFile  m_meshData;
    int        m_nMeshCount;
    int        m_nTotalNodes;
    int        m_nCurNode;
    int        m_nMaterialCount;    //材质数量
    const static     int m_nVersion = 1;

    bool      m_bSelected;
    bool      m_bLHCoord;   //转换成左手坐标的标志
    bool      m_bExportAnimateOnly;    //只导出骨骼动画数据
    bool      m_bExportStaticMesh;     //导出静态模型
    bool      m_bExportVertexAnimation;   //导出顶点动画
    bool      m_bExportSkeletalAnimation;   //导现骨骼动画

    std::string m_strFileInput;     //外部输入的文件名
};



class MyExportClassDesc : public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new MyExport(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID() { return MyExport_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("MyExport"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }					// returns owning module handle
	

};

static MyExportClassDesc MyExportDesc;
ClassDesc2* GetMyExportDesc() { return &MyExportDesc; }





INT_PTR CALLBACK MyExportOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static MyExport *imp = NULL;

    char szAnimationName[256] = { 0 };
    char szMaterialFileName[256] = { 0 };
	switch(message) {
		case WM_INITDIALOG:
			imp = (MyExport *)lParam;
			//CenterWindow(hWnd,GetParent(hWnd));
			imp->OnInitDialog(hWnd);
			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
            g_bExport = false;
			return 0;

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
            case IDC_BTN_EXPORT:
                if (!imp->OnBtnExport())
                {
                    return FALSE;
                }
                EndDialog(hWnd, 0);
                return TRUE;

            case IDC_BTN_EXPORT_ANIMATION:
                AIREXPORTER->SetLeftHand(Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_LEFTHAND)));
                AIREXPORTER->SetUpY(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_YUP)));
                AIREXPORTER->SetStartFrame(GetDlgItemInt(hWnd, IDC_EDIT_StartFrame, NULL, FALSE));
                AIREXPORTER->SetEndFrame(GetDlgItemInt(hWnd, IDC_EDIT_EndFrame, NULL, FALSE));
                if (!imp->OnBtnExportAnimation())
                {
                    return TRUE;
                }
                imp->SetExportStaticMesh(false);
                imp->SetExportVertexAnimation(false);
                imp->SetExportAnimateOnly(true);
                EndDialog(hWnd, 0);
                break;

            case IDC_BTN_EXPORT_STATICMESH:
                imp->SetExportStaticMesh(true);
                imp->SetExportVertexAnimation(false);
                AIREXPORTER->SetLeftHand(Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_LEFTHAND)));
                AIREXPORTER->SetUpY(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_YUP)));
                AIREXPORTER->SetExportMaterial(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_EXPORT_MATERIAL)));
                if (AIREXPORTER->IsExportMaterial())
                {
                    GetDlgItemText(hWnd, IDC_EDT_MATERIAL, szMaterialFileName, 256);
                    AIREXPORTER->SetMaterialFileName(szMaterialFileName);
                    if (strlen(szMaterialFileName) == 0)
                    {
                        MessageBox(hWnd, "请输入要导出的材质文件名！", "提示", MB_OK);
                        return TRUE;
                    }
                }
                if (AIREXPORTER->IsShareSkeleton())
                {
                    GetDlgItemText(hWnd, IDC_EDIT_SKELETAL_FILE, szMaterialFileName, 256);
                    if (strlen(szMaterialFileName) == 0)
                    {
                        MessageBox(hWnd, "请输选择骨骼动画文件！", "提示", MB_OK);
                        return TRUE;
                    }
                }
                AIREXPORTER->SetStartFrame(GetDlgItemInt(hWnd, IDC_EDIT_StartFrame, NULL, FALSE));
                AIREXPORTER->SetEndFrame(GetDlgItemInt(hWnd, IDC_EDIT_EndFrame, NULL, FALSE));
                EndDialog(hWnd, 0);
                break;

            case IDC_BTN_EXPORT_VERTEXANIMATION:
                //导出顶点动画
                imp->SetExportVertexAnimation(true);
                imp->SetExportStaticMesh(false);
                imp->SetExportAnimateOnly(false);
                //imp->SetE
                AIREXPORTER->SetLeftHand(Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_LEFTHAND)));
                AIREXPORTER->SetUpY(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_YUP)));
                AIREXPORTER->SetExportMaterial(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_EXPORT_MATERIAL)));
                if (AIREXPORTER->IsExportMaterial())
                {
                    GetDlgItemText(hWnd, IDC_EDT_MATERIAL, szMaterialFileName, 256);
                    AIREXPORTER->SetMaterialFileName(szMaterialFileName);
                    if (strlen(szMaterialFileName) == 0)
                    {
                        MessageBox(hWnd, "请输入要导出的材质文件名！", "提示", MB_OK);
                        return TRUE;
                    }
                }
                AIREXPORTER->SetStartFrame(GetDlgItemInt(hWnd, IDC_EDIT_StartFrame, NULL, FALSE));
                AIREXPORTER->SetEndFrame(GetDlgItemInt(hWnd, IDC_EDIT_EndFrame, NULL, FALSE));

                GetDlgItemText(hWnd, IDC_EDIT_ANIMATION_NAME, szAnimationName, 256);
                if (strlen(szAnimationName) == 0)
                {
                    MessageBox(hWnd, "请输入动画的名字！", "Warning", MB_OK);
                    return TRUE;
                }
                AIREXPORTER->SetAnimationName(szAnimationName);
                EndDialog(hWnd, 0);
                break;

            case IDC_CHK_LEFTHAND:
                AIREXPORTER->SetLeftHand(Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_LEFTHAND)));
                //imp->SetLHCoord(Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_LEFTHAND)));
                //int a = 0;
                break;

            case IDC_CHECK_YUP:
                AIREXPORTER->SetUpY(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_YUP)));
                break;

            case IDC_CHECK_EXPORT_MATERIAL:
                {
                    HWND hEdit = GetDlgItem(hWnd, IDC_EDT_MATERIAL);

                    //DWORD dwStyle = GetWindowLong(hEdit, GWL_STYLE);

                    if (Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_EXPORT_MATERIAL)))
                    {
                        ::SendMessage(hEdit, EM_SETREADONLY, FALSE, 0);
                    }
                    else
                    {
                        ::SendMessage(hEdit, EM_SETREADONLY, TRUE, 0);
                    }
                    
                    //SetWindowLong(hEdit, GWL_EXSTYLE, dwStyle);
                    
                }
                break;

            case IDC_CHK_SAVEASBINARY:
                AIREXPORTER->SetSaveAsBinary(Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_SAVEASBINARY)));
                break;

            case IDC_CHK_SHARE_SKELETON:
                AIREXPORTER->SetShareSkeleton(Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_SHARE_SKELETON)));
                break;

            case IDC_EDIT_StartFrame:
                if (HIWORD(wParam) == WM_CHAR)
                {
                    AIREXPORTER->SetStartFrame(GetDlgItemInt(hWnd, IDC_EDIT_StartFrame, NULL, FALSE));
                }
                break;

			case IDC_BUTTON_CHOOSE_FILE:
				imp->OnChooseSkeletonFile();
				break;

            case IDC_CHK_EXPORT_VERTEXANIMATION:
                imp->SetExportVertexAnimation(Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_EXPORT_VERTEXANIMATION)));
                AIREXPORTER->SetMeshAnimationType(eMAT_VertexAnimation, Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_EXPORT_VERTEXANIMATION)));
                break;
            case IDC_CHK_POSITION:
                AIREXPORTER->SetVertexAniExportMask(eVAM_Position, Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_POSITION)));
                break;
            case IDC_CHK_UV:
                AIREXPORTER->SetVertexAniExportMask(eVAM_UV, Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_UV)));
                break;
            case IDC_CHK_COLOR:
                AIREXPORTER->SetVertexAniExportMask(eVAM_Color, Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_COLOR)));
                break;
            case IDC_CHK_EXPORT_SKELETAL_ANIMATION:
                imp->SetExportSkeletalAnimation(Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_EXPORT_SKELETAL_ANIMATION)));
                AIREXPORTER->SetMeshAnimationType(eMAT_SkeletionAnimation, Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_EXPORT_SKELETAL_ANIMATION)));
                break;
            }
            break;
        case BM_GETCHECK:
            if (LOWORD(wParam) == IDC_CHK_LEFTHAND)
            {
                AIREXPORTER->SetLeftHand(true);
            }
            else if (LOWORD(wParam) == IDC_CHECK_EXPORT_MATERIAL)
            {
                HWND hEdit = GetDlgItem(hWnd, IDC_EDT_MATERIAL);
                DWORD dwStyle = GetWindowLong(hEdit, GWL_STYLE);
                dwStyle &= ~ES_READONLY;
                SetWindowLong(hEdit, GWL_STYLE, dwStyle);
                AIREXPORTER->SetExportMaterial(true);
            }
            break;

        case WM_CHAR:
            break;
	}
	return 0;
}


//--- MyExport -------------------------------------------------------
MyExport::MyExport() : m_bExportSkeletalAnimation(false)
{
    m_nMeshCount = 0;
    m_nTotalNodes = 0;
    m_nCurNode = 0;
    m_nMaterialCount = 0;
    m_bSelected = false;
    m_bLHCoord = false;
    m_bExportAnimateOnly = false;
    m_bExportStaticMesh = false;
    m_bExportVertexAnimation = false;
    //memset(&m_myMesh, 0, sizeof(AIRMESH));
}

MyExport::~MyExport() 
{

}

int MyExport::ExtCount()
{
	//TODO: Returns the number of file name extensions supported by the plug-in.
	return 2;
}

const TCHAR *MyExport::Ext(int n)
{		
	//TODO: Return the 'i-th' file name extension (i.e. "3DS").
    switch (n)
    {
    case 0:
        return _T("airmesh");
    case 1:
        return _T("airskel");
    }
	return _T("airmesh");
}

const TCHAR *MyExport::LongDesc()
{
	//TODO: Return long ASCII description (i.e. "Targa 2.0 Image File")
	return _T("AIR 1.0 file");
}
	
const TCHAR *MyExport::ShortDesc() 
{			
	//TODO: Return short ASCII description (i.e. "Targa")
	return _T("AIR Mesh File");
}

const TCHAR *MyExport::AuthorName()
{			
	//TODO: Return ASCII Author name
	return _T("Liangairan");
}

const TCHAR *MyExport::CopyrightMessage() 
{	
	// Return ASCII Copyright message
	return _T("");
}

const TCHAR *MyExport::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *MyExport::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int MyExport::Version()
{				
	//TODO: Return Version number * 100 (i.e. v3.01 = 301)
	return 100;
}

void MyExport::ShowAbout(HWND hWnd)
{			
	// Optional
}

BOOL MyExport::SupportsOptions(int ext, DWORD options)
{
	// TODO Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}


int	MyExport::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	//TODO: Implement the actual file Export here and 
	//		return TRUE If the file is exported properly
    //先初始化了接口
	//CMaxInterface Interface;
	//Interface.Create(ei, i);
	AIREXPORTER->Create(ei, i, GetActiveWindow());

    m_strFileInput = GetFilename(name);

	if(!suppressPrompts)
		DialogBoxParam(hInstance, 
				MAKEINTRESOURCE(IDD_PANEL), 
				GetActiveWindow(), 
				MyExportOptionsDlgProc, (LPARAM)this);

    if (!g_bExport)
    {
        AIREXPORTER->Clear();
        CAIRExporter::KillSelf();
        return 1;
    }

    AIREXPORTER->SetExportSelected(options & SCENE_EXPORT_SELECTED);

    m_ip = i;

    AllocConsole();   //创建console

    //m_ip->ProgressStart(GetString(IDS_PROGRESS), TRUE, fn, NULL);
    bool bResult = false;

    AIREXPORTER->InitExportConfig();

    const char* szSkeleton = name;
    const char* szExt = strstr(szSkeleton, ".");

    char szSkeletonFile[256] = { 0 };
    char szMeshFile[256] = { 0 };

    AIREXPORTER->SetKeyframeFile("");


    if (m_bExportAnimateOnly)
    {
        CAIRSkeleton skeletonExport;
        bResult = AIREXPORTER->ExportAnimation(AIREXPORTER->GetSelectedSkeleton(), AIREXPORTER->GetSelectedSkeletonFile().c_str());
    }
    else
    {
        bool bExportMesh = false;
        if (m_bExportSkeletalAnimation)
        {
            strncpy(szSkeletonFile, szSkeleton, szExt - szSkeleton);
            strcat(szSkeletonFile, ".airskel");

            CAIRSkeleton skeletonExport;
            bResult = AIREXPORTER->ExportSkeleton(skeletonExport, szSkeletonFile);

            strncpy(szMeshFile, szSkeleton, szExt - szSkeleton);
            strcat(szMeshFile, ".airmesh");
            bResult = AIREXPORTER->ExportStaticModel(&skeletonExport, szMeshFile);
            bExportMesh = true;

            //下面开始导出动画 
            //strncpy(szAnimationFile, szSkeleton, szExt - szSkeleton);
            //strcat(szAnimationFile, ".airanim");
            bResult = AIREXPORTER->ExportAnimation(&skeletonExport, szSkeletonFile);
        }

        if (m_bExportVertexAnimation)
        {
            char szKeyframefile[256] = { 0 };
            strncpy(szKeyframefile, szSkeleton, szExt - szSkeleton);
            strcat(szKeyframefile, ".airkey");
            AIREXPORTER->SetKeyframeFile(szKeyframefile);

            if (!bExportMesh)
            {
                strncpy(szMeshFile, szSkeleton, szExt - szSkeleton);
                strcat(szMeshFile, ".airmesh");
                bResult = AIREXPORTER->ExportStaticModel(NULL, szMeshFile);
                bExportMesh = true;
            }
            bResult = AIREXPORTER->ExportVertexAnimation(szKeyframefile);
        }

        if (!bExportMesh)
        {
            strncpy(szMeshFile, szSkeleton, szExt - szSkeleton);
            strcat(szMeshFile, ".airmesh");
            bResult = AIREXPORTER->ExportStaticModel(NULL, szMeshFile);
        }
        
    }

    //下面是旧的写法
    /*
    if (m_bExportStaticMesh)
    {
        strncpy(szMeshFile, szSkeleton, szExt - szSkeleton);
        strcat(szMeshFile, ".airmesh");
        bResult = AIREXPORTER->ExportStaticModel(NULL, szMeshFile);
    }
    if (!m_bExportAnimateOnly && !m_bExportStaticMesh && !m_bExportVertexAnimation)
    {
        
        //char szAnimationFile[256] = { 0 };
        strncpy(szSkeletonFile, szSkeleton, szExt - szSkeleton);
        strcat(szSkeletonFile, ".airskel");

        CAIRSkeleton skeletonExport;
        bResult = AIREXPORTER->ExportSkeleton(skeletonExport, szSkeletonFile);

        strncpy(szMeshFile, szSkeleton, szExt - szSkeleton);
        strcat(szMeshFile, ".airmesh");
        bResult = AIREXPORTER->ExportStaticModel(&skeletonExport, szMeshFile);

        //下面开始导出动画 
        //strncpy(szAnimationFile, szSkeleton, szExt - szSkeleton);
        //strcat(szAnimationFile, ".airanim");
        bResult = AIREXPORTER->ExportAnimation(&skeletonExport, szSkeletonFile);

        if (AIREXPORTER->IsExportVertexAnimation())
        {
            char szKeyframefile[256] = { 0 };
            strncpy(szKeyframefile, szSkeleton, szExt - szSkeleton);
            strcat(szKeyframefile, ".airkey");
            AIREXPORTER->SetKeyframeFile(szKeyframefile);
            bResult = AIREXPORTER->ExportVertexAnimation(szKeyframefile);
        }

        skeletonExport.Clear();
    }
    else if (m_bExportAnimateOnly)
    {
        CAIRSkeleton skeletonExport;
        bResult = AIREXPORTER->ExportAnimation(AIREXPORTER->GetSelectedSkeleton(), AIREXPORTER->GetSelectedSkeletonFile().c_str());
    }
    else if (m_bExportVertexAnimation)
    {
        char szKeyframefile[256] = { 0 };
        strncpy(szKeyframefile, szSkeleton, szExt - szSkeleton);
        strcat(szKeyframefile, ".airkey");
        AIREXPORTER->SetKeyframeFile(szKeyframefile);
        bResult = AIREXPORTER->ExportVertexAnimation(szSkeleton);
    }
    */

    //m_fStream = fopen(name, "wt");

    AIREXPORTER->Clear();


    //m_ip->ProgressEnd();

    cprintf("now the max_node count is:%d\n", CMaxMaterial::s_nNewCount);
    cprintf("now the max_mesh count is:%d\n", CMaxMesh::s_nNewCount);
    cprintf("now the max_material count is:%d\n", CMaxMaterial::s_nNewCount);
    cprintf("now the airbones count is:%d\n", CAIRBone::s_nNewCount);

    if (bResult)
    {
        MessageBox(NULL, "导出模型成功！", "提示", MB_OK);
    }
    else
    {
        MessageBox(NULL, "导出失败！请检查！", "提示", MB_OK);
    }
    
    FreeConsole();

    CAIRExporter::KillSelf();
	return 1;
}

void MyExport::ClearMeshData()
{
    /*
    for (int i = 0; i < m_myMeshes.size(); i++)
    {
        LPAIRMESH pMesh = m_myMeshes[i];
        if (pMesh->nVertices > 0 && pMesh->pVertices != NULL)
        {
            delete [] pMesh->pVertices;
            pMesh->pVertices = NULL;
        }

        if (pMesh->nTriangles > 0 && pMesh->pTriangles != NULL)
        {
            delete [] pMesh->pTriangles;
            pMesh->pTriangles = NULL;
        }
    }
    */
    
    //memset(&m_myMesh, 0, sizeof(AIRMESH));
}

int MyExport::ExportGlobalInfo()
{
    fprintf(m_fStream, "%s %d\n", GetString(IDS_VERSION), m_nVersion);
    _cprintf("%s %d\n", GetString(IDS_VERSION), m_nVersion);

    //fprintf(m_fStream, "%s %d", NUM_MESH, );
    return 0;
}

bool MyExport::ExportGeomObject(INode* node, int indentLevel)
{
    ObjectState os = node->EvalWorldState(0);   //暂时定为0
    if (!os.obj)
        return 0;

    // Targets are actually geomobjects, but we will export them
    // from the camera and light objects, so we skip them here.
    if (os.obj->ClassID() == Class_ID(TARGET_CLASS_ID, 0))
        return 0;

    return true;
}
/*
bool MyExport::ExportMesh(INode* pNode, int nFrame)
{
    _cprintf("ExportMesh......\n");
    

    ObjectState os = pNode->EvalWorldState(nFrame);
    if (!os.obj || os.obj->SuperClassID() != GEOMOBJECT_CLASS_ID) 
    {
        return false; // Safety net. This shouldn't happen.
    }

    Mtl* pNodeMtl = pNode->GetMtl();
    Matrix3 tm = pNode->GetNodeTM(nFrame);//pNode->GetObjTMAfterWSM(nFrame);

    INode* pParent = pNode->GetParentNode();

    //Matrix3 worldMatrix = pNode->GetNodeTM(nFrame);
    //if (pParent)
    //{
    //    worldMatrix *= pParent->GetNodeTM(nFrame);
    //}

    BOOL negScale = TMNegParity(tm);
    int vx1, vx2, vx3;

    if (negScale) 
    {
        vx1 = 2;
        vx2 = 1;
        vx3 = 0;
    }
    else 
    {
        vx1 = 0;
        vx2 = 1;
        vx3 = 2;
    }

    BOOL needDel;

    TriObject* tri = NULL;
    Object *obj = pNode->EvalWorldState(nFrame).obj;
    if (obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0))) 
    { 
        tri = (TriObject *) obj->ConvertToType(nFrame, 
            Class_ID(TRIOBJ_CLASS_ID, 0));
        // Note that the TriObject should only be deleted
        // if the pointer to it is not equal to the object
        // pointer that called ConvertToType()
        if (obj != tri) 
        {
            //deleteIt = TRUE;
        }
        //return tri;
    }
    //TriObject* tri = GetTriObjectFromNode(pNode, t, needDel);
    if (!tri) 
    {
        return false;
    }

    _cprintf("Triangle Object name:%s\n", tri->GetObjectName());

    Mesh* mesh = &tri->GetMesh();

    m_nMeshCount++;
    
    //mesh->getNumMtl();

    //fprintf(m_fStream, "%s {\n", MESH_HEADER);
    _cprintf("mesh vertices count:%d\n", mesh->getNumVerts());
    _cprintf("mesh face count:%d\n", mesh->getNumFaces());

    CAIRStaticMesh airMesh;

    airMesh.SetName(pNode->GetName());

    ExportMaterial(pNode, airMesh);

    

    Box3 boundingbox = mesh->getBoundingBox();

    boundingbox = boundingbox * tm;
    
    AIRBOUNDINGBOX box;

    if (m_bLHCoord)
    {
        //AIR::Vector3Df vmax(boundingbox.Max().x, boundingbox.Max().y, boundingbox.Max().z);
        //AIR::Vector3Df vmin(boundingbox.Min().x, boundingbox.Min().y, boundingbox.Min().z);
        //mat.TransformCoord(vmax);
        //mat.TransformCoord(vmin);
        //box.maxX = vmax.x;
        //box.maxY = vmax.y;
        //box.maxZ = vmax.z;
        //box.minX = vmin.x;
        //box.minY = vmin.y;
        //box.minZ = vmin.z;
    }
    else
    {
        box.maxX = boundingbox.Max().x;
        box.maxY = boundingbox.Max().y;
        box.maxZ = boundingbox.Max().z;
        box.minX = boundingbox.Min().x;
        box.minY = boundingbox.Min().y;
        box.minZ = boundingbox.Min().z;
    }
    
    airMesh.SetBoundingBox(box);

    //m_meshData.AddMesh(airMesh);

    //_cprintf("Mesh name:%s\n", mesh->g);
    //fprintf(m_fStream, "}\n")
    return 0;
}
*/

bool MyExport::NodeEnum(INode* pNode)
{
    m_nCurNode++;
    m_ip->ProgressUpdate((int)((float)m_nCurNode / m_nTotalNodes * 100.0f)); 

    _cprintf("NodeEnum, node index:%d, node name:%s\n", m_nCurNode, pNode->GetName());

    ObjectState os = pNode->EvalWorldState(0); 

    // The obj member of ObjectState is the actual object we will export.
    if (os.obj) {

        // We look at the super class ID to determine the type of the object.
        switch(os.obj->SuperClassID()) 
        {
            case GEOMOBJECT_CLASS_ID: 
                //ExportMesh(pNode, 0);
                //if (GetIncludeObjGeom()) ExportGeomObject(node, indentLevel); 
                break;
            case CAMERA_CLASS_ID:
                //if (GetIncludeObjCamera()) ExportCameraObject(node, indentLevel); 
                break;
            case LIGHT_CLASS_ID:
                //if (GetIncludeObjLight()) ExportLightObject(node, indentLevel); 
                break;
            case SHAPE_CLASS_ID:
                //if (GetIncludeObjShape()) ExportShapeObject(node, indentLevel); 
                break;
            case HELPER_CLASS_ID:
                //if (GetIncludeObjHelper()) ExportHelperObject(node, indentLevel); 
                break;
        }
    }

    int nChildNodeCount = pNode->NumberOfChildren();

    for (int c = 0; c < nChildNodeCount; c++)
    {
        NodeEnum(pNode->GetChildNode(c));
    }

    return true;
}

BOOL MyExport::TMNegParity(Matrix3 &m)
{
    return (DotProd(CrossProd(m.GetRow(0), m.GetRow(1)), m.GetRow(2)) < 0.0) ? 1 : 0;
}

void MyExport::SetLHCoord(bool bLH)
{
    m_bLHCoord = bLH;
}

bool MyExport::ExportMaterial(INode* pNode, CAIRStaticMesh& mesh)
{
    Mtl* mtl = pNode->GetMtl();

    if (mtl)
    {
        ExportMaterial(pNode, mtl, mesh);
    }
    else
    {
        return false;
    }
    

    /*
    AIRMATERIAL airMaterial;
    memset(&airMaterial, 0, sizeof(AIRMATERIAL));

    //TSTR indent = GetIndent(indentLevel);

    // If the node does not have a material, export the wireframe color
    if (mtl) 
    {
        //暂定只有1个mtl，实际上有多个
        Color clrMaterial = mtl->GetAmbient();
        airMaterial.Ambient[0] = clrMaterial.r;
        airMaterial.Ambient[1] = clrMaterial.g;
        airMaterial.Ambient[2] = clrMaterial.b;

        clrMaterial = mtl->GetDiffuse();
        airMaterial.Diffuse[0] = clrMaterial.r;
        airMaterial.Diffuse[1] = clrMaterial.g;
        airMaterial.Diffuse[2] = clrMaterial.b;

        clrMaterial = mtl->GetSpecular();
        airMaterial.Specular[0] = clrMaterial.r;
        airMaterial.Specular[1] = clrMaterial.g;
        airMaterial.Specular[2] = clrMaterial.b;

        airMaterial.Shiness = mtl->GetShininess();
    }
    //else 
    //{
        airMaterial.dwColor = pNode->GetWireColor();
        //DWORD c = pNode->GetWireColor();
        //fprintf(pStream,"%s\t%s %s\n", indent.data(), ID_WIRECOLOR,
        //    Format(Color(GetRValue(c)/255.0f, GetGValue(c)/255.0f, GetBValue(c)/255.0f)));
    //}
    */
    return true;
}

bool MyExport::ExportMaterial(INode* pNode, Mtl* pMtl, CAIRStaticMesh& mesh)
{
    if (pMtl->NumSubMtls() > 0)
    {
        int nSubMtlCount = pMtl->NumSubMtls();
        for (int i = 0; i < nSubMtlCount; i++) 
        {
            Mtl* subMtl = pMtl->GetSubMtl(i);
            if (subMtl) 
            {
                ExportMaterial(pNode, subMtl, mesh);
            }
        }
    }
    else
    {
        //int nMtlID = pMtl->GetMtlID();
        for (int i = 0; i < pMtl->NumSubTexmaps(); i++)
        {
            Texmap* pTexMap = pMtl->GetSubTexmap(i);
            if (pTexMap)
            {
                if (i == ID_DI)
                {
                    const char* szTexmap = pTexMap->GetName();
                    BitmapTex *pBMPTex = (BitmapTex *) pTexMap;
                    const char* szBMP = pBMPTex->GetName();
                    const char* szMapName = pBMPTex->GetMapName();

                    std::string strName = szMapName;
                    int nIndex = strName.find_last_of("\\");
                    mesh.SetTextureName(string(szMapName + nIndex + 1));
                }


                AIRMATERIAL airMaterial;
                memset(&airMaterial, 0, sizeof(AIRMATERIAL));

                //TSTR indent = GetIndent(indentLevel);

                // If the node does not have a material, export the wireframe color
                if (pMtl) 
                {
                    //暂定只有1个mtl，实际上有多个
                    Color clrMaterial = pMtl->GetAmbient();
                    airMaterial.Ambient[0] = clrMaterial.r;
                    airMaterial.Ambient[1] = clrMaterial.g;
                    airMaterial.Ambient[2] = clrMaterial.b;

                    clrMaterial = pMtl->GetDiffuse();
                    airMaterial.Diffuse[0] = clrMaterial.r;
                    airMaterial.Diffuse[1] = clrMaterial.g;
                    airMaterial.Diffuse[2] = clrMaterial.b;

                    clrMaterial = pMtl->GetSpecular();
                    airMaterial.Specular[0] = clrMaterial.r;
                    airMaterial.Specular[1] = clrMaterial.g;
                    airMaterial.Specular[2] = clrMaterial.b;

                    airMaterial.Shiness = pMtl->GetShininess();
                    mesh.m_material = airMaterial;
                }
            }
        }
    }

    return true;
}

void MyExport::PreProcess(INode* pNode)
{
    Mtl* pMtl = pNode->GetMtl();

    if (pMtl->NumSubMtls() > 0)
    {
        int nSubMtlCount = pMtl->NumSubMtls();
        for (int i = 0; i < nSubMtlCount; i++) 
        {
            Mtl* subMtl = pMtl->GetSubMtl(i);
            if (subMtl) 
            {
                PreProcess(pNode);
            }
        }
    }
    else
        m_vtMtl.push_back(pMtl);
}

void MyExport::OnInitDialog(HWND hWnd)
{
	CenterWindow(hWnd,GetParent(hWnd));

	CMaxInterface* pInterface = AIREXPORTER->GetMaxInterface();
	Interval ARange =  pInterface->GetInterface()->GetAnimRange(); //获得动画的长度
	SetDlgItemInt(hWnd, IDC_EDIT_StartFrame, ARange.Start(), TRUE);
	SetDlgItemInt(hWnd, IDC_EDIT_EndFrame, ARange.End() / GetTicksPerFrame(), TRUE);
    AIREXPORTER->SetTotalFrame(ARange.End() / GetTicksPerFrame());

    char szMaterialName[256] = { 0 };
    strcpy(szMaterialName, m_strFileInput.c_str());
    char* p = strrchr(szMaterialName, '.');
    if (p)
    {
        strcpy(p, ".material");
    }
    else
        strcat(szMaterialName, ".material");
    SetDlgItemText(hWnd, IDC_EDT_MATERIAL, szMaterialName);

	AIREXPORTER->SetDlgHWnd(hWnd);
}

void MyExport::OnChooseSkeletonFile()
{
	OPENFILENAME   ofn;               //   common   dialog   box   structure   
	char   szFile[260];               //   buffer   for   file   name   
	//HWND   hwnd;                             //   owner   window   
	HANDLE   hf;                             //   file   handle   

	//   Initialize   OPENFILENAME   
	ZeroMemory(&ofn,   sizeof(ofn));   
	ofn.lStructSize   =   sizeof(ofn);   
	ofn.hwndOwner   =   GetActiveWindow();   
	ofn.lpstrFile   =   szFile;   
	//   
	//   Set   lpstrFile[0]   to   '\0'   so   that   GetOpenFileName   does   not     
	//   use   the   contents   of   szFile   to   initialize   itself.   
	//   
	ofn.lpstrFile[0]   =   '\0';   
	ofn.nMaxFile   =   sizeof(szFile);   
	ofn.lpstrFilter   =   "AIR Skeleton Files (*.airskel)|*.airskel|All Files (*.*)|*.*||";   
	ofn.nFilterIndex   =   1;   
	ofn.lpstrFileTitle   =   NULL;   
	ofn.nMaxFileTitle   =   0;   
	ofn.lpstrInitialDir   =   NULL;   
	ofn.Flags   =   OFN_PATHMUSTEXIST   |   OFN_FILEMUSTEXIST;   

	//   Display   the   Open   dialog   box.     

	if   (GetOpenFileName(&ofn)==TRUE)
	{
		if (AIREXPORTER->CreateSelectedSkeleton(szFile))
		{
			SetDlgItemText(AIREXPORTER->GetDlgHWnd(), IDC_EDIT_SKELETAL_FILE, szFile);
		}
	}
	/*
	CString strFilename;
	CFileDialog dlg(
		TRUE,
		"airskel",
		strFilename,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		"AIR Skeleton Files (*.airskel)|*.airskel|All Files (*.*)|*.*||",
		NULL);
	if(dlg.DoModal() != IDOK) 
		return;

	// set new filename
	strFilename = dlg.GetPathName();

	if (AIREXPORTER->CreateSelectedSkeleton(strFilename))
	{
		SetDlgItemText(AIREXPORTER->GetDlgHWnd(), IDC_EDIT_SKELETAL_FILE, strFilename);
	}
	*/
}

bool MyExport::OnBtnExportAnimation()
{
    char szFileName[256] = { 0 };
    GetDlgItemText(AIREXPORTER->GetDlgHWnd(), IDC_EDIT_SKELETAL_FILE, szFileName, 256);

    if (strlen(szFileName) == 0)
    {
        MessageBox(AIREXPORTER->GetDlgHWnd(), "请先输入骨骼文件！", "Warning", MB_OK);
        return false;
    }

    GetDlgItemText(AIREXPORTER->GetDlgHWnd(), IDC_EDIT_ANIMATION_NAME, szFileName, 256);
    if (strlen(szFileName) == 0)
    {
        MessageBox(AIREXPORTER->GetDlgHWnd(), "请输入动画的名字！", "Warning", MB_OK);
        return false;
    }

    AIREXPORTER->SetAnimationName(szFileName);

    return true;
    //AIREXPORTER->ExportAnimation()
}

void MyExport::SetExportAnimateOnly(bool bExportAni)
{
    m_bExportAnimateOnly = bExportAni;
}

void MyExport::SetExportStaticMesh(bool bExportStatic)
{
    m_bExportStaticMesh = bExportStatic;
}

bool MyExport::OnBtnExport()
{
    HWND hWnd = AIREXPORTER->GetDlgHWnd();
    AIREXPORTER->SetLeftHand(Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_LEFTHAND)));
    AIREXPORTER->SetUpY(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_YUP)));
    AIREXPORTER->SetExportMaterial(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_EXPORT_MATERIAL)));
    SetExportVertexAnimation(false);
    if (AIREXPORTER->IsExportMaterial())
    {
        char szMaterialFileName[256] = { 0 };
        GetDlgItemText(hWnd, IDC_EDT_MATERIAL, szMaterialFileName, 256);
        AIREXPORTER->SetMaterialFileName(szMaterialFileName);
        if (strlen(szMaterialFileName) == 0)
        {
            MessageBox(hWnd, "请输入要导出的材质文件名！", "提示", MB_OK);
            return false;
        }
    }
    AIREXPORTER->SetStartFrame(GetDlgItemInt(hWnd, IDC_EDIT_StartFrame, NULL, FALSE));
    AIREXPORTER->SetEndFrame(GetDlgItemInt(hWnd, IDC_EDIT_EndFrame, NULL, FALSE));
    SetExportStaticMesh(false);
    SetExportAnimateOnly(false);
    char szAnimationName[256] = { 0 };
    GetDlgItemText(hWnd, IDC_EDIT_ANIMATION_NAME, szAnimationName, 256);
    if (strlen(szAnimationName) == 0)
    {
        MessageBox(hWnd, "请输入动画的名字！", "Warning", MB_OK);
        return false;
    }
    AIREXPORTER->SetAnimationName(szAnimationName);

    return true;
}
