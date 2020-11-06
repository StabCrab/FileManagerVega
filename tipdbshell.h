#ifndef TIPDBSHELL_H
#define TIPDBSHELL_H

#include <vector>
#include <QString>
#include <QTime>
#include <QModelIndex>
#include <QFileSystemModel>
#include <QMessageBox>
#include <iostream>


using namespace std;

struct TIPInfo
{
    long id;
    QString name; // полный путь
    long sizeInBytes;//-
    long sizeInTerms;
    QDateTime creationTime;
    QString ownerName;
// если нет данных у следующего блока, то не отображать
    double compData1;
    double compData2;
    double compData3;
};

typedef int folderid; // folderid есть у всех папок БД, то сть каждый фолдер пронумерован и это не зависит от той директории, в которой в данный момент находится пользователь
typedef pair<folderid, QString> folderinfo;

class TIPDBShell
{


public:
TIPInfo *current_info_DB;

    TIPDBShell();

    folderid ChangeFolder(folderid id = 0, QString newFolder = ""); //в работе
    folderinfo GetCurrentFolder(); // +
    bool GetFolderContents(folderid id, vector<TIPInfo*> &Items, vector<folderinfo*> &Folders); // в работе

// начало блока для работы с БД слева и БД справа
    bool CopyItem(TIPInfo *item, folderid source, folderid destination);
    bool MoveItem(TIPInfo *item, folderid source, folderid destination);
    bool DeleteItem(TIPInfo *item, folderid source);//+
    folderid NewFolder(QString name);
    bool CopyFolder(folderid id, folderid source, folderid destination);
    bool MoveFolder(folderid id, folderid source, folderid destination);
    bool DeleteFolder(folderid source);
    bool OpenItem(TIPInfo *item);
// конец блока для работы с БД слева и справа


// начало блока для работы с БД с одной стороны и файловой системой с другой
    TIPInfo *CopyFileToDB(QString filename, folderid destination);
    bool CopyItemToFile(TIPInfo *item, QString filename);
// конец блока для работы с БД с одной стороны и файловой системой с другой


    float GetOperationProgress(); // 0..1 вызывается во время различных операций (копирования, перемещения, удаления и тд....

    bool CompareItems(TIPInfo *item); // 0..1, выдавать в % f12 потом вызываем гетфолдерконтент. Сравнивает портреты БД слева и справа
    bool CompareItemsToFile(QString filename); // 0..1, выдавать в % БД с одной стороны, файловая система с другой

    bool Init(QString arg);
};


#endif // TIPDBSHELL_H
