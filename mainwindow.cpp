#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    filesystem = new FileSystem(this);
    filesystem->setFilter(QDir::NoDot | QDir::AllEntries | QDir::System);
    filesystem->setRootPath("");
    filesystem->setReadOnly(false);
    QFileInfoList drives = QDir::drives();
    QFileInfo firstDrive = drives[0];
    ui->leftPanel->setIfBD(false);
    ui->rightPanel->setIfBD(false);
    std::vector<string> DBnamespace;
    inifile = new TIniFile("settings.ini");
    foreach (QFileInfo drive, drives)
    {
            ui->leftBox->addItem(drive.filePath());
            ui->rightBox->addItem(drive.filePath());
    }

    ui->labelLeftPath->setText("  " + firstDrive.path());
    ui->labelRightPath->setText("  " + firstDrive.path());

    ui->leftPanel->setModel(filesystem);
    ui->leftPanel->setRootIndex(filesystem->index(firstDrive.absoluteFilePath()));
    ui->rightPanel->setModel(filesystem);
    ui->rightPanel->setRootIndex(filesystem->index(firstDrive.absoluteFilePath()));

    ui->leftPanel->setIsleft(true);
    ui->rightPanel->setIsleft(false);

    ui->leftPanel->setFileSystem(filesystem);
    ui->rightPanel->setFileSystem(filesystem);

    ui->leftPanel->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->rightPanel->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->leftPanel->header()->resize(1, QHeaderView::ResizeToContents);
    ui->rightPanel->header()->resize(1, QHeaderView::ResizeToContents);
    ui->leftPanel->header()->resize(2, 100);
    ui->rightPanel->header()->resize(2, 100);
    ui->leftPanel->header()->resize(3, 100);
    ui->rightPanel->header()->resize(3, 100);

    workspace = new Workspace(ui->leftPanel, ui->rightPanel, filesystem);

    for (int i = 0; i < inifile->GetSectionCount(); ++i)
    {
        ui->leftBox->addItem(QString::fromStdString(inifile->GetSections()[i]));
        ui->rightBox->addItem(QString::fromStdString(inifile->GetSections()[i]));
    }
    tab = new QShortcut(this);
    tab->setKey(Qt::Key_Tab);
    backspace = new QShortcut(this);
    backspace->setKey(Qt::Key_Backspace);
    insert = new QShortcut(this);
    insert->setKey(Qt::Key_Insert);
    enter = new QShortcut(this);
    enter->setKey(Qt::Key_Return);
    up = new QShortcut(this);
    up->setKey(Qt::Key_Up);
    down = new QShortcut(this);
    down->setKey(Qt::Key_Down);

    connect(ui->viewButton, SIGNAL(clicked(bool)), workspace, SLOT(changeDir()));
    connect(ui->deleteButton, SIGNAL(clicked(bool)), workspace, SLOT(remove()));
    connect(ui->copyButton, SIGNAL(clicked(bool)), workspace, SLOT(copy()));
    connect(ui->moveButton, SIGNAL(clicked(bool)), workspace, SLOT(move()));
    connect(ui->createButton, SIGNAL(clicked(bool)), workspace, SLOT(createDir()));

    connect(ui->leftPanel, SIGNAL(clicked(QModelIndex)), ui->leftPanel, SLOT(choose(QModelIndex)));
    connect(ui->rightPanel, SIGNAL(clicked(QModelIndex)), ui->rightPanel, SLOT(choose(QModelIndex)));
    connect(ui->leftPanel, SIGNAL(doubleClicked(QModelIndex)), ui->leftPanel, SLOT(changeDirectory(QModelIndex)));
    connect(ui->rightPanel, SIGNAL(doubleClicked(QModelIndex)), ui->rightPanel, SLOT(changeDirectory(QModelIndex)));

    connect(ui->leftPanel, SIGNAL(showInfo(QString)), ui->leftInfo, SLOT(setText(QString)));
    connect(ui->rightPanel, SIGNAL(showInfo(QString)), ui->rightInfo, SLOT(setText(QString)));
    connect(ui->leftPanel, SIGNAL(showPath(QString)), ui->labelLeftPath, SLOT(setText(QString)));
    connect(ui->rightPanel, SIGNAL(showPath(QString)), ui->labelRightPath, SLOT(setText(QString)));

    connect(insert, SIGNAL(activated()), workspace, SLOT(choose()));
    connect(backspace, SIGNAL(activated()), workspace, SLOT(changeSelectionMode()));
    connect(tab, SIGNAL(activated()), workspace, SLOT(changeCurrentPanel()));

    connect(enter, SIGNAL(activated()), workspace, SLOT(changeDir()));

    connect(down, SIGNAL(activated()), ui->leftPanel, SLOT(arrowDown()));
    connect(down, SIGNAL(activated()), ui->rightPanel, SLOT(arrowDown()));
    connect(up, SIGNAL(activated()), ui->leftPanel, SLOT(arrowUp()));
    connect(up, SIGNAL(activated()), ui->rightPanel, SLOT(arrowUp()));


    ui->copyButton->setShortcut(Qt::Key_F5);
    ui->createButton->setShortcut(Qt::Key_F7);
    ui->moveButton->setShortcut(Qt::Key_F6);
    ui->deleteButton->setShortcut(Qt::Key_F8);
    ui->sortButton->setShortcut(Qt::Key_F12);


    workspace->updateFolder(true, firstDrive.path());
    workspace->updateFolder(false, firstDrive.path());

    ui->leftPanel->setItemsExpandable(false);
    ui->leftPanel->setRootIsDecorated(false);
    ui->leftPanel->setAllColumnsShowFocus(true);
    ui->rightPanel->setItemsExpandable(false);
    ui->rightPanel->setRootIsDecorated(false);
    ui->rightPanel->setAllColumnsShowFocus(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_leftBox_activated(const QString &arg)
{
    bool ifContains = false;
    for (int i = 0; i < inifile->GetSectionCount(); i++)
    {
        if (arg.toStdString() == inifile->GetSections()[i])
        {
            ifContains = true;
            break;
        }
    }

    if (ifContains)
    {
        ui->leftPanel->ChangeFolderDB(1);
        ui->leftPanel->setIfBD(true);
        ui->labelLeftPath->setText("/");
        ui->leftPanel->setPath("/");
        ui->leftPanel->header()->setSectionResizeMode(0, QHeaderView::Stretch);
        ui->leftPanel->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        ui->leftPanel->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
        ui->leftPanel->setColumnWidth(3, 90);
        ui->leftPanel->setColumnWidth(4, 220);
        ui->leftPanel->header()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
        ui->leftPanel->header()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
        ui->leftPanel->header()->setSectionResizeMode(7, QHeaderView::ResizeToContents);
    }
    else
    {
        ui->leftPanel->getFunctionsDB()->Init(arg);
        ui->leftPanel->setIfBD(false);
        ui->leftPanel->setFileSystem(filesystem);
        ui->leftPanel->setRootIndex(filesystem->index(arg));
        ui->labelLeftPath->setText(arg);
        ui->leftPanel->update();

        ui->leftPanel->header()->setSectionResizeMode(0, QHeaderView::Stretch);
        ui->leftPanel->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        ui->leftPanel->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
        ui->leftPanel->header()->setSectionResizeMode(3, QHeaderView::Custom);



    }
    ui->leftPanel->clearPanel();
}


void MainWindow::on_rightBox_activated(const QString &arg)
{
    bool ifContains = false;
    for (int i = 0; i < inifile->GetSectionCount(); i++)
    {
        if (arg.toStdString() == inifile->GetSections()[i])
        {
            ifContains = true;
            break;
        }
    }
    if (ifContains)
    {
        ui->rightPanel->getFunctionsDB()->Init(arg);
        ui->rightPanel->setPath("/");
        ui->labelRightPath->setText("/");
        ui->rightPanel->setIfBD(true);
        ui->rightPanel->ChangeFolderDB(1);
        ui->rightPanel->header()->setSectionResizeMode(0, QHeaderView::Stretch);
        ui->rightPanel->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        ui->rightPanel->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
        ui->rightPanel->setColumnWidth(3, 90);
        ui->rightPanel->setColumnWidth(4, 220);
        ui->rightPanel->header()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
        ui->rightPanel->header()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
        ui->rightPanel->header()->setSectionResizeMode(7, QHeaderView::ResizeToContents);
    }
    else
    {
        ui->rightPanel->setIfBD(false);
        ui->rightPanel->setFileSystem(filesystem);
        ui->rightPanel->setRootIndex(filesystem->index(arg));
        ui->labelRightPath->setText(arg);
        ui->rightPanel->update();
        ui->rightPanel->header()->setSectionResizeMode(0, QHeaderView::Stretch);
        ui->rightPanel->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        ui->rightPanel->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
        ui->rightPanel->header()->setSectionResizeMode(3, QHeaderView::Custom);
    }
    ui->rightPanel->clearPanel();
}

void MainWindow::on_exitButton_clicked()
{
    exit(1);
}
