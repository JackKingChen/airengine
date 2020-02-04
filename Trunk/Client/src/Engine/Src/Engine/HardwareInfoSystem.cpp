#include "stdafx.h"
#include "HardwareInfoSystem.h"

//#include <Shlwapi.h>
//#include <stdlib.h>
//#include <windows.h>
//#include <Mmsystem.h>
//#include <ddraw.h>
//#pragma comment(lib,"Winmm.lib")
//#pragma comment(lib,"dxguid.lib")
//#pragma comment(lib,"ShLwApi.Lib")
//
//#ifdef TCONFIG
//    #define NR "\r\n"
//#else
//    #define NR "\n"
//#endif
//
///// 显卡DEVICE ID
//// 0x0300应该是nvidia fx系列开始的device id，低于这个编号的，就使用5673(xp自带的显卡驱动），高于这个系列的，就推荐用7801
//const u32 NVIDIA_FX_START_DEVICEID = 0x0300;
//
///// 显卡驱动版本号
//const u32 NVIDIA_LOW_DRIVER_VERSION = 5673;
//const u32 NVIDIA_HIGH_DRIVER_VERSION = 7801;
//
///// 显卡VENDOR ID
//const u32 NVIDIA_VENDOR = 0x000010DE;
//const u32 ATI_VENDOR = 0x00001002;
//const u32 INTER_VENDOR = 0x00008086;
//const u32 SIS_VENDOR = 0x00001039;
//const u32 S3_VENDOR = 0x00005333;
//
//HardwareInfoSystem* HardwareInfoSystem::sHardwareInfoSystem = NULL;
//
//enum RealtimeShadowTechnique
//{
//    RST_NONE = 0,
//    RST_PROJECTOR,
//    RST_MODULATIVE_STENCIL,
//    RST_ADDITIVE_STENCIL,
//};
//
//enum TextureFilter
//{
//    TF_BILINEAR = 0,
//    TF_TRILINEAR,
//    TF_ANISOTROPIC,
//};
//
//enum PostFilter
//{
//    PF_DISABLE = 0,
//	PF_ENABLE,
//};
//
//enum FSAALevel
//{
//    FL_NONE = 0,
//    FL_LEVEL_TWO,
//    FL_LEVEL_FOUR,
//};
//
//HardwareInfoSystem::HardwareInfoSystem()
//{
//    sHardwareInfoSystem = this;
//
//    mD3DLibrary = 0;
//    mIDirect3D9 = NULL;
//
//    mMaxVSVersionMajor = 0;
//    mMaxVSVersionMinor = 0;
//    mMaxPSVersionMajor = 0;
//    mMaxPSVersionMinor = 0;
//
//    mHardwareInfoString = "";
//    mErrorInfoString = "";
//    mHardwareInfoValidateString = "";
//}
//
//HardwareInfoSystem::~HardwareInfoSystem()
//{
//    _deinitD3D();
//
//    sHardwareInfoSystem = NULL;
//}
//
//bool HardwareInfoSystem::_initD3D(void)
//{
//    mD3DLibrary = ::LoadLibrary("d3d9.dll");
//
//    if (!mD3DLibrary)
//    {
//        _pushErrorInfo("Can't find the d3d9.dll! HardwareInfoSystem::_initD3D");
//        return false;
//    }
//
//    typedef IDirect3D9 * (__stdcall *D3DCREATETYPE)(UINT);
//    D3DCREATETYPE d3dCreate = (D3DCREATETYPE)::GetProcAddress(mD3DLibrary, "Direct3DCreate9");
//
//    if (!d3dCreate)
//    {
//        _pushErrorInfo("GetProcAddress Direct3DCreate9 Failed! HardwareInfoSystem::_initD3D");
//        return false;
//    }
//
//    mIDirect3D9 = (*d3dCreate)(D3D_SDK_VERSION);
//
//    if (mIDirect3D9)
//    {
//        if ( FAILED( mIDirect3D9->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &mD3DCAPS9 ) ) ||
//            FAILED( mIDirect3D9->GetAdapterIdentifier( D3DADAPTER_DEFAULT, 0, &mD3DIdentifier9 ) ) )
//        {
//            _pushErrorInfo("Get d3d device info Failed! HardwareInfoSystem::_initD3D");
//            return false;
//        }
//        else
//        {
//            mMaxVSVersionMajor = static_cast<unsigned short>((mD3DCAPS9.VertexShaderVersion & 0x0000FF00) >> 8);
//            mMaxVSVersionMinor = static_cast<unsigned short>(mD3DCAPS9.VertexShaderVersion & 0x000000FF);
//
//            mMaxPSVersionMajor = static_cast<unsigned short>((mD3DCAPS9.PixelShaderVersion & 0x0000FF00) >> 8);
//            mMaxPSVersionMinor = static_cast<unsigned short>(mD3DCAPS9.PixelShaderVersion & 0x000000FF);
//
//            return true;
//        }
//    }
//    else
//    {
//        _pushErrorInfo("Create IDirect3D9 interface Failed! HardwareInfoSystem::_initD3D");
//        return false;
//    }
//}
//
//void HardwareInfoSystem::_deinitD3D(void)
//{
//    if (mIDirect3D9)
//    {
//        mIDirect3D9->Release();
//        mIDirect3D9 = NULL;
//    }
//
//    if (mD3DLibrary)
//        ::FreeLibrary(mD3DLibrary);
//}
//
//
//bool HardwareInfoSystem::_isInRange(const char* rangeStr, float number)
//{
//    std::string range(rangeStr);
//
//    int maxNum = 0;
//    int minNum = 0;
//
//    size_t operatorPos = range.find("<");
//    if (operatorPos != std::string::npos)
//    {
//        maxNum = atoi( range.substr(operatorPos + 1).c_str() );
//
//        if (number < maxNum)
//        {
//            return true;
//        }
//        else
//            return false;
//    }
//    else
//    {
//        operatorPos = range.find(">");
//
//        if (operatorPos != std::string::npos)
//        {
//            minNum = atoi( range.substr(operatorPos + 1).c_str() );
//
//            if (number > minNum)
//                return true;
//            else
//                return false;
//        }
//        else
//        {
//            operatorPos = range.find("-");
//
//            if (operatorPos != std::string::npos)
//            {
//                minNum = atoi( range.substr(0, range.size() - operatorPos).c_str() );
//                maxNum = atoi( range.substr(operatorPos + 1).c_str() );
//
//                if (number <= maxNum && number >= minNum)
//                    return true;
//                else
//                    return false;
//            }
//            else
//                return false;
//        }
//    }
//}
//
//
//bool HardwareInfoSystem::_ifCpuId()
//{
//    __try
//    {
//        _asm
//        {
//            xor eax, eax;
//            cpuid;
//        }
//    }
//    __except (EXCEPTION_EXECUTE_HANDLER)
//    {
//        return false;
//    }
//    return true;
//}
//
//void HardwareInfoSystem::_cpuVendor(char *szVendor)
//{
//    union{
//        char c[12 + 1];
//        struct{
//            u32 dw0;
//            u32 dw1;
//            u32 dw2;
//        }s;
//    }szTemp;
//    _asm
//    {
//        xor eax, eax;
//        cpuid;
//        mov szTemp.s.dw0, ebx;
//        mov szTemp.s.dw1, edx;
//        mov szTemp.s.dw2, ecx;
//    }
//    szTemp.c[12] = 0;
//    if (strcmp(szTemp.c, "AuthenticAMD") == 0
//        || strcmp(szTemp.c, "AMD ISBETTER") == 0)
//    {
//        strcpy(szVendor, "AMD");
//    }
//    else if (strcmp(szTemp.c, "GenuineIntel") == 0)
//    {
//        strcpy(szVendor, "Intel");
//    }
//    else if (strcmp(szTemp.c, "CyrixInstead") == 0)
//    {
//        strcpy(szVendor, "Cyrix/VIA");
//    }
//    else if (strcmp(szTemp.c, "CentaurHauls") == 0)
//    {
//        strcpy(szVendor, "IDT/Centaur");
//    }
//    else if (strcmp(szTemp.c, "NexGenDriven") == 0)
//    {
//        strcpy(szVendor, "NexGen");
//    }
//    else if (strcmp(szTemp.c, "UMC UMC UMC ") == 0)
//    {
//        strcpy(szVendor, "UMC");
//    }
//    else if (strcpy(szTemp.c, "RiseRiseRise") == 0)
//    {
//        strcpy(szVendor, "Rise");
//    }
//    else if (strcpy(szTemp.c, "GenuineTMx86") == 0)
//    {
//        strcpy(szVendor, "Transmeta");
//    }
//    else
//    {
//        strcpy(szVendor, szTemp.c);
//    }
//}
//
//void HardwareInfoSystem::_cpuTFMS(int *pnType, int *pnFamily, int *pnModel, int *pnStepping)
//{
//    u32 dwTemp;
//    _asm
//    {
//        mov eax, 1;
//        cpuid;
//        mov dwTemp, eax;
//    }
//    *pnType = (dwTemp >> 12) & 0x00000003;
//    *pnFamily = (dwTemp >> 8) & 0x0000000f;
//    if (*pnFamily == 15)
//    {
//        *pnFamily |= (dwTemp >> 16) & 0x00000ff0;
//    }
//    *pnModel = (dwTemp >> 4) & 0x0000000f;
//    if (*pnModel == 15)
//    {
//        *pnModel |= (dwTemp >> 12) & 0x000000f0;
//    }
//    *pnStepping = dwTemp & 0x0000000f;
//}
//
//bool HardwareInfoSystem::_cpuCache(int *pnL2, int *pnL3)
//{
//    u32 dwTemp;
//    union
//    {
//        BYTE by[16];
//        struct
//        {
//            u32 dw0;
//            u32 dw1;
//            u32 dw2;
//            u32 dw3;
//        }s;
//    }uTemp;
//    _asm
//    {
//        xor eax, eax;
//        cpuid;
//        mov dwTemp, eax;
//        mov eax, 2;
//        cpuid;
//        mov uTemp.s.dw0, eax;
//        mov uTemp.s.dw1, ebx;
//        mov uTemp.s.dw2, ecx;
//        mov uTemp.s.dw3, edx;
//    }
//    *pnL2 = -1;
//    *pnL3 = -1;
//    if (dwTemp < 2 || uTemp.by[0] != 1)
//    {
//        return false;
//    }
//    bool bNoL2L3 = false;
//    for (int i = 1; i < 16; i++)
//    {
//        int nNewL2 = -1;
//        int nNewL3 = -1;
//        switch (uTemp.by[i])
//        {
//        case 0x1a: nNewL2 = 96; break;
//        case 0x22: nNewL3 = 512; break;
//        case 0x23: nNewL3 = 1024; break;
//        case 0x25: nNewL3 = 2048; break;
//        case 0x29: nNewL3 = 4096; break;
//        case 0x39: nNewL2 = 128; break;
//        case 0x3b: nNewL2 = 128; break;
//        case 0x3c: nNewL2 = 256; break;
//        case 0x40: bNoL2L3 = true; break;
//        case 0x41: nNewL2 = 128; break;
//        case 0x42: nNewL2 = 256; break;
//        case 0x43: nNewL2 = 512; break;
//        case 0x44: nNewL2 = 1024; break;
//        case 0x45: nNewL2 = 2048; break;
//        case 0x79: nNewL2 = 128; break;
//        case 0x7a: nNewL2 = 256; break;
//        case 0x7b: nNewL2 = 512; break;
//        case 0x7c: nNewL2 = 1024; break;
//        case 0x7e: nNewL2 = 256; break;
//        case 0x81: nNewL2 = 128; break;
//        case 0x82: nNewL2 = 256; break;
//        case 0x83: nNewL2 = 512; break;
//        case 0x84: nNewL2 = 1024; break;
//        case 0x85: nNewL2 = 2048; break;
//        case 0x86: nNewL2 = 512; break;
//        case 0x87: nNewL2 = 1024; break;
//        case 0x88: nNewL3 = 2048; break;
//        case 0x89: nNewL3 = 4096; break;
//        case 0x8a: nNewL3 = 8192; break;
//        case 0x8d: nNewL3 = 4096; break;
//        }
//        if (nNewL2 != -1)
//        {
//            if (*pnL2 == -1)
//            {
//                *pnL2 = nNewL2;
//            }
//            else
//            {
//                *pnL2 = -2;
//            }
//            nNewL2 = -1;
//        }
//        if (nNewL3 != -1)
//        {
//            if (*pnL3 == -1)
//            {
//                *pnL3 = nNewL3;
//            }
//            else
//            {
//                *pnL3 = -2;
//            }
//            nNewL3 = -1;
//        }
//    }
//    if (bNoL2L3)
//    {
//        if (*pnL2 == -1)
//        {
//            *pnL2 = 0;
//        }
//        else if (*pnL3 == -1)
//        {
//            *pnL3 = 0;
//        }
//        else
//        {
//            *pnL2 = -2;
//            *pnL3 = -2;
//        }
//    }
//    if (*pnL2 == -2 || *pnL3 == -2)
//    {
//        return false;
//    }
//
//    if (*pnL2 == -1)
//    {
//        *pnL2 = 0;
//    }
//    if (*pnL3 == -1)
//    {
//        *pnL3 = 0;
//    }
//    return true;
//}
//
//int HardwareInfoSystem::_cpuBrand()
//{
//    u32 dwTemp;
//    _asm
//    {
//        mov eax, 1;
//        cpuid;
//        mov dwTemp, ebx;
//    }
//    return dwTemp & 0x000000ff;
//}
//
//inline unsigned __int64 RDTSC()
//{
//    _asm rdtsc;
//}
//
//int HardwareInfoSystem::_cpuSpeed()
//{
//    __try
//    {
//        _asm rdtsc;
//    }
//    __except (EXCEPTION_EXECUTE_HANDLER)
//    {
//        return 0;
//    }
//
//    __int64 qwBegin, qwEnd;
//    u32 dwBegin, dwEnd;
//    dwBegin = timeGetTime();
//    qwBegin = RDTSC();
//    while (timeGetTime() < dwBegin + 1000);
//    dwEnd = timeGetTime();
//    qwEnd = RDTSC();
//
//    return (int)((qwEnd - qwBegin) / (dwEnd - dwBegin) / 1000);
//}
//
//void HardwareInfoSystem::_InterCpu(char *szName, int nType, int nFamily, int nModel, int nStepping, int nL2, int nL3, int nBrand)
//{
//    switch (nFamily)
//    {
//    case 5:
//        switch (nModel)
//        {
//        case 0:
//            strcpy(szName, "Pentium A-Step");
//            break;
//        case 1:
//            strcpy(szName, "Pentium");
//            break;
//        case 2:
//        case 7:
//            strcpy(szName, "Pentium P54C");
//            break;
//        case 3:
//            strcpy(szName, "Pentium P24T");
//            break;
//        case 4:
//        case 5:
//        case 6:
//        case 8:
//            strcpy(szName, "Pentium MMX");
//            break;
//        default:
//            sprintf(szName, "Pentium Model %d\0", nModel);
//        }
//        break;
//    case 6:
//        switch (nModel)
//        {
//        case 0:
//        case 1:
//            strcpy(szName, "Pentium Pro");
//            break;
//        case 3:
//        case 4:
//            strcpy(szName, "Pentium II");
//            break;
//        case 5:
//            if (nL2 == 0)
//            {
//                strcpy(szName, "Celeron");
//            }
//            else
//            {
//                strcpy(szName, "Pentium II / Xeon");
//            }
//            break;
//        case 6:
//            if (nL2 == 0)
//            {
//                strcpy(szName, "Celeron");
//            }
//            else if (nL2 <= 128)
//            {
//                strcpy(szName, "Celeron A");
//            }
//            else
//            {
//                strcpy(szName, "Pentium II PE");
//            }
//            break;
//        case 7:
//        case 8:
//            if (nBrand == 1 || nBrand == 7 || (nBrand == 3 && nL2 < 256))
//            {
//                strcpy(szName, "PIII Celeron");
//            }
//            else if (nBrand == 3)
//            {
//                strcpy(szName, "PIII Xeon");
//            }
//            else
//            {
//                strcpy(szName, "Pentium III");
//            }
//            break;
//        case 9:
//            strcpy(szName, "PIII-M");
//            break;
//        case 0x0a:
//            strcpy(szName, "PIII Xeon");
//            break;
//        case 0x0b:
//            if (nBrand == 1 || nBrand == 7 || nBrand == 3)
//            {
//                strcpy(szName, "PIII Celeron");
//            }
//            else if (nBrand == 0x16)
//            {
//                strcpy(szName, "PIII-M");
//            }
//            else
//            {
//                strcpy(szName, "Pentium III");
//            }
//            break;
//        default:
//            sprintf(szName, "P6 Model %d\0", nModel);
//        }
//        break;
//    case 15:
//        switch (nBrand)
//        {
//        case 0x08:
//        case 0x0f:
//            if (nL2 < 256)
//            {
//                strcpy(szName, "P4 Celeron");
//            }
//            else
//            {
//                strcpy(szName, "Pentium 4");
//            }
//            break;
//        case 0x0b:
//            if (nModel == 1 && nStepping == 1)
//            {
//                strcpy(szName, "P4 Xeon MP");
//            }
//            else
//            {
//                strcpy(szName, "P4 Xeon");
//            }
//            break;
//        case 0x0e:
//            strcpy(szName, "Pentium 4 / Xeon");
//            break;
//        case 0x0a:
//            strcpy(szName, "P4 Celeron");
//            break;
//        case 0x0c:
//            strcpy(szName, "P4 Xeon MP");
//            break;
//        default:
//            strcpy(szName, "Pentium 4");
//        }
//        break;
//    default:
//        sprintf(szName, "Family %d Model %d\0", nFamily, nModel);
//    }
//}
//
//void HardwareInfoSystem::_AmdCpu(char *szName, int nType, int nFamily, int nModel, int nStepping)
//{
//    switch (nFamily)
//    {
//    case 5:
//        switch (nModel)
//        {
//        case 0:
//            strcpy(szName, "5k86");
//            break;
//        case 1:
//        case 2:
//        case 3:
//            strcpy(szName, "K5");
//            break;
//        case 6:
//        case 7:
//            strcpy(szName, "K6");
//            break;
//        case 8:
//            strcpy(szName, "K6-2");
//            break;
//        case 9:
//            strcpy(szName, "K6-3");
//            break;
//        case 0x0d:
//            strcpy(szName, "K6-2+ / K6-3+");
//            break;
//        default:
//            sprintf(szName, "K5 / K6 Model %d\0", nModel);
//        }
//        break;
//    case 6:
//        switch (nModel)
//        {
//        case 1:
//        case 2:
//        case 4:
//        case 6:
//        case 8:
//        case 0x0a:
//            strcpy(szName, "Athlon");
//            break;
//        case 3:
//        case 7:
//            strcpy(szName, "Duron");
//            break;
//        default:
//            sprintf(szName, "K7 Model %d\0", nModel);
//        }
//        break;
//    case 15:
//        if (nModel == 5)
//        {
//            strcpy(szName, "Opteron");
//        }
//        else
//        {
//            sprintf(szName, "K8 Model %d\0", nModel);
//        }
//        break;
//    default:
//        sprintf(szName, "Family %d Model %d\0", nFamily, nModel);
//    }
//}
//
//void HardwareInfoSystem::_getOSInfo(char* osInfo)
//{
//    OSVERSIONINFO osvi;
//    ZeroMemory(&osvi, sizeof(osvi));
//    osvi.dwOSVersionInfoSize = sizeof(osvi);
//    GetVersionEx(&osvi);
//
//    sprintf(osInfo, "Windows %d.%d\0", osvi.dwMajorVersion, osvi.dwMinorVersion);
//    if (osvi.dwPlatformId == VER_PLATFORM_WIN32s)
//    {
//        sprintf(osInfo, "Win32s on Windows %d.%d\0", osvi.dwMajorVersion, osvi.dwMinorVersion);
//    }
//    else if (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
//    {
//        if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
//        {
//            strcpy(osInfo, "Windows 95");
//        }
//        else if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
//        {
//            strcpy(osInfo, "Windows 98");
//        }
//        else if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
//        {
//            strcpy(osInfo, "Windows ME");
//        }
//    }
//    else if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
//    {
//        if (osvi.dwMajorVersion < 5)
//        {
//            sprintf(osInfo, "Windows NT %d.%d\0", osvi.dwMajorVersion, osvi.dwMinorVersion);
//        }
//        else if (osvi.dwMajorVersion == 5)
//        {
//            if (osvi.dwMinorVersion == 0)
//            {
//                strcpy(osInfo, "Windows 2000");
//            }
//            else if (osvi.dwMinorVersion == 1)
//            {
//                strcpy(osInfo, "Windows XP");
//            }
//            else if (osvi.dwMinorVersion == 2)
//            {
//                strcpy(osInfo, "Windows Server 2003 family");
//            }
//        }
//    }
//    strcat(osInfo, " ");
//    strcat(osInfo, osvi.szCSDVersion);
//}
//
//void HardwareInfoSystem::_getCPUInfo(char* cpuInfo)
//{
//    DWORD_PTR tam = SetThreadAffinityMask(GetCurrentThread(), 1);
//
//    if (!_ifCpuId())
//    {
//        strcpy(cpuInfo, "未知");
//    }
//    else
//    {
//        int nType, nFamily, nModel, nStepping;
//        _cpuVendor(cpuInfo + strlen(cpuInfo));
//        _cpuTFMS(&nType, &nFamily, &nModel, &nStepping);
//        if (strcmp(cpuInfo, "Intel") == 0)
//        {
//            int nL2, nL3, nBrand;
//            _cpuCache(&nL2, &nL3);
//            nBrand = _cpuBrand();
//            strcat(cpuInfo, " ");
//            _InterCpu(cpuInfo + strlen(cpuInfo), nType, nFamily, nModel, nStepping, nL2, nL3, nBrand);
//        }
//        else if (strcmp(cpuInfo, "AMD") == 0)
//        {
//            strcat(cpuInfo, " ");
//            _AmdCpu(cpuInfo + strlen(cpuInfo), nType, nFamily, nModel, nStepping);
//        }
//        else
//        {
//            sprintf(cpuInfo + strlen(cpuInfo), "Family %d Model %d\0", nFamily, nModel);
//        }
//    }
//
//    strcat(cpuInfo, "，主频: ");
//    int nSpeed = _cpuSpeed();
//    if (nSpeed != 0)
//    {
//        mHardwareInfo.mCpuSpeed = nSpeed;
//
//        if (nSpeed >= 1000)
//        {
//            sprintf(cpuInfo + strlen(cpuInfo), "%.2fGHz\0", (float)nSpeed / 1000.0f);
//        }
//        else
//        {
//            sprintf(cpuInfo + strlen(cpuInfo), "%dMHz\0", nSpeed);
//        }
//    }
//    else
//    {
//        strcat(cpuInfo, "未知");
//    }
//
//    SetThreadAffinityMask(GetCurrentThread(), tam);
//}
//
//void HardwareInfoSystem::_getMemoryInfo(char* memoryInfo)
//{
//    MEMORYSTATUS ms;
//    GlobalMemoryStatus(&ms);
//
//    mHardwareInfo.mPhyMemory = (float)ms.dwTotalPhys / 1024.0f / 1024.0f;
//
//    sprintf(memoryInfo, "%.1f MB\0", mHardwareInfo.mPhyMemory);
//
//    strcat(memoryInfo, "，虚拟内存: ");
//    sprintf(memoryInfo + strlen(memoryInfo), "%.1f MB\0", (float)ms.dwTotalVirtual / 1024.0f / 1024.0f);	
//
//    strcat(memoryInfo, "，空闲内存: ");
//
//    mHardwareInfo.mFreePhyMemory = (float)ms.dwAvailPhys / 1024.0f / 1024.0f;
//    sprintf(memoryInfo + strlen(memoryInfo), "%.1f MB\0", mHardwareInfo.mFreePhyMemory);
//}
//
//void HardwareInfoSystem::_getVideoCardInfo(char *szDevice, char *szDriver, char *extraInfo)
//{
//    if (mIDirect3D9)
//    {
//        strcpy( szDevice, mD3DIdentifier9.Description );
//
//        mHardwareInfo.mVideoDriverVersion = mD3DIdentifier9.DriverVersion.LowPart & 0x0000ffff;
//        sprintf( szDriver, "%s， 版本号: %d\0", 
//            mD3DIdentifier9.Driver, mHardwareInfo.mVideoDriverVersion );
//
//        sprintf( extraInfo, "显卡DeviceId: 0x%08X, 显卡VendorId: 0x%08X\0",            
//            mD3DIdentifier9.DeviceId, mD3DIdentifier9.VendorId );
//
//
//        mHardwareInfo.mVideoCardDeviceId = mD3DIdentifier9.DeviceId;
//        mHardwareInfo.mVideoCardVendorId = mD3DIdentifier9.VendorId;
//    }
//    else
//    {
//        strcpy(szDevice, "未知");
//        strcpy(szDriver, "未知，版本号: 未知");
//        strcpy(extraInfo, "显卡额外信息未知");
//    }
//
//    IDirectDraw7 *lpDD = NULL;
//
//    HINSTANCE mDDrawLibrary = 0;
//
//    mDDrawLibrary = ::LoadLibrary("ddraw.dll");
//
//    if (!mDDrawLibrary)
//    {
//        _pushErrorInfo("Can't find the ddraw.dll! HardwareInfoSystem::_getVideoCardInfo");
//        return;
//    }
//
//    typedef HRESULT (__stdcall *DDRAWCREATE)( GUID FAR * lpGuid, LPVOID  *lplpDD, REFIID  iid,IUnknown FAR *pUnkOuter );
//
//    DDRAWCREATE ddrawcreate = (DDRAWCREATE)::GetProcAddress(mDDrawLibrary, "DirectDrawCreateEx");
//
//    if (!ddrawcreate)
//    {
//        _pushErrorInfo("GetProcAddress DirectDrawCreateEx Failed! HardwareInfoSystem::_getVideoCardInfo");
//
//        ::FreeLibrary(mDDrawLibrary);
//        return;
//    }
//
//    HRESULT ddrawHr = (*ddrawcreate)(NULL, (LPVOID*)&lpDD, IID_IDirectDraw7, NULL);
//
//    if ( SUCCEEDED(ddrawHr) )
//    {
//        DDSCAPS2 DDSCaps;
//        ZeroMemory(&DDSCaps, sizeof(DDSCaps));
//        u32 dwTotalLocal, dwFreeLocal, dwTotalNonLocal, dwFreeNonLocal;
//
//        // 获取显卡显存信息
//        DDSCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM ;
//
//        if ( DD_OK == lpDD->GetAvailableVidMem(&DDSCaps, &dwTotalLocal, &dwFreeLocal) )
//        {
//            strcat(szDevice, NR);
//			strcat(szDevice, "显卡显存: ");
//
//            mHardwareInfo.mLocalVideoMemory = (float)dwTotalLocal / 1024.0f / 1024.0f;
//
//            sprintf(szDevice + strlen(szDevice), "%.1f MB\0", mHardwareInfo.mLocalVideoMemory);
//
//            mHardwareInfo.mFreeLocalVideoMemory = (float)dwFreeLocal / 1024.0f / 1024.0f;
//
//			strcat(szDevice, ", 空闲显卡显存: ");
//
//            sprintf(szDevice + strlen(szDevice), "%.1f MB\0", mHardwareInfo.mFreeLocalVideoMemory);
//            
//        }
//        else
//        {
//            _pushErrorInfo("GetAvailableVidMem Failed! HardwareInfoSystem::_getVideoCardInfo");
//            
//            strcat(szDevice, NR);
//            strcat(szDevice, "显卡显存容量未知");
//        }
//
//        // 获取AGP显存信息
//        DDSCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_NONLOCALVIDMEM ;
//
//        if ( DD_OK == lpDD->GetAvailableVidMem(&DDSCaps, &dwTotalNonLocal, &dwFreeNonLocal) )
//        {         
//            strcat(szDevice, NR);
//            strcat(szDevice, "AGP显存: ");
//
//            mHardwareInfo.mNonLocalVideoMemory = (float)dwTotalNonLocal / 1024.0f / 1024.0f;
//
//            sprintf(szDevice + strlen(szDevice), "%.1f MB\0", mHardwareInfo.mNonLocalVideoMemory);
//
//            mHardwareInfo.mFreeNonLocalVideoMemory = (float)dwFreeNonLocal / 1024.0f / 1024.0f;
//
//            strcat(szDevice, ", 空闲AGP显存: ");
//
//            sprintf(szDevice + strlen(szDevice), "%.1f MB\0", mHardwareInfo.mFreeNonLocalVideoMemory);
//
//        }
//        else
//        {
//            _pushErrorInfo("GetAvailableVidMem Failed! HardwareInfoSystem::_getVideoCardInfo");
//
//            strcat(szDevice, NR);
//            strcat(szDevice, "AGP显存容量未知");
//        }
//
//    }
//    else
//    {
//        strcat(szDevice, "\n显存容量未知");
//        _pushErrorInfo("DirectDrawCreateEx Failed! HardwareInfoSystem::_getVideoCardInfo");
//    }
//
//    if (lpDD)
//    {
//        lpDD->Release();
//        lpDD = NULL;
//    }
//
//    ::FreeLibrary(mDDrawLibrary);
//}
//
//const std::string& HardwareInfoSystem::validateVideoDriver(void)
//{
//    bool isValidVideoDriverVersion = true;
//    u32 recommendableVideoDriverVersion = 0;
//
//    // 根据显卡vendor id来辨别各种显卡
//    switch (mHardwareInfo.mVideoCardVendorId)
//    {
//    case NVIDIA_VENDOR:
//        {
//            // 低于fx系列的（tnt，geforce2, geforce4等）
//            if (mHardwareInfo.mVideoCardDeviceId < NVIDIA_FX_START_DEVICEID) 
//            {
//                // 如果驱动版本低
//                if (mHardwareInfo.mVideoDriverVersion < NVIDIA_LOW_DRIVER_VERSION)
//                {
//                    isValidVideoDriverVersion = false;
//                    recommendableVideoDriverVersion = NVIDIA_LOW_DRIVER_VERSION;
//                }                
//            }
//            // fx系列以上
//            else
//            {
//                // 如果驱动版本低
//                if (mHardwareInfo.mVideoDriverVersion < NVIDIA_HIGH_DRIVER_VERSION)
//                {
//                    isValidVideoDriverVersion = false;
//                    recommendableVideoDriverVersion = NVIDIA_HIGH_DRIVER_VERSION;
//                }
//            }
//        }
//        break;
//
//    case ATI_VENDOR:
//        {
//        }
//        break;
//
//    case INTER_VENDOR:                // inter
//        {
//        }
//        break;
//
//    default:
//        {
//        }
//    }
//
//    if (false == isValidVideoDriverVersion)
//    {
//        char errorInfoStr[256];
//
//        sprintf(errorInfoStr, "您的当前显卡为 %s \n"
//            "当前显卡驱动版本为 %d ，版本过低，可能无法顺利地运行游戏\n"
//            "推荐驱动版本为 %d，请到 http://tl.sohu.com/download/client.shtml 下载显卡驱动\n",
//            mD3DIdentifier9.Description, mHardwareInfo.mVideoDriverVersion, recommendableVideoDriverVersion );
//
//        mHardwareInfoValidateString = errorInfoStr;
//    }
//
//    return mHardwareInfoValidateString;
//}
//
//const char* HardwareInfoSystem::getDXVersion(void)
//{
//    if (!mHardwareInfo.mDXVersion)
//    {
//        u32 dwType, dwSize=MAX_PATH;
//
//        mHardwareInfo.mDXVersion = new char[MAX_PATH];
//
//        ::SHGetValue( HKEY_LOCAL_MACHINE, "Software\\Microsoft\\DirectX", "Version", 
//            &dwType, mHardwareInfo.mDXVersion, &dwSize );
//    }
//
//    return mHardwareInfo.mDXVersion;
//}
//
//void HardwareInfoSystem::_buildHardwareInfoString(void)
//{
//    mHardwareInfoString.clear();
//
//    char szTemp1[MAX_PATH] = {0};
//    char szTemp2[MAX_PATH] = {0};
//    char szTemp3[MAX_PATH] = {0};
//
//    _getOSInfo(szTemp1); 
//#ifndef TCONFIG
//    mHardwareInfoString += NR;
//#endif
//	mHardwareInfoString += "系统: ";
//    mHardwareInfoString += szTemp1;
//    mHardwareInfoString += NR;
//
//    szTemp1[0] = 0;
//    _getCPUInfo(szTemp1);  
//	mHardwareInfoString += "CPU: ";
//    mHardwareInfoString += szTemp1;
//    mHardwareInfoString += NR;
//
//    szTemp1[0] = 0;
//    _getMemoryInfo(szTemp1); 
//	mHardwareInfoString += "内存: ";
//    mHardwareInfoString += szTemp1;
//    mHardwareInfoString += NR;
//
//    szTemp1[0] = 0;
//    _getVideoCardInfo(szTemp1, szTemp2, szTemp3);
//	mHardwareInfoString += "显卡: ";
//    mHardwareInfoString += szTemp1;
//    mHardwareInfoString += NR;
//	mHardwareInfoString += "驱动: ";
//    mHardwareInfoString += szTemp2;
//    mHardwareInfoString += NR;
//    mHardwareInfoString += szTemp3;
//    mHardwareInfoString += NR;
//
//    getDXVersion();
//    mHardwareInfoString += "DirectX: ";
//    mHardwareInfoString += mHardwareInfo.mDXVersion;
//    mHardwareInfoString += NR;
//}
//
//const char* HardwareInfoSystem::getHardwareInfoAsString(void)
//{
//    return mHardwareInfoString.c_str();
//}
//
//int HardwareInfoSystem::_getMaxFSAA(void)
//{
//    if ( D3D_OK == mIDirect3D9->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT,
//        D3DDEVTYPE_HAL,
//        D3DFMT_R5G6B5,
//        FALSE,
//        D3DMULTISAMPLE_4_SAMPLES,
//        0 ) && 
//        D3D_OK == mIDirect3D9->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT,
//        D3DDEVTYPE_HAL,
//        D3DFMT_X8R8G8B8,
//        FALSE,
//        D3DMULTISAMPLE_4_SAMPLES,
//        0 ) )
//    {
//        return FL_LEVEL_FOUR;
//    }
//    else if ( D3D_OK == mIDirect3D9->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT,
//        D3DDEVTYPE_HAL,
//        D3DFMT_R5G6B5,
//        FALSE,
//        D3DMULTISAMPLE_2_SAMPLES,
//        0 ) && 
//        D3D_OK == mIDirect3D9->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT,
//        D3DDEVTYPE_HAL,
//        D3DFMT_X8R8G8B8,
//        FALSE,
//        D3DMULTISAMPLE_2_SAMPLES,
//        0 ) )
//    {
//        return FL_LEVEL_TWO;
//    }
//    else
//        return FL_NONE;
//}
//
//int HardwareInfoSystem::_getMaxShadowTechnique(void)
//{
//    if (mMaxPSVersionMajor >= 1 && mMaxPSVersionMinor >= 0)
//        return RST_ADDITIVE_STENCIL;
//    else 
//        return RST_MODULATIVE_STENCIL;
//}
//
//int HardwareInfoSystem::_getMaxTextureFilter(void)
//{
//    if (mD3DCAPS9.MaxAnisotropy > 1)
//        return TF_ANISOTROPIC;
//    else
//        return TF_TRILINEAR;
//}
//
//int HardwareInfoSystem::_getPostFilterEnable(void)
//{
//    if (mMaxPSVersionMajor >= 2 && mMaxPSVersionMinor >= 0)
//        return PF_ENABLE;
//    else
//        return PF_DISABLE;
//}
//
//void HardwareInfoSystem::_setSingleOption(SingleOption& option, int minValue, int maxValue)
//{
//    option.mMaxValue = maxValue;
//    option.mMinValue = minValue > maxValue ? maxValue : minValue;
//}
//
//void HardwareInfoSystem::_initOptionMap(void)
//{
//    if (!mIDirect3D9)
//        return;
//
//    mVideoOptionMap.clear();
//
//    SingleOption singleOption;
//
//    // terrain light map
//    _setSingleOption(singleOption, mVideoSetupInfo.mTerrainLightMap, 2);
//    mVideoOptionMap.insert( VideoOptionMap::value_type("View_LightmapQuality", singleOption) );
//
//    // anti alias
//    _setSingleOption(singleOption, mVideoSetupInfo.mAntiAlias, _getMaxFSAA());
//    mVideoOptionMap.insert( VideoOptionMap::value_type("View_Fanhunjiao", singleOption) );
//
//    // real time shadow
//    _setSingleOption(singleOption, mVideoSetupInfo.mRealtimeShadow, _getMaxShadowTechnique());
//    mVideoOptionMap.insert( VideoOptionMap::value_type("View_HumanLightmap", singleOption) );
//
//    // texture quality
//    _setSingleOption(singleOption, mVideoSetupInfo.mTextureQuality, 2);
//    mVideoOptionMap.insert( VideoOptionMap::value_type("View_ColorQuality", singleOption) );
//
//    // texture filter
//    _setSingleOption(singleOption, mVideoSetupInfo.mTextureFilter, _getMaxTextureFilter());
//    mVideoOptionMap.insert( VideoOptionMap::value_type("View_TextureFiltering", singleOption) );
//
//    // post filter
//    _setSingleOption(singleOption, mVideoSetupInfo.mPostFilter, _getPostFilterEnable());
//    mVideoOptionMap.insert( VideoOptionMap::value_type("View_FullScreenLight", singleOption) );
//
//    // vsync
//    _setSingleOption(singleOption, mVideoSetupInfo.mVSync, 1);
//    mVideoOptionMap.insert( VideoOptionMap::value_type("View_Aplomb", singleOption) );
//
//    // weather
//    _setSingleOption(singleOption, mVideoSetupInfo.mWeather, 1);
//    mVideoOptionMap.insert( VideoOptionMap::value_type("View_Weather", singleOption) );
//
//    // walk effect
//    _setSingleOption(singleOption, mVideoSetupInfo.mWalkEffect, 1);
//    mVideoOptionMap.insert( VideoOptionMap::value_type("View_WalkEffect", singleOption) );
//}
//
//bool HardwareInfoSystem::initSystem(void)
//{
//    bool succeedD3D = _initD3D();
//
//    _buildHardwareInfoString();
//
//    _initOptionMap();
//
//    return succeedD3D;
//}
//
//void HardwareInfoSystem::_pushErrorInfo(const std::string& errorInfo)
//{
//    mErrorInfoString += errorInfo;
//    mErrorInfoString += '\n';
//}
//
//const char* HardwareInfoSystem::getErrorInfoString(void)
//{
//    return mErrorInfoString.c_str();
//}
