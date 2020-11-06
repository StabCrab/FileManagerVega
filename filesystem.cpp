#include "filesystem.h"

FileSystem::FileSystem(QObject *parent)
{
    this->setParent(parent);
}
void FileSystem:: copyFolder(QString sourceFolder, QString destFolder)
{
    QDir sourceDir(sourceFolder);
    if(!sourceDir.exists())
        return;
    QDir destDir(destFolder);
    if(!destDir.exists())
    {
        destDir.mkdir(destFolder);
    }
    QStringList files = sourceDir.entryList(QDir::Files);
    for(int i = 0; i < files.count(); i++)
    {
        QString srcName = sourceFolder + "/" + files[i];
        QString destName = destFolder + "/" + files[i];
        QFile::copy(srcName, destName);
    }
    files.clear();
    files = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    for(int i = 0; i < files.count(); i++)
    {
        QString srcName = sourceFolder + "/" + files[i];
        QString destName = destFolder + "/" + files[i];
        copyFolder(srcName, destName);
    }
}

bool FileSystem::copyIndex(QModelIndex index, QString destinationFilePath)
{
    QString sourceFilePath = this->fileInfo(index).absoluteFilePath();
    QFile sourceFile(sourceFilePath);
    QFile destinationFile(destinationFilePath);
    QFileInfo sourceFileInfo(sourceFile);
    QFileInfo destinationFileInfo(destinationFile);
    if (sourceFileInfo.absoluteFilePath() == destinationFileInfo.absoluteFilePath())
    {
        return false;
    }

    if(sourceFileInfo.fileName() == "." || sourceFileInfo.fileName() == "..")
       return false;

    if(sourceFileInfo.isFile() && sourceFileInfo.isReadable())
    {
        if(destinationFileInfo.isFile() && destinationFile.isWritable())
        {
            QString newFileName = destinationFile.fileName();
            destinationFile.remove();
            sourceFile.copy(newFileName);
        }
        else if(destinationFileInfo.isDir() && destinationFileInfo.isWritable())
        {
            QString newFileName = destinationFile.fileName();
            newFileName.append("/");
            newFileName.append(QFileInfo(sourceFile).fileName());
            sourceFile.copy(newFileName);
        }
    }
    else if(sourceFileInfo.isDir() && sourceFileInfo.isReadable())
    {
        destinationFilePath.append("/");
        destinationFilePath.append(QFileInfo(sourceFile).fileName());
        this->copyFolder(sourceFilePath, destinationFilePath);
    }
}
bool FileSystem::removeIndex(QModelIndex index)
{
    if (!this->isDir(index))
    {
        this->remove(index);
        return true;
    }
    else {
        QDir dir;
        dir.setPath(this->fileInfo(index).absoluteFilePath());
        removeFolder(dir);
        return true;
    }
}
bool FileSystem::removeFolder(QDir dir)
{
    bool result = false;
    //Получаем список каталогов
    QStringList lstDirs = dir.entryList(QDir::Dirs |
                    QDir::AllDirs |
                    QDir::NoDotAndDotDot);
    //Получаем список файлов
    QStringList lstFiles = dir.entryList(QDir::Files);

    //Удаляем файлы
    foreach (QString entry, lstFiles)
    {
     QString entryAbsPath = dir.absolutePath() + "/" + entry;
     QFile::setPermissions(entryAbsPath, QFile::ReadOwner | QFile::WriteOwner);
     QFile::remove(entryAbsPath);
    }

    //Для папок делаем рекурсивный вызов
    foreach (QString entry, lstDirs)
    {
     QString entryAbsPath = dir.absolutePath() + "/" + entry;
     QDir dr(entryAbsPath);
     removeFolder(dr);
    }

    //Удаляем обрабатываемую папку
    if (!QDir().rmdir(dir.absolutePath()))
    {
      result = true;
    }
    return result;
}
long long int FileSystem::getFolderSize(QString path)
{
    QFileInfo info(path);
    if (!info.isDir())
        return info.size() / 1024;
    QDir currentFolder( path );
    long long int size = 0;
    currentFolder.setFilter( QDir::Dirs | QDir::Files | QDir::NoSymLinks );
    currentFolder.setSorting( QDir::Name );

    QFileInfoList folderitems( currentFolder.entryInfoList() );

    foreach ( QFileInfo i, folderitems )
    {
        QString iname( i.fileName() );
        if ( iname == "." || iname == ".." || iname.isEmpty() )
            continue;
        if ( !i.isDir())
            size += i.size();
    }
    return size / 1024;
}
