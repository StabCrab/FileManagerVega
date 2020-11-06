/*******************************************************************\
 * Inifile.h  - заголовочный файл класса TIniFile, для работы с ini
 * файлами в Visual Studio/QNX/Linux
 *******************************************************************
 * Дата разработки               -  11.07.14
 * Дата последней корректировки  -  04.09.14  (A.V. Metelkin)
 * Автор						 -  A.Pershin, A. Barchenkov
\*******************************************************************/
#ifndef INIFILE_H
#define INIFILE_H

#include <string>
#include <vector>
#include <map>

//#include <RT/rtautime.h> // Ради TADateTime, в принципе эту зависимость можно под дефайн загнать
//#include <a101defs.h> // Поддержка (u)intXX_t под все платформы

// Винда не в курсе, что на эти числа есть дефайны
#ifdef _WIN32
    #define R_OK    4       /* Test for read permission.   */
    #define W_OK    2       /* Test for write permission.  */
    #define F_OK    0       /* Test for existence.         */
#endif

class TIniFile
{
private:
    std::string filename;
    bool enableCache; //хранит включённость кэширования
    std::vector<std::string> cache; //кэш содержимого файла
    std::map <std::string, int> sections; //хранит именя секций и номера строк их начала

    template <class TypeName> std::string Read(const std::string& section, const std::string& key, const TypeName& DefaultValue);
    template <class TypeName> bool Write(const std::string& section, const std::string& key, const TypeName& Value);
    std::string Trim(std::string str) const;//удаление пробелов в начале и конце строки str
    std::string TrimQuotes(std::string str) const; // удаление кавычек (" и ') по краям строки
    std::string GetSectionName(const std::string& str) const;//возвращает имя секции(значение, ограниченное []) из строки str
    std::string GetKeyName(const std::string& str) const;//возвращает имя ключа из строки str
    std::string GetValue(const std::string& str) const;//возвращает значение из строки str
    std::string GetComment(const std::string& str) const;//возвращает комментарий из строки str

    std::vector<std::string> ReadFileContents(); // чтения файла по строкам в вектор
    void updateCache(std::vector<std::string>& lines); // перезаписывает кэш файла строками из вектора

    size_t ProceedToSection(const std::string& section, std::vector<std::string>& lines); // возвращает номер строчки, следующей за заголовком секции, если не найдена, то lines.size()
    bool RewriteFile(std::vector<std::string>& lines); // Перезапиь файла строчками из вектора

    bool Equal(std::string s1, std::string s2); // проверка равенства двух строк без учета регистра - ВСЕ СРАВНЕНИЯ СЕКЦИЙ/КЛЮЧЕЙ ТОЛЬКО ЧЕРЕЗ ЭТОТ МЕТОД

public:
    TIniFile(std::string fname, bool enCache = false);
    std::string FileName() const;
    bool CacheEnabled() const; // возвращает, включено ли кэширование
    void EnableCache(bool enCahce); // включение/выключение кэширования
    bool Commit(); // запись кэша в файл

    /*
          Параметр section - имя секции.
          Параметр key - имя ключа.
          Параметр defval в функциях чтения - значение по умолчанию.
    */
    int         ReadInteger (std::string section, std::string key, int defval = 1);// чтение целочисленного значения
    bool        ReadBool    (std::string section, std::string key, bool defval = false);// чтение логического значения
    std::string ReadString  (std::string section, std::string key, std::string defval = "error");// чтение строкового значения
    std::string ReadStringS (std::string section, std::string key, std::string defval = "error", unsigned int strSize = sizeof("error"));
    double      ReadFloat   (std::string section, std::string key, double defval = 0.);// чтение вещественного значения
    int         ReadColor   (std::string section, std::string key, int defval = 0);// чтение значения цвета(в файле оно записано в 16-тиричной форме)
    void*       ReadStruct  (std::string section, std::string key, void *ptr, unsigned int size);// чтение структуры, принимает адрес куда писать и размер в байтах, при ошибке позвращает NULL и ничего туда не пишет
    uint64_t    ReadBitSet  (std::string section, std::string key, uint64_t defval = 0);// чтение из файла 64 бит

    // Функции записи [1] - [9] отличаются типом записываемых значений(запись в том же формате, что и чтение)
    bool WriteInteger   (std::string section, std::string key, int val);//[1]
    bool WriteFloat     (std::string section, std::string key, double val);//[2]
    bool WriteBool      (std::string section, std::string key, bool val);//[3]
    bool WriteString    (std::string section, std::string key, std::string val);//[4]
    bool WriteColor     (std::string section, std::string key, int val);//[5]
    bool WriteStruct    (std::string section, std::string key, void *ptr, unsigned int size);//[8]
    bool WriteBitSet    (std::string section, std::string key, uint64_t value);// [9]

    static bool FileExists(std::string fname);// проверка существования файла
    static bool CanWrite(std::string fname);  // имеем ли мы права на чтение файла
    static bool CanRead(std::string fname);   // имеем ли мы права на запись файла
    bool IsExist() { return FileExists(filename); } // поддержка legacy-кода

    int GetSectionCount();// кол-во секций
    int GetKeyCount(std::string section);// кол-во ключей в секции
    std::string SectionName(unsigned int SectionIndex);// имя секции по индексу
    std::string KeyName(std::string section, unsigned int KeyIndex);// имя ключа по имени секции и индексу

    bool sectionExists(std::string section); // есть ли в файле такая секция
    bool keyExists(std::string section, std::string key); // есть ли в секции такой ключ
    std::vector<std::string> GetSections(); // получить список секций
    std::vector<std::string> GetKeys(std::string section); // получить список ключей в секции
    std::vector<std::string> GetValues(std::string section); // получить список занчений в секции

    bool DeleteSection(std::string section);// удалении секции со всем содержимым
    bool DeleteKey(std::string section, std::string key);// удаление ключа из секции
};

#endif
