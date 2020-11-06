#ifndef WORKSPACE_H
#define WORKSPACE_H
#include <panel.h>
#include<filesystem.h>
#include<QMessageBox>
#include <iostream>
enum class Button {
    Copy = 0,
    Move,
    MkDir,
    Remove,
};
class Workspace : public QObject
{
    Q_OBJECT
private:
    Panel *left; // левая панель
    Panel *right;// правая панель
    bool isLeftCurrent; // с помощью этой переменной определяем текущую панель
    FileSystem *filesystem; //файловая система    
public:
    Workspace(Panel *left, Panel *right, FileSystem *filesystem); //конструктор
public slots:
    bool getIsLeftCurrent(); // получаем информацию о текущей панели
    void choose(); // выбор элемента
    void remove(); // удаление элемента
    void copy(); // копирование элемента
    void move();
    void updateInfo(bool isLeft, bool isPlus, QModelIndex index);
    void updateInfoDB(bool isLeft, bool isPlus);
    void updateFolder(bool isLeft, QString path);
    void indexToString(bool isLeft, QModelIndex index);
    void changeSelectionMode();
    void changeCurrentPanel();
    void createDir();
    void changeDir();
};
#endif // WORKSPACE_H
