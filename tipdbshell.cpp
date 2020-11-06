#include "tipdbshell.h"


bool start = true;
folderid count_folders = 1;
int count_item_id = 0;

TIPDBShell::TIPDBShell()
{
    current_info_DB = new TIPInfo;
}

folderid TIPDBShell::ChangeFolder(folderid id, QString newFolder)
{
folderid ret = (start) ? 1 :2;
start = !start;
//QMessageBox::information(this, "Info", "Function ChangeFolder was ");
return ret;
}

folderinfo TIPDBShell::GetCurrentFolder()
{
pair<folderid, QString> ret = pair<folderid, QString>((start)?1:2, (start)?QString("/"):QString("/Folder2"));
return ret;

/*
pair<folderid, QString> current_folder(current_info_DB.id, current_info_DB.name);
return current_folder;
*/
}

bool TIPDBShell::GetFolderContents(folderid id, vector<TIPInfo *> &Items, vector<folderinfo *> &Folders) //
{
    srand(time(0));
    size_t f_size = rand()%10;
    size_t it_size = f_size+1;
    Items.clear();
    Folders.clear();
    folderinfo *fold = new folderinfo[f_size];
    TIPInfo * it = new TIPInfo[it_size];

    for (int i = 0; i < f_size; i++)
    {
        fold[i].first = count_folders;
        count_folders++;
        fold[i].second = "folder" + QString::number(count_folders);
        Folders.push_back(&fold[i]);
        //Folders.push_back();
    }
    for (int i = 0; i < it_size; i++)
    {
        //srand(time(0));
        it[i].sizeInTerms = (rand()%100) + 5;
       // srand(time(NULL));
        it[i].sizeInBytes = (rand()%100) + 3;
        it[i].ownerName = "Owner" + QString::number(count_item_id);
        it[i].name = "Name" + QString::number(count_item_id);
        it[i].id = count_item_id;
        count_item_id++;
        it[i].creationTime = QDateTime::currentDateTime();
        //srand(time(NULL));
        bool k = it[i].sizeInBytes%2;
        //srand(time(NULL));
        if (k) it[i].compData1 = ((rand() % 1000)/10);
        else it[i].compData1 = 0;
        k = rand()%2;
        //srand(time(NULL));
        if (k) it[i].compData2 = ((rand() % 1000)/10);
        else it[i].compData2 = 0;
        k = rand()%2;
        //srand(time(NULL));
        if (k) it[i].compData3 = ((rand() % 1000)/10);
        else it[i].compData3 = 0;
        Items.push_back(&it[i]);
    }
    start = true;
    return start;
}

bool TIPDBShell::CopyItem(TIPInfo *item, folderid source, folderid destination)
{
start = true;
return start;
}

bool TIPDBShell::MoveItem(TIPInfo *item, folderid source, folderid destination)
{
start = !start;
return !start;
}

bool TIPDBShell::DeleteItem(TIPInfo *item, folderid source)
{
return start;
}

folderid TIPDBShell::NewFolder(QString name)
{
folderid ret = count_item_id++;
return ret;
}

bool TIPDBShell::CopyFolder(folderid id, folderid source, folderid destination)
{
start = true;
return start;
}

bool TIPDBShell::MoveFolder(folderid id, folderid source, folderid destination)
{
start = !start;
return !start;
}

bool TIPDBShell::DeleteFolder(folderid source)
{
start = !start;
return start;
}

bool TIPDBShell::OpenItem(TIPInfo *item)
{
    std::cout << item->id << endl;;
start = !start;
return !start;
}

TIPInfo *TIPDBShell::CopyFileToDB(QString filename, folderid destination)
{
TIPInfo *info = new TIPInfo;
info->id = count_item_id++;//(start) ?1 :2;
info->name = (start)?QString("/"):QString("/Folder2");
info->sizeInBytes = (start) ?100 :300;
info->sizeInTerms = (start) ?200 :400;
info->creationTime = QDateTime::currentDateTime();
info->ownerName = (start)?QString("Igor1"):QString("Igor2");
info->compData1 = (start)?0:1;
info->compData2 = (start)?0:1;
info->compData3 = (start)?0:1;
return info;
}

bool TIPDBShell::CopyItemToFile(TIPInfo *item, QString filename)
{
start = !start;
return start;
}

float TIPDBShell::GetOperationProgress()
{
float ret;
ret = (start)?0.1:0.9;
return ret;
}

bool TIPDBShell::CompareItems(TIPInfo *item)
{
    start = true;
    return start;
}

bool TIPDBShell::CompareItemsToFile(QString filename)
{
    start = true;
    return start;
}

bool TIPDBShell::Init(QString arg)
{
    start = !start;
    return !start;
}
