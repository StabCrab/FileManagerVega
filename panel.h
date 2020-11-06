#ifndef PANEL_H
#define PANEL_H
#include <QFileSystemModel>
#include <QTreeView>
#include<QInputDialog>
#include<QStringList>
#include <QKeyEvent>
#include <QShortcut>
#include <QFile>
#include <QTreeWidgetItem>
#include "QStandardItemModel"
#include "tipdbshell.h"
#include <filesystem.h>
#include <iostream>

class Panel : public QTreeView
{
    Q_OBJECT
private:
    QString path; // путь, который отображает панель
    bool ifDB;
    QString info; // информация под панелью
    QModelIndexList list; // список выделенных индексов
    int countChosenFolders, countChosenFiles; // количество выбранных элементов. Нужно для информации под панелью
    int countFolders, countFiles;
    long long int size; // размер выбранных файлов. Тоже нужно для информации под панелью
    bool isLeft;
    long long int currentDirSize;
    FileSystem *filesystem;
    QStandardItemModel *DBmodel;
    std::vector <TIPInfo*> items;
    std::vector <folderinfo*> folders;
    std::list <TIPInfo*> chosenItems;
    std::list <folderinfo*> chosenFolders;
    unsigned int count_elements_DB;
    TIPDBShell *functions_of_current_BD;
    folderid current_folder_id = 0;
    std::list<folderid> pathID;
signals:
    void updateInfo(bool isLeft, bool ifPlus, QModelIndex index);
    void showInfo(QString);
    void showPath(QString);
    void changeFolder(bool isLeft, QModelIndex index);
    void createDir(bool, QString);
public:
    Panel(QWidget *parent); // Конструктор
public slots:
    QString getPath(); // получаем путь
    QString getInfo(); // получаем информацию под панелью
    QModelIndexList getList(); // получаем список выбранных индексов
    void chooseButton ();
    void choose(QModelIndex index); // в этом методе определяем выбран
    void changeDirectory(QModelIndex index); // меняем директорию на панели
    void changeSelectionMode(); //меняем режим выбора на панели(один элемент или несколько)
    void changeCountChosenFiles(bool isPlus);
    void changeCountChosenFolders(bool isPlus);
    void changeSize(bool isPlus, long long int delta);
    void changeCurrentFolderInfo(QString path, long long int sizeCurrentFolder, int filesCount, int foldersCount);
    void setIsleft(bool isLeft);
    void setPath(QString path);
    void setFileSystem(FileSystem *filesystem);
    void ChangeFolderDB(folderid folder);
    bool getIfBD();
    void setIfBD(bool ifDB);
    TIPDBShell* getFunctionsDB();
    std::vector<TIPInfo*>* getItems();
    std::vector<folderinfo*>* getFolders();
    void clearPanel();
    QStandardItemModel* getDB();
    folderid getCurrentFolder();
    void setCurrentFolder(folderid folder);
    QString cdUp(QString path);
    folderinfo* findFolder(folderid folder);
    TIPInfo* findItem(folderid folder);
    void PushDB(QModelIndex index);
    void RemoveDB(QModelIndex index);
    std::list <TIPInfo*>& getChosenItems();
    std::list <folderinfo*>& getChosenFolders();
    FileSystem* getFilesystem();
    void InfoToString();
    void clearInfo();
    void arrowUp();
    void arrowDown();
    void refreshDB();
};

#endif // PANEL_H
