#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QFileSystemModel>
#include <QMainWindow>

class FileSystem : public QFileSystemModel
{
private:

public:
    FileSystem(QObject *parent = 0);
    bool copyIndex(QModelIndex index, QString destination);
    void copyFolder(QString sourceFolder, QString destFolder);
    bool removeIndex (QModelIndex index);
    bool removeFolder(QDir dir);
    long long int getFolderSize(QString path);
};

#endif // FILESYSTEM_H
