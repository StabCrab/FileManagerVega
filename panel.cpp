#include "panel.h"

Panel::Panel(QWidget *parent)
: QTreeView(parent), info(""), countChosenFolders(0), countChosenFiles(0), countFolders(0), countFiles(0),size(0), currentDirSize(0)
{
    this->setItemsExpandable(false);
    this->setRootIsDecorated(false);
    this->setAllColumnsShowFocus(true);
    this->setSelectionMode(QAbstractItemView:: NoSelection);
    DBmodel = new QStandardItemModel(this);
    DBmodel->insertColumns(0, 8);
    QStringList Coloumns_name;
    Coloumns_name << "      Name" << "Size" << "Id" << "Owner" << "Date Creation" << "Relevance" << " " << " "; // ��������� ���: CompData1, CompData2, CompData3
    int i = 0;
    foreach (QString it, Coloumns_name)
    {
        DBmodel->setHeaderData(i, Qt::Horizontal, QVariant(it));
        i++;
    }
    functions_of_current_BD = new TIPDBShell;
    //ui->leftPanel2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //ui->rightPanel2->setEditTriggers(QAbstractItemView::NoEditTriggers);


}
QString Panel::getPath()
{
    return this->path;
}

QString Panel::getInfo()
{
    return this->info;
}
QModelIndexList Panel::getList()
{
    return this->list;
}
void Panel::chooseButton()
{
    if (this->selectionMode() != QAbstractItemView::MultiSelection)
        this->setSelectionMode(QAbstractItemView::MultiSelection);
    this->setCurrentIndex(this->currentIndex());
    this->choose(this->currentIndex());
}
void Panel:: choose(QModelIndex index)
{
    if (!index.isValid())
        return;
    if (this->selectionMode() != QAbstractItemView::MultiSelection)
    {
        if (this->selectionMode()==QAbstractItemView::NoSelection)
        {
            this->setSelectionMode(QAbstractItemView::SingleSelection);
            this->setCurrentIndex(index);
        }
        if (!ifDB)
        {
            list.clear();
            list.push_back(index);
            size = 0;
            countChosenFiles = 0;
            countChosenFolders = 0;
            emit updateInfo(isLeft, true, index);
        }
        else
        {
            countChosenFiles = 0;
            countChosenFolders = 0;
            chosenItems.clear();
            chosenFolders.clear();
            PushDB(index);
            emit updateInfo(isLeft, true, index);
        }
    }
    else if (this->selectionMode() == QAbstractItemView::MultiSelection && !list.contains(index))
    {
        if (!ifDB)
        {
            list.push_back(index);
            emit updateInfo(isLeft, true, index);
        }
        else
        {
            emit updateInfo(isLeft, true, index);
            PushDB(index);
        }
    }
    else if (this->selectionMode() == QAbstractItemView::MultiSelection && list.contains(index))
    {
         if (!ifDB)
         {
             list.removeOne(index);
             emit updateInfo(isLeft, false, index);
         }
         else
         {
             emit updateInfo(isLeft, true, index);
             RemoveDB(index);
         }
    }
    info.append(QString :: number(size) + " KB ");
    info.append("of " + QString :: number(currentDirSize) + " KB ");
    info.append("files " + QString:: number(countChosenFiles) + " of " + QString::number(countFiles) + " folders " +QString::number(countChosenFolders)+" of " + QString :: number(countFolders));
    emit showInfo(info);
    info.clear();
}
void Panel:: changeDirectory(QModelIndex index)
{
    if (!index.isValid())
        return;
    size = 0;
    QStandardItem *mb_ps = this->getDB()->item(index.row());
    if (ifDB && mb_ps->text() == "..")
    {
        current_folder_id = pathID.back();
        pathID.pop_back();
        this->setPath(cdUp(this->getPath()));
        this->changeCurrentFolderInfo(this->getPath(), 0, 0, 0);
        this->ChangeFolderDB(this->getCurrentFolder()); //тут мы должны сменить директорию
    }
    else if (ifDB && this->getPath() == "/" && DBmodel->item(index.row(), 1)->text() == " ")
    {
            pathID.push_back(current_folder_id);
            this->setCurrentFolder(this->getDB()->item(index.row(), 2)->text().toInt());
            path = path + this->getDB()->item(index.row())->text();
            this->changeCurrentFolderInfo(this->getPath(), 0, 0, 0);
            this->ChangeFolderDB(this->getCurrentFolder()); //тут мы должны сменить директорию
    }
    else if (ifDB && this->getPath() != "/" && DBmodel->item(index.row(), 1)->text() == " ")
    {
            pathID.push_back(current_folder_id);
            this->setCurrentFolder(this->getDB()->item(index.row(), 2)->text().toInt());
            path = path + "/" + this->getDB()->item(index.row())->text();
            this->changeCurrentFolderInfo(this->getPath(), 0, 0, 0);
            this->ChangeFolderDB(this->getCurrentFolder()); //тут мы должны сменить директорию
    }
    else if (ifDB && DBmodel->item(index.row(), 1)->text() != " ")
    {
        this->getFunctionsDB()->OpenItem(findItem(this->getDB()->item(index.row(), 2)->text().toInt()));
    }
    else
    {
        if (this->filesystem->fileInfo(index).fileName() == "..")
        {
            this->setRootIndex(filesystem->index(filesystem->filePath(index)));
            this->changeFolder(isLeft, index);
        }
        else
        {
            this->setRootIndex(index);
            this->setPath(filesystem->filePath(index));
            this->changeFolder(isLeft, index);
        }
    }
    this->clearInfo();
    this->setFocus();
    this->setSelectionMode(QAbstractItemView::NoSelection);
    this->setCurrentIndex(model()->index(0,0, this->rootIndex()));
    if (ifDB)
        changeCurrentFolderInfo(path, 0,0,0);
    else
        InfoToString();
}
void Panel:: changeSelectionMode()
{
    if (this->selectionMode() == QAbstractItemView::NoSelection)
    {
        this->setSelectionMode(QAbstractItemView::MultiSelection);
    }
    else if (this->selectionMode() == QAbstractItemView::MultiSelection)
    {
        this->setSelectionMode(QAbstractItemView::SingleSelection);
    }
    else if (this->selectionMode() == QAbstractItemView::SingleSelection)
    {
        this->setSelectionMode(QAbstractItemView::MultiSelection);
    }
    this->clearInfo();
    this->InfoToString();
}
void Panel::changeCountChosenFiles(bool isPlus)
{
    if (isPlus)
        countChosenFiles++;
    else
        countChosenFiles--;
}
void Panel::changeCountChosenFolders(bool isPlus)
{
    if (isPlus)
        countChosenFolders++;
    else
        countChosenFolders--;
}
void Panel::changeSize(bool isPlus, long long int delta)
{
    if (isPlus)
        size += delta;
    else
        size -= delta;
}
void Panel::changeCurrentFolderInfo(QString path, long long int sizeCurrentFolder, int filesCount, int foldersCount)
{
    this->path = path;
    this->currentDirSize = sizeCurrentFolder;
    this->countFiles = filesCount;
    this->countFolders = foldersCount;
    this->InfoToString();
    showPath(this->path);
}
void Panel::setIsleft(bool isLeft)
{
    this->isLeft = isLeft;
}
void Panel::setPath(QString path)
{
    this->path = path;
}

void Panel::ChangeFolderDB(folderid folder)
{

    items.clear();
    folders.clear();
    chosenItems.clear();
    chosenFolders.clear();
    functions_of_current_BD->GetFolderContents(folder, items, folders);
    int allCount = items.size() + folders.size();
    DBmodel->removeRows(0, DBmodel->rowCount());
    this->update();

    //ui->labelRightPath->setText(right_BD_path);

    //count_of_right_elements_DB = Current_Right_Folders.size() + Current_Right_Items.size();
    DBmodel->insertRows(0, allCount);
//    for (int i = 0; i < 10; i++)
//    {
//        items.push_back(new TIPInfo);
//        //folders.push_back(new TIPInfo);
//    }
    QStringList Coloumns_value;
    for (unsigned int i = 0; i < allCount; i++) {
        if (i < folders.size()) {
            Coloumns_value << folders[i]->second << " " << QString::number(folders[i]->first) << " " << " " << " " << " " << " ";
        }
        else
        {
            countFiles++;
            Coloumns_value << items[i % items.size()]->name << QString::number(items[i % items.size()]->sizeInTerms) << QString::number(items[i % items.size()]->id)  << items[i % items.size()]->ownerName << items[i % items.size()]->creationTime.toString();

                  if (items[i % items.size()]->compData1 == 0) {
                      Coloumns_value << " ";
                  }
                  else {
                      Coloumns_value << QString::number(items[i % items.size()]->compData1);
                  }

                  if (items[i % items.size()]->compData2 == 0) {
                      Coloumns_value << " ";
                  }
                  else {
                      Coloumns_value << QString::number(items[i % items.size()]->compData2);
                  }

                  if (items[i% items.size()]->compData3 == 0) {
                      Coloumns_value << " ";
                  }
                  else {
                      Coloumns_value << QString::number(items[i % items.size()]->compData3);
                  }
        }
        int j = 0;
        foreach (QString it, Coloumns_value) {
            QStandardItem *item = new QStandardItem(it);
            DBmodel->setItem(i, j, item);
            if(j == 3) {
                if(DBmodel->item(i, 3)->text() == " ") {
                    DBmodel->item(i, 0)->setIcon(QIcon("D:/resources/folder.png"));
                }
                else {
                    DBmodel->item(i, 0)->setIcon(QIcon("D:/resources/dbf.png"));
                }
            }
            j++;
        }
        Coloumns_value.clear();
    }

    if (folder != 1)
    {
        QStandardItem *back_item = new QStandardItem("..");
        DBmodel->insertRow(0, back_item);
    }

    this->setModel(DBmodel);
    //if (ui->RightChange->currentIndex() == 0) ui->RightChange->setCurrentIndex(1);
    //current_index = right_item_model->index(0, 0);
    this->currentDirSize = 0;
    for (int i = 0; i < items.size(); i++)
    {
        this->currentDirSize += items[i]->sizeInBytes / 1024;
    }
    countFolders = allCount - countFiles;
    return;
}
void Panel::setFileSystem(FileSystem * filesystem)
{
    this->setModel(filesystem);
    this->filesystem = filesystem;
    this->update();
}

bool Panel::getIfBD()
{
    return ifDB;
}

void Panel::setIfBD(bool ifDB)
{
    this->ifDB = ifDB;
}
TIPDBShell* Panel::getFunctionsDB()
{
    return functions_of_current_BD;
}
std::vector<TIPInfo*>* Panel::getItems()
{
    return &this->items;
}
std::vector<folderinfo*>* Panel::getFolders()
{
    return &this->folders;
}
void Panel::clearPanel()
{
    list.clear();
    this->setSelectionMode(QAbstractItemView::NoSelection);
    this->size = 0;
    this->countChosenFiles = 0;
    this->countChosenFolders = 0;
    current_folder_id = 0;
}
QStandardItemModel* Panel::getDB()
{
    return this->DBmodel;
}

void Panel::setCurrentFolder(folderid folder)
{
    this->current_folder_id = folder;
}
folderid Panel::getCurrentFolder()
{
    return this->current_folder_id;
}
QString Panel::cdUp(QString path)
{
    if (path == "/") return path;
    else
    {
        int last_slash = -1;
        QString buffer = path;
        for (int i = 0; i < path.size(); i++)
        {
            if (buffer[i] == '/') last_slash = i;
        }
        if (last_slash == 0)
        {
            buffer.remove(1, buffer.size()-1);
        }
        else
        {
            buffer.remove(last_slash, buffer.size()-last_slash);
        }
        return buffer;
    }
}
TIPInfo* Panel::findItem(folderid item)
{
    for (int i = 0; i < items.size(); i++)
    {
        if (items[i]->id == item)
            return items[i];
    }
    cout << "ERROR" << endl;
    return nullptr;
}
folderinfo* Panel::findFolder(folderid folder)
{
    for (int i = 0; i < folders.size(); i++)
    {
        if (folders[i]->first == folder)
            return folders[i];
    }
    cout << "ERROR" << endl;
    return nullptr;
}
void Panel::PushDB(QModelIndex index)
{
    if (!index.isValid())
        return;
    if (ifDB && DBmodel->item(index.row())->text() == "..")
    {
        return;
    }
    else if(ifDB && this->getPath() == "/" && DBmodel->item(index.row(), 1)->text() == " ")
    {
        chosenFolders.push_back(findFolder(DBmodel->item(index.row(), 2)->text().toInt()));
        this->countChosenFolders++;
    }
    else if (ifDB && this->getPath() == "/" && DBmodel->item(index.row(), 1)->text() != " ")
    {
        chosenItems.push_back(findItem(DBmodel->item(index.row(), 2)->text().toInt()));
        size += findItem(DBmodel->item(index.row(), 2)->text().toInt())->sizeInBytes/1024;
        this->countChosenFiles++;
    }
}
void Panel::RemoveDB(QModelIndex index)
{
    if (ifDB && DBmodel->item(index.row())->text() == "..")
    {
        return;
    }
    else if(ifDB && this->getPath() == "/" && DBmodel->item(index.row(), 1)->text() == " ")
    {
        chosenFolders.remove(findFolder(DBmodel->item(index.row(), 2)->text().toInt()));
        this->countChosenFolders--;
    }
    else if (ifDB && this->getPath() == "/" && DBmodel->item(index.row(), 1)->text() != " ")
    {
        chosenItems.remove(findItem(DBmodel->item(index.row(), 2)->text().toInt()));
        size += findItem(DBmodel->item(index.row(), 2)->text().toInt())->sizeInBytes/1024;
        this->countChosenFiles--;
    }
}
std::list <TIPInfo*>& Panel::getChosenItems()
{
    return chosenItems;
}

std::list <folderinfo*>& Panel::getChosenFolders()
{
    return chosenFolders;
}
FileSystem* Panel::getFilesystem()
{
    return this->filesystem;
}

void Panel::InfoToString()
{
    info.append(QString :: number(size) + " KB ");
    info.append("of " + QString :: number(currentDirSize) + " KB ");
    info.append("files " + QString:: number(countChosenFiles) + " of " + QString::number(countFiles) + " folders " +QString::number(countChosenFolders)+" of " + QString :: number(countFolders));
    emit showInfo(info);
    info.clear();
}
void Panel::clearInfo()
{
    size = 0;
    countChosenFiles = 0;
    countChosenFolders = 0;
    list.clear();
}

void Panel::arrowUp()
{
    if (!currentIndex().isValid())
    {
        setCurrentIndex(rootIndex().child(0,0));
    }
    if (selectionMode() == QAbstractItemView::SingleSelection)
    {
        selectionModel()->clearSelection();
        clearInfo();
        InfoToString();
        setSelectionMode(QAbstractItemView::NoSelection);
        if ((currentIndex().row()- 1) >= 0)
            setCurrentIndex(rootIndex().child(currentIndex().row()- 1,0));
        else
            setCurrentIndex(rootIndex().child(model()->rowCount(currentIndex().parent()) - 1,0));
    }
    else if (selectionMode() == QAbstractItemView::NoSelection)
    {
        selectionModel()->clearSelection();
        getList().clear();
        if ((currentIndex().row()- 1) >= 0)
            setCurrentIndex(rootIndex().child(currentIndex().row()- 1,0));
        else
            setCurrentIndex(rootIndex().child(model()->rowCount(currentIndex().parent()) - 1,0));
    }
    else if (selectionMode() == QAbstractItemView::MultiSelection)
    {
        if(currentIndex().row() - 1 >= 0)
        {
            setCurrentIndex(rootIndex().child(currentIndex().row()- 1,0));
            setCurrentIndex(currentIndex());
        }
        else
        {
            setCurrentIndex(rootIndex().child(model()->rowCount(currentIndex().parent()) - 1,0));
            setCurrentIndex(currentIndex());
        }
    }
}

void Panel::arrowDown()
{
    if (!currentIndex().isValid())
    {
        setCurrentIndex(rootIndex().child(0,0));
    }
    if (selectionMode() == QAbstractItemView::SingleSelection)
    {
        selectionModel()->clearSelection();
        clearInfo();
        InfoToString();
        setSelectionMode(QAbstractItemView::NoSelection);
        if ((currentIndex().row() + 1) < model()->rowCount(currentIndex().parent()))
            setCurrentIndex(rootIndex().child(currentIndex().row()+ 1,0));
        else
            setCurrentIndex(rootIndex().child(0,0));
    }
    else if (selectionMode() == QAbstractItemView::NoSelection)
    {
        selectionModel()->clearSelection();
        getList().clear();
        if ((currentIndex().row() + 1) < model()->rowCount(currentIndex().parent()))
            setCurrentIndex(rootIndex().child(currentIndex().row()+ 1,0));
        else
            setCurrentIndex(rootIndex().child(0,0));
    }
    else if (selectionMode() == QAbstractItemView::MultiSelection)
    {
        if ((currentIndex().row() + 1) < model()->rowCount(currentIndex().parent()))
        {
            setCurrentIndex(rootIndex().child(currentIndex().row()+ 1,0));
            setCurrentIndex(currentIndex());
        }
        else
        {
            setCurrentIndex(rootIndex().child(0,0));
            setCurrentIndex(currentIndex());
        }
    }
}
void Panel::refreshDB()
{
    this->ChangeFolderDB(current_folder_id);
    this->clearInfo();
    this->InfoToString();
    this->update();
    this->setSelectionMode(QAbstractItemView::NoSelection);
    this->selectionModel()->clear();
}
