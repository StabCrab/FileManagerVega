#include "inifile.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <bitset>
#include <cctype>
#include <functional>
#include <locale>
#include <cstdlib>

#ifndef _WIN32
    #include <unistd.h>
#else
    #include <io.h>
#endif

// QNX и MSVS 2008 - не поддерживают семантику переноса (C++11)
// MSVS 2010 поддерживает по-умолчанию, Linux - если включить флаг
#if defined(__QNX__) || (defined(_MSC_VER) && _MSC_VER < 1600)
    #define move(a) a
#elif defined(__linux__) && (__cplusplus < 201103L)
    #error add QMAKE_CXXFLAGS += -std=c++11 to project file
#endif

using namespace std;

struct dot_separator : numpunct<char> {
    char do_decimal_point() const { return '.'; } // разделитель - точка
};
struct comma_separator : numpunct<char> {
    char do_decimal_point() const { return ','; } // разделитель - запятая
};

//*******************************************************************
template <class TypeName> bool TIniFile::Write(const string& section, const string& key, const TypeName& Value)
{
    // Если не указана секция/ключ или если файл есть, но прав писать в него нет
    if (section.empty() || key.empty() || (FileExists(filename) && !CanWrite(filename)))
        return false;

    //TypeName to string
    stringstream ss;
    ss << fixed;
    ss << Value;
    string val = ss.str();

    vector<string> lines = move(ReadFileContents());
    string sectionName;
    string keyName;

    size_t ndx = ProceedToSection(section, lines);
    if (ndx < lines.size())
    {
        string comment;
        bool keyExist = false;
        while (ndx < lines.size() && GetSectionName(lines[ndx]).empty())// пока не добрались до следующей секции
        {
            if(Equal(GetKeyName(lines[ndx]), key))
            {
                comment = GetComment(lines[ndx]);
                lines[ndx] = key + " = " + val + (comment.empty()? string(): (" " + comment));
                keyExist = true;
                break;
            }
            ++ndx;
        }
        if (!keyExist) //Нет ключа "key"
        {
            keyName = key + " = " + val;

            // дабы не писать вплотную к заголовку секции - ищем последнюю непустую строчку перед ним и вставляем после нее
            while (lines[ndx - 1].empty())
                ndx--;
            lines.insert(lines.begin() + ndx, keyName);
        }
    }
    else // Нет секции с именем "section"
    {
        sectionName = "[" + section + "]";
        lines.push_back(move(sectionName));
        keyName = key + " = " + val;
        lines.push_back(move(keyName));
    }

    return RewriteFile(lines);
}

//********************************************************************
template <class TypeName> string TIniFile::Read(const string& section, const string& key, const TypeName& DefaultValue)
{
    //TypeName to string
    stringstream ss;
    ss << DefaultValue;
    string value = ss.str();

    if (!section.empty() && !key.empty())
    {
        string tempVal;
        vector<string> lines = move(ReadFileContents());
        size_t ndx = ProceedToSection(section, lines);
        while (ndx < lines.size() && GetSectionName(lines[ndx]).empty())// пока не добрались до следующей секции
        {
            if(Equal(GetKeyName(lines[ndx]), key))
            {
                tempVal = GetValue(lines[ndx]);
                if(!tempVal.empty())
                    value = move(TrimQuotes(move(tempVal)));
                break;
            }
            ++ndx;
        }
    }
    return value;
}

//********************************************************************
string TIniFile::Trim(string str) const
{
    if (str.empty())
        return str;

    string::iterator it = str.begin();
    while(it != str.end() && isspace((unsigned char)*it))
        it++;

    string::reverse_iterator rit = str.rbegin();
    while(rit != str.rend() && isspace((unsigned char)*rit))
        rit++;

    // сначала затираем все пробельные символы с конца строки, потом - с начала (если там что осталось)
    str.erase(str.length() - (rit - str.rbegin()));
    if (!str.empty())
        str.erase(str.begin(), it);
    return str;
}

//********************************************************************
string TIniFile::TrimQuotes(string str) const
{
    if (str.length() > 1)
    {
        if ((str[0] == '\"' && str[str.length() - 1] == '\"') || (str[0] == '\'' && str[str.length() - 1] == '\''))
            str = str.substr(1, str.length() - 2);
    }
    return str;
}

//********************************************************************
string TIniFile::GetSectionName(const string& str) const
{
    size_t begin = str.find_first_of('[');
    if (begin != string::npos)
    {
        size_t end = str.find_last_of(']');
        if (end != string::npos)
        {
            size_t comment = str.find_first_of(";#=");//комменты и поля секций
            string sName;
            if(comment == string::npos || (comment > begin && comment > end))
                sName = str.substr(begin + 1, end - begin - 1);
            return Trim(move(sName));
        }
    }
    return string();
}

//********************************************************************
string TIniFile::SectionName(unsigned int index)
{
    vector<string> lines = move(ReadFileContents());

    string SectionName;
    int count = -1;
    for(size_t i = 0; i < lines.size(); ++i)
    {
        SectionName = GetSectionName(lines[i]);
        if(!SectionName.empty())
            ++count;
        if((unsigned int)count == index)
            return SectionName;
    }
    return string();
}

//********************************************************************
string TIniFile::GetKeyName(const string& str) const
{
    string key;
    size_t end = str.find_first_of('=');
    if (end != string::npos)
    {
        size_t comment = str.find_first_of(";#");
        if (comment == string::npos || (comment != string::npos && comment > end))
            key = str.substr(0, end);
    }
    return Trim(move(key));
}

//********************************************************************
string TIniFile::GetValue(const string& str) const
{
    string value;
    size_t begin = str.find_first_of('=');
    if (begin != string::npos)
    {
        size_t comment = str.find_first_of(";#");
        if (comment != string::npos)
        {
            if(comment > begin)
                value = str.substr(begin + 1, comment - begin - 1);
        }
        else
            value = str.substr(begin + 1);
    }
    return Trim(move(value));
}

//********************************************************************
string TIniFile::GetComment(const string& str) const
{
    size_t pos = str.find_first_of(";#");
    if (pos != string::npos)
        return str.substr(pos);
    else
        return string();
}

//********************************************************************
vector<string> TIniFile::ReadFileContents()
{
    if(CacheEnabled() && !cache.empty())
        return cache;

    vector<string> lines;

    if(!FileExists(filename) || !CanRead(filename))
        return lines;

    ifstream file(filename.c_str());
    if (file.is_open())
    {
        string line;
        while(getline(file, line))
            lines.push_back(move(Trim(move(line))));
        file.close();

        //Multi-line
        for(size_t i = 0 ; i < lines.size() ; ++i)
        {
            int lineSize = lines[i].size();
            if (lineSize > 1 && lines[i][lineSize - 1] == '\\' && lines[i][lineSize - 2] == '\\')
            {
                lines[i].erase(lines[i].end() - 1);
                if (i < lines.size() - 1)
                    lines[i] += lines[i + 1];
                lines.erase(lines.begin() + i + 1);
                --i;
            }
        }
    }

    updateCache(lines);

    return lines;
}

//********************************************************************
void TIniFile::updateCache(std::vector<std::string>& lines)
{
    if(!CacheEnabled())
        return;

    cache = lines;
    sections.clear();

    for(size_t i = 0; i < lines.size(); ++i)
    {
        string sect = GetSectionName(lines[i]);
        if (!sect.empty())
            sections[sect] = i;
    }
}

//********************************************************************
size_t TIniFile::ProceedToSection(const string& section, vector<string>& lines)
{
    if (CacheEnabled())
        return sections.count(section)? sections[section] + 1: cache.size();

    size_t ndx = 0;
    while (ndx < lines.size() && !Equal(GetSectionName(lines[ndx]), section))
        ++ndx;
    if (ndx < lines.size())
        ++ndx; // сдвигаемся еще, чтоб уйти с заголовка секции
    return ndx;
}

//********************************************************************
bool TIniFile::RewriteFile(vector<string>& lines)
{
    //затираем все пустые строки с конца файла
    while(lines.back().empty())
        lines.pop_back();

    updateCache(lines);

    if(CacheEnabled())
        return true;

    ofstream newFile(filename.c_str());
    if(newFile.is_open())
    {
        for(size_t i = 0; i < lines.size(); ++i)
            newFile << lines[i] << endl;
    }
    else
        return false;
    newFile.close();

    return true;
}

//********************************************************************
bool TIniFile::Equal(string s1, string s2)
{
    transform(s1.begin(), s1.end(), s1.begin(), ptr_fun<int, int>(tolower));
    transform(s2.begin(), s2.end(), s2.begin(), ptr_fun<int, int>(tolower));
    return s1 == s2;
}

//********************************************************************
int TIniFile::GetSectionCount()
{
    vector<string> lines = move(ReadFileContents());

    int SectionCount = 0;
    for(size_t i = 0; i < lines.size(); ++i)
    {
        if (!GetSectionName(lines[i]).empty())
            SectionCount++;
    }
    return SectionCount;
}

//********************************************************************
int TIniFile::GetKeyCount(string section)
{
    int keyCount = 0;
    vector<string> lines = move(ReadFileContents());

    size_t ndx = ProceedToSection(section, lines);
    if (ndx < lines.size())
    {
        while (ndx < lines.size() && GetSectionName(lines[ndx]).empty())// пока не добрались до следующей секции
        {
            if(!GetKeyName(lines[ndx]).empty())
                keyCount++;
            ++ndx;
        }
    }
    return keyCount;
}

//********************************************************************
string TIniFile::KeyName(string section, unsigned int KeyIndex)
{
    vector<string> lines = move(ReadFileContents());

    size_t ndx = ProceedToSection(section, lines);
    if (ndx < lines.size())
    {
        int keyCount = -1;
        while (ndx < lines.size() && GetSectionName(lines[ndx]).empty())// пока не добрались до следующей секции
        {
            if(!GetKeyName(lines[ndx]).empty())
                keyCount++;
            if ((unsigned int)keyCount == KeyIndex)
                return GetKeyName(lines[ndx]);
            ++ndx;
        }
    }
    return string();
}

//********************************************************************
bool TIniFile::sectionExists(string section)
{
    vector<string> sect(GetSections());
    for(size_t i = 0; i < sect.size(); ++i)
    {
        if (sect[i] == section)
            return true;
    }
    return false;
}

//********************************************************************
bool TIniFile::keyExists(string section, string key)
{
    vector<string> keys(GetKeys(section));
    for(size_t i = 0; i < keys.size(); ++i)
    {
        if (keys[i] == key)
            return true;
    }
    return false;
}

//********************************************************************
std::vector<string> TIniFile::GetSections()
{
    vector<string> res;

    if (CacheEnabled())
    {
        for(std::map <std::string, int>::iterator it = sections.begin(); it != sections.end(); ++it)
            res.push_back(it->first);
        return res;
    }

    vector<string> lines = move(ReadFileContents());
    for(size_t i = 0; i < lines.size(); ++i)
    {
        string sect = GetSectionName(lines[i]);
        if (!sect.empty())
            res.push_back(move(sect));
    }
    return res;
}

//********************************************************************
std::vector<string> TIniFile::GetKeys(string section)
{
    vector<string> lines = move(ReadFileContents());
    vector<string> keys;

    size_t ndx = ProceedToSection(section, lines);
    if (ndx < lines.size())
    {
        while (ndx < lines.size() && GetSectionName(lines[ndx]).empty())// пока не добрались до следующей секции
        {
            string key = GetKeyName(lines[ndx]);
            if (!key.empty())
                keys.push_back(move(key));
            ++ndx;
        }
    }
    return keys;
}

//********************************************************************
std::vector<string> TIniFile::GetValues(string section)
{
    vector<string> lines = move(ReadFileContents());
    vector<string> values;

    size_t ndx = ProceedToSection(section, lines);
    if (ndx < lines.size())
    {
        while (ndx < lines.size() && GetSectionName(lines[ndx]).empty())// пока не добрались до следующей секции
        {
            string value = GetValue(lines[ndx]);
            if (!value.empty())
                values.push_back(move(value));
            ++ndx;
        }
    }
    return values;
}
//********************************************************************
TIniFile::TIniFile(string fname, bool enCache)
{
    filename = fname;
    enableCache = enCache;

    if(CacheEnabled())
        ReadFileContents();
}

//********************************************************************
string TIniFile::FileName() const
{
    return filename;
}

//********************************************************************
void TIniFile::EnableCache(bool enCache)
{
    enableCache = enCache;
}

//********************************************************************
bool TIniFile::CacheEnabled() const
{
    return enableCache;
}

//********************************************************************
bool TIniFile::Commit()
{
    ofstream newFile(filename.c_str());
    if(newFile.is_open())
    {
        for(size_t i = 0; i < cache.size(); ++i)
            newFile << cache[i] << endl;
    }
    else
        return false;
    newFile.close();

    return true;
}

//********************************************************************
int TIniFile::ReadInteger(string section, string key, int defval)
{
    string str = Read(section, key, defval);
    char* parseEnd = NULL;
    int val = strtol(str.c_str(), &parseEnd, 10);
    if (parseEnd != str.c_str() && parseEnd <= str.c_str() + str.length())
        return val;
    else
        return defval;
}

//********************************************************************
bool  TIniFile::ReadBool(string section, string key, bool defval)
{
    char c = Read(section, key, defval).c_str()[0];
    return (c == '1' || tolower(c) == 't');
}

//********************************************************************
string TIniFile::ReadString(string section, string key, string defval)
{
    return Read(section, key, defval);
}

//********************************************************************
double TIniFile::ReadFloat(string section, string key, double defval)
{
    string str = Read(section, key, defval);

    double r1 = defval;
    istringstream text1(str);
    text1.imbue(locale(text1.getloc(), new dot_separator));// утечек памяти тут нет, локаль удалит сама
    text1 >> r1;

    double r2 = defval;
    istringstream text2(str);
    text2.imbue(locale(text1.getloc(), new comma_separator)); // см. выше
    text2 >> r2;

    double res = defval;
    if (!text1.fail() && !text2.fail())
    {
        // если в одной локали сконвертилось в целое, а в другой в нормальное, надо отдавать большее по модулю
        res = (fabs(r1) > fabs(r2))? r1: r2;
    }
    else if (!text1.fail())
        res = r1;
    else if (!text2.fail())
        res = r2;

    return res;
}

//********************************************************************
int TIniFile::ReadColor(string section, string key, int defval)
{
    unsigned int val;
    stringstream ss;
    ss << hex << Read(section, key, defval);
    ss >> val;
    return val;
}

//********************************************************************
void* TIniFile::ReadStruct(string section, string key, void *ptr, unsigned int size)
{
    string str = Read(section, key, string());
    if (str.empty())
        return NULL;

    char* chPtr = static_cast<char*>(ptr);
    stringstream raw(str);
    char s[3];
    size_t len = min(str.length() / 2, static_cast<size_t>(size));
    for (size_t i = 0; i < len; ++i)
    {
        raw.get(s, sizeof(s));
        chPtr[i] = strtol(s, NULL, 16);
    }
    return ptr;
}

//********************************************************************
string TIniFile::ReadStringS(string section, string key, string defval, unsigned int strSize)
{
    string ret = Read(section, key, defval);
    if (ret != defval && ret.size() > strSize)
        ret = defval;//ret.resize(strSize);

    return ret;
}

//***************************************************************************
bool  TIniFile::WriteInteger(string section, string key, int val)
{
    return Write(section, key, val);
}

//********************************************************************
bool  TIniFile::WriteString (string section, string key, string val)
{
    return Write(section, key, val);
}

//********************************************************************
bool  TIniFile::WriteBool(string section, string key, bool val)
{
    return Write(section, key, val);
}

//********************************************************************
bool  TIniFile::WriteFloat(string section, string key, double val)
{
    return Write(section, key, val);
}

//********************************************************************
bool TIniFile::WriteColor(string section,string key, int val)
{
    string newValue;
    stringstream ss;
    ss << hex << val;
    ss >> newValue;
    return Write(section, key, newValue);
}

//*********************************************************************
bool TIniFile::WriteStruct(string section, string key, void *ptr, unsigned int size)
{
    char* chPtr = static_cast<char*>(ptr);
    stringstream ss;
    ss << setfill('0') << hex << noshowbase;
    unsigned char ch;
    for(unsigned int i = 0; i < size; ++i)
    {
        ch = (chPtr[i])? chPtr[i]: 0;
        ss << setw(2) << static_cast<unsigned short>(ch);
    }
    string str = ss.str();
    return Write(section, key, str);
}

//**************************************************************************
bool TIniFile::FileExists(string fname)
{
#ifdef _MSC_VER >= 1900
    return _access(fname.c_str(), F_OK) != -1;
#else
    return access(fname.c_str(), F_OK) != -1;
#endif
}

//**************************************************************************
bool TIniFile::CanWrite(string fname)
{
#ifdef _MSC_VER >= 1900
    return _access(fname.c_str(), W_OK) != -1;
#else
    return access(fname.c_str(), W_OK) != -1;
#endif
}

//**************************************************************************
bool TIniFile::CanRead(string fname)
{
#ifdef _MSC_VER >= 1900
    return _access(fname.c_str(), R_OK) != -1;
#else
    return access(fname.c_str(), R_OK) != -1;
#endif
}

//**************************************************************************
bool TIniFile::DeleteSection(string section)
{
    bool erased = false;

    vector<string> lines = move(ReadFileContents());
    size_t ndx = ProceedToSection(section, lines);
    if (ndx < lines.size())
    {
        vector<string>::iterator itStart = lines.begin() + ndx - 1;
        while (ndx < lines.size() && GetSectionName(lines[ndx]).empty())// пока не добрались до следующей секции
            ++ndx;
        vector<string>::iterator itEnd = lines.begin() + ndx;
        lines.erase(itStart, itEnd);
        erased = RewriteFile(lines);
    }
    return erased;
}

//**************************************************************************
bool TIniFile::DeleteKey(string section, string key)
{
    vector<string> lines = move(ReadFileContents());

    bool erased = false;
    size_t ndx = ProceedToSection(section, lines);
    while (ndx < lines.size() && GetSectionName(lines[ndx]).empty())// пока не добрались до следующей секции
    {
        if(Equal(GetKeyName(lines[ndx]), key))
        {
            lines.erase(lines.begin() + ndx);
            erased = RewriteFile(lines);
            break;
        }
        ++ndx;
    }
    return erased;
}

//**************************************************************************
bool TIniFile::WriteBitSet(string section, string key, uint64_t value)
{
    return Write(section, key, bitset<64>(value).to_string());
}

//**************************************************************************
uint64_t TIniFile::ReadBitSet(string section, string key, uint64_t defval)
{
    string def = bitset<64>(defval).to_string();
    string bits = Read(section, key, def);

#ifndef __linux__
    uint64_t res = defval;
    char* tmp = (char*)&res;

    for (int i = 0; i < 8; ++i)
    {
       bitset<8> part(bits.substr(i * 8, 8));
       tmp[7 - i] = part.to_ulong();
    }
    return res;
#else
    return bitset<64>(bits).to_ulong();
#endif
}
