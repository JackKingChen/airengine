#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "EngineConfig.h"
#include "MemoryAllocatorConfig.h"
using namespace std;

class EngineExport ConfigReader
{
public:
    ConfigReader();
    ConfigReader(const string cfg_fl, bool extract, const char* magicword);
    ~ConfigReader();

    bool ReadFile(const string cfg_fl);
    void Trim(string &vst);

    bool ExtractFile(const string txt_file, const char* magicword);

    bool Exist(const char* tag);

    istringstream& GetStringStream(const char* tag); 
    char* GetString(const char* tag);
    char*  GetString(const char* tag, char* streturn);
    double GetDouble(const char* tag);
    int GetInt(const char* tag);
    bool GetBool(const char* tag);
    size_t GetVectorSize(const char* tag);
    size_t GetMemberSize(const char* tag);
    double* GetVectorDouble(const char* tag, size_t &len);
    int* GetVectorInt(const char* tag, size_t &len);
    void GetVector(const char* tag, double &x, double &y, double &z);
    double GetVector(const char* tag, const int elem);

    char*  GetWord(const char* tag, const int wordpos);
    char*  GetWord(const char* tag, const int wordpos, char* streturn);
    void   Set(const char* tag, const char* value);
    void   Set(const char* tag, const double value);
    void   Set(const char* tag, const int value);
    void   Set(const char* tag, const bool value);
    char*  TagMember(const char* tag, const char* member);
    char*  TagMember(const char* tag, const int member);
    void   Dump(const char* fname);

    //以下由liangairan添加
    void   AddTag(const char* tag, const char* value);
    void   AddTag(const char* tag, const double value);
    void   AddTag(const char* tag, const int value);
    void   AddTag(const char* tag, const bool value);
protected:
private:
    AIRVector<std::string> Tag;
    AIRVector<std::string> Value;

    char ConfigFile[256];
    istringstream CfgStream;
};