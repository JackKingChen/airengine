#include "stdafx.h"
/* lib
************************************************
* Object-MD
* Object Oriented Molecular Dynamics Program
* 
* (c)Yudi Rosandi, 2005
*
* Version 0.1 (05.2006)
*
* Project started on July 2005.
*
************************************************
* Object-MD Implementations
*
* Configuration file reader
*
*/

//#include "stdafx.h"
#include <cstring>
#include <cstdlib>
//#include <TOOL/throwerr>
#include "CfgReader.h"

#if AIR_TARGET_PLATFORM != AIR_PLATFORM_WIN32
#include <unistd.h>
#endif

#define MAX_LINE_LEN 1024
#define MAX_TAG_LEN 32

using std::ifstream;
using std::ofstream;
using std::ostringstream;

static char Dummy[256];
static char TagDummy[256];

/* Rules: Change it!!!!
- When no double quotes used, then all the spaces will be ignored
- When used everythings between the quotes will be accepted as string
including trailing spaces.
- When it find newline instead of double quote, the string 
is also accepted
- only double quote (") accepted
- Do not use '|' in the string, it is used for array variables
*/

// remove only trailing spaces and empty characters inside string
// Spaces inside string are accepted
// ',;|\n' are converted to space

ConfigReader::ConfigReader()
{

}

void ConfigReader::Trim(string &vst)
{
    for (unsigned int i=0;i<vst.length();i++) if ((vst[i]=='\r')||(vst[i]=='\t')) vst.erase(i);
    for (unsigned int i=vst.length()-1; (vst[i]==' '||vst[i]=='\n'||vst[i]=='\t')&&i>=0; i--) vst.erase(i);
    for (unsigned int i=0;i<vst.length();i++) 
        if (vst[i]=='\n'||vst[i]==','||vst[i]==';'||vst[i]=='|') vst[i]=' ';
}

ConfigReader::ConfigReader(const string cfg_fl, bool extract, const char* magicword) {
    if (extract) ExtractFile(cfg_fl, magicword);
    else ReadFile(cfg_fl);
}

ConfigReader::~ConfigReader() {
    Tag.clear();
    Value.clear();
}	

bool ConfigReader::ReadFile(const string cfg_fl) 
{
    char ch, stagcnt[8];
    int nline=0, tagnum;
    string tag, val;

    strcpy(ConfigFile, cfg_fl.c_str());
    ifstream Cfg(ConfigFile);
    if(Cfg.fail()) 
        return false;//THROWSTR("Cannot read configuration file...", ConfigFile);

    Tag.clear();
    Value.clear();
    while (Cfg.good()) 
    {
        //strips trailing spaces!
        while (Cfg.peek()=='\n'||Cfg.peek()=='\t'||Cfg.peek()==' '||Cfg.peek()=='\r')
            Cfg.get();
        if(Cfg.eof()) 
            break;
        nline++;
        if(Cfg.peek()=='#'||Cfg.peek()=='\n') 
            Cfg.ignore(MAX_LINE_LEN, '\n');
        else 
        {
            getline(Cfg, tag, '=');
            // throw all trailing whitespaces, check the first character
            while (Cfg.peek()=='\n'||Cfg.peek()=='\t'||Cfg.peek()==' '||Cfg.peek()=='\r')
                Cfg.get();
            switch(ch = Cfg.peek()) 
            {
                case '(': 
                    Cfg.get(); 
                    ch=')'; 
                    break;
                case '{': 
                    Cfg.get(); 
                    ch='}'; 
                    break;
                case '[': Cfg.get(); ch=']'; 
                    break;
                case '"': 
                    Cfg.get(); 
                    break;
                default:  
                    ch='\n';
            }
            while (Cfg.peek()=='\n'||Cfg.peek()=='\t'||Cfg.peek()==' '||Cfg.peek()=='\r')
                Cfg.get();
            getline(Cfg, val, ch);
            if(ch!='"') 
                Trim(val); 
            Trim(tag);
            if (tag.size() == 0 || val.size()==0) 
                return false;//THROWINT("Invalid format! Input file line", nline);

            tagnum = 0;			
            // First tag has no member id (tagnum=0)... next tag has member id=1....
            for(unsigned int i = 0; i < Tag.size(); i++) 
                if(Tag[i] == tag) 
                    tagnum = 1;
            if (tagnum == 1) 
            {
                tag.append(".");
                for(unsigned int i = 0; i < Tag.size(); i++) 
                    if(Tag[i].find(tag,0) == 0) 
                        tagnum++;
            }
            if(tagnum > 0) 
            {
                sprintf(stagcnt, "%d", tagnum); 
                tag.append(stagcnt);
            }
            Tag.push_back(tag);
            Value.push_back(val);
        }
    }
    Cfg.close();
    return true;
}

// With this function one can get the configuration commands from 
// the arbitrary text file.
// the configuration line must start with 'MagicWord' and must be in one line
// This function also check the existence of configuration tags within a file.
// True = exist, and read, False = not exist.

bool ConfigReader::ExtractFile(const string txt_file, const char* magicword) 
{	
    char tmpname[32];
    char MagicWord[8] = "#?";
    int i = 0;
    string line;
    if(magicword) 
        strcpy(MagicWord, magicword);
    sprintf(tmpname, "tmp%d", 0);
    ofstream ofl(tmpname);
    ifstream ifl(txt_file.c_str());
    while (ifl.good()) 
    {
        getline(ifl, line);
        if (line.substr(0,strlen(MagicWord)) == MagicWord) 
        {
            ofl << line.substr(strlen(MagicWord)) << '\n'; i++;
        }
    }
    ifl.close(); 
    ofl.close();	
    if (i == 0) 
        return false;
    ReadFile(tmpname); 
    unlink(tmpname);
    strcpy(ConfigFile, txt_file.c_str());
    return true;
}

bool ConfigReader::Exist(const char* tag)
{
    for(unsigned int i = 0; i < Tag.size(); i++) 
        if(Tag[i].compare(tag)==0) 
            return true;
    return false;
}

/**
Reads string value from configuration item 'tag'. 
It the tag name starts with $ then the file name stated
by the string value sill be read.

The following example tell ConfigReader to read from file 'readfile':

Data = $readfile

*/

istringstream& ConfigReader::GetStringStream(const char* tag) 
{
    CfgStream.clear();
    // CfgStream.seekg(0);
    for(unsigned int i=0; i<Tag.size(); i++)
        if(Tag[i].compare(tag)==0) 
        {
            if (Value[i][0]=='$') 
            {
                ostringstream ls;
                ifstream fs(&Value[i].c_str()[1]);
                if(fs.fail())
                {
                    return CfgStream;
                    //THROWSTR("Cannot open file", &Value[i].c_str()[1]);
                }
                    //THROWSTR("Cannot open file", &Value[i].c_str()[1]);
                char tmpch[32];
                while (fs >> tmpch) ls << tmpch << ' ';
                CfgStream.str(ls.str());
                //				DEBUGSTR("Isi str", ls.str().c_str());
            } 
            else 
                CfgStream.str(Value[i]);
            return CfgStream;
        }	
        
        //THROWSTR("Cannot find tag", tag); 
}

char* ConfigReader::GetString(const char* tag)
{
    GetStringStream(tag).getline(Dummy, MAX_LINE_LEN, EOF);
    return Dummy;
}

/**
Reads string value from configuration item 'tag', and copy it to streturn. 
*/

char*  ConfigReader::GetString(const char* tag, char* streturn)
{	
    strcpy(streturn, GetString(tag)); 
    return streturn;
}

/**
Reads double value from configuration item 'tag'. Returns double if the
value token is a number, otherwise undefined.
*/

double ConfigReader::GetDouble(const char* tag)
{
    double d; 
    GetStringStream(tag) >> d; 
    return d;
}

/**
Reads integer value from configuration item 'tag'. Returns integer if the
value token is an integer number, otherwise undefined.
*/

int ConfigReader::GetInt(const char* tag)
{
    int d; 
    GetStringStream(tag) >> d; 
    return d;
}

/**
Reads boolean value from configuration item 'tag'. Returns true if the value
token is 'true' (string), otherwise returns false.
*/

bool ConfigReader::GetBool(const char* tag)
{
    if (strcmp(GetWord(tag, 0), "true")==0) 
        return true;
    return false;
}

/**
This function give three double values to parameters x, y, and z.
It also returns the square value of vector. The vector is the series of
three numbers that is writen in the configuration file souronding by bracket
and separated by coma.  
Example: (1.0, 2.5, 3.0)
*/

size_t ConfigReader::GetVectorSize(const char* tag) 
{
    size_t s=0; char d[512];
    GetStringStream(tag);
    while(CfgStream >> d) 
        s++;
    return s;
}

size_t ConfigReader::GetMemberSize(const char* tag) 
{
    size_t s = 0; 
    int i = 0;
    //while(ExistMember(tag, i++)) 
    //    s++;
    if (s == 0) 
        return 0;
        //THROWSTR("Cannot find tag", tag); 
    return s;
}

// giving zero to len means take all elements
double* ConfigReader::GetVectorDouble(const char* tag, size_t &len) 
{
    double d;
    AIRVector<double> tmp;

    GetStringStream(tag);
    while(CfgStream >> d) 
        tmp.push_back(d);
    if (len==0 || len>tmp.size()) 
        len=tmp.size();
    double* dvec = new double[len];
    for (unsigned int i=0; i<len; i++) 
        dvec[i] = tmp[i];		
    return dvec;
}

int* ConfigReader::GetVectorInt(const char* tag, size_t &len) 
{
    int di;
    AIRVector<int> tmp;

    GetStringStream(tag);
    while(CfgStream >> di) 
    {
        tmp.push_back(di);
        if (tmp.size() == len) 
            break;
    }

    if (len == 0) 
        len = tmp.size();
    int* ivec=new int[len];
    for (unsigned int i=0; i<len; i++) 
        ivec[i]=tmp[i];		
    return ivec;
}

//candidates	
//size_t ConfigReader::GetVector(const char* tag, char* ivec[]);

void ConfigReader::GetVector(const char* tag, double &x, double &y, double &z)
{
    size_t len = 3;
    double *dvec = GetVectorDouble(tag, len);
    if (len < 3) 
        return;
        //THROWSTR("Invalid vector size", tag);
    x=dvec[0]; 
    y=dvec[1]; 
    z=dvec[2];
    delete[] dvec;
}

/**
Reads and returns one element of vector value from configuration item 'tag'. 
*/

double ConfigReader::GetVector(const char* tag, const int elem)
{
    size_t len=0;
    double *dvec=GetVectorDouble(tag, len);
    if (elem >= len) 
        return 0.0;
        //THROWSTR("Out of bound while reading vector", tag);
    double d = dvec[elem];
    delete[] dvec;
    return d;
}

char*  ConfigReader::GetWord(const char* tag, const int wordpos)
{
    int n=0;
    GetStringStream(tag);
    while (CfgStream >> Dummy) 
        if (wordpos == n++) 
            return Dummy;
    return NULL;
    //THROWSTR("Out of bound word from string", tag);
}

char*  ConfigReader::GetWord(const char* tag, const int wordpos, char* streturn)
{	
    strcpy(streturn, GetWord(tag, wordpos)); 
    return streturn;
}

/** 
Set the configuration item of tag 'tag' with the value 'value'. The values
can be double, integer or boolean string. There is no type checking of
these values when they are set to the tag.

*/

//@{	

void ConfigReader::Set(const char* tag, const char* value)
{
    for(unsigned int i=0; i<Tag.size(); i++)
        if(Tag[i].compare(tag)==0) 
        {	
            Value[i] = value;
            return;
        }

    //THROWSTR("Cannot find tag", tag);
}

void ConfigReader::Set(const char* tag, const double value)
{
    sprintf(Dummy,"%0.10f", value);
    Set(tag, Dummy);
}

void ConfigReader::Set(const char* tag, const int value)
{
    sprintf(Dummy,"%d", value);
    Set(tag, Dummy);
}

void ConfigReader::Set(const char* tag, const bool value)
{
    if (value) 
        strcpy(Dummy, "true");
    else 
        strcpy(Dummy, "false");
    Set(tag, Dummy);
}


//@}

/** 
Gets the full name of tags member name. These members have name in the
format: tagname.membername
*/

char* ConfigReader::TagMember(const char* tag, const char* member)
{
    sprintf(TagDummy, "%s.%s", tag, member); 
    return TagDummy;
}

/**
Tag member=0 means tag without member
*/

char* ConfigReader::TagMember(const char* tag, const int member) 
{
    if (member==0) strcpy(TagDummy, tag); 
    else sprintf(TagDummy, "%s.%d", tag, member); 
    return TagDummy;
}

/**
Writes all the tokens to a file pointing by the file name at parameter.
*/

void ConfigReader::Dump(const char* fname)
{
    ofstream fl(fname);
    if (fl.fail()) 
        return;
        //THROW("Cannot open file for writting...");

    fl  << "############################################\n"
        << "# Objective MD Configuration File\n"
        << "#\n"
        << "# (c) 2010, Liangairan\n"
        << "#\n"
        << "#\n\n";

    for (unsigned int i=0; i<Tag.size(); i++)
        fl << Tag.at(i) << " = " << Value.at(i) << '\n';

    fl.close();
}

void ConfigReader::AddTag(const char* tag, const char* value)
{
    Tag.push_back(tag);
    Value.push_back(value);
}

void ConfigReader::AddTag(const char* tag, const double value)
{
    sprintf(Dummy, "%0.10f", value);
    AddTag(tag, Dummy);
}

void ConfigReader::AddTag(const char* tag, const bool value)
{
    if (value) 
        strcpy(Dummy, "true");
    else 
        strcpy(Dummy, "false");
    AddTag(tag, Dummy);
}

void ConfigReader::AddTag(const char* tag, const int value)
{
    sprintf(Dummy, "%d", value);
    AddTag(tag, Dummy);
}
