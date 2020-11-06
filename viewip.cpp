#include "viewip.h"
#include "ui_viewip.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->tb_terms->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tb_terms->horizontalHeader()->setStretchLastSection(true);
    ui->tb_terms->verticalHeader()->hide();

    ui->tb_shingles->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tb_shingles->horizontalHeader()->setStretchLastSection(true);
    ui->tb_shingles->verticalHeader()->hide();

    QStringList horHeader = QString(HEADER).split("\n");

    ui->tb_terms->setColumnCount(3);
    ui->tb_terms->setHorizontalHeaderLabels(horHeader);

    ui->tb_shingles->setColumnCount(3);
    ui->tb_shingles->setHorizontalHeaderLabels(horHeader);

    int iColWidths[3] = {100, 150, 100};
    for (int i = 0; i < 3; ++i)
    {
        ui->tb_terms->setColumnWidth(i, iColWidths[i]);
        ui->tb_shingles->setColumnWidth(i, iColWidths[i]);
    }

    connect(ui->pb_set, SIGNAL(clicked()), this, SLOT(genData()));
    connect(ui->pb_clear, SIGNAL(clicked()), this, SLOT(clear()));
}

Widget::~Widget()
{
    delete ui;
}
/*
std::string random_string( size_t length )
{
    auto randchar = []() -> char
    {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(length,0);
    std::generate_n( str.begin(), length, randchar );
    return str;
}

void Widget::genData()
{
    vector<IPTerm*> terms;
    for(int i = 0; i < rand() % 100; i++)
    {
        IPTerm *p = new IPTerm(static_cast<ulong>(i+1), QString(random_string(20).c_str()), (double)rand() / RAND_MAX);
        terms.push_back(p);
    }
    vector<IPShingle*> shingles;
    for(int i = 0; i < rand() % 100; i++)
    {
        IPShingle *p = new IPShingle(static_cast<ulong>(i+1), QString(random_string(15).c_str()), (double)rand() / RAND_MAX);
        shingles.push_back(p);
    }
    setData("256", "ПОЛЬЗОВАТЕЛЬ", "11.02", "Комментарий", terms, shingles);
}
*/
void Widget::setData(QString id, QString user, QString date, QString comment, vector<IPTerm*> terms, vector<IPShingle*> shingles)
{
    ui->edit_id->setText(id);
    ui->edit_user->setText(user);
    ui->edit_date->setText(date);
    ui->edit_com->setText(comment);


    for(uint i = 0; i < terms.size(); i++)
    {
         if(ui->tb_terms->rowCount() <= i) ui->tb_terms->insertRow(i);
         ui->tb_terms->setItem(i,0, new QTableWidgetItem(QString("%1").arg(terms[i]->id)));
         ui->tb_terms->setItem(i,1, new QTableWidgetItem(terms[i]->term));
         ui->tb_terms->setItem(i,2, new QTableWidgetItem(QString("%1").arg(terms[i]->weight)));
    }

    for(uint i = 0; i < shingles.size(); i++)
    {
        if(ui->tb_shingles->rowCount() <= i) ui->tb_shingles->insertRow(i);
        ui->tb_shingles->setItem(i,0, new QTableWidgetItem(QString("%1").arg(shingles[i]->id)));
        ui->tb_shingles->setItem(i,1, new QTableWidgetItem(shingles[i]->term));
        ui->tb_shingles->setItem(i,2, new QTableWidgetItem(QString("%1").arg(shingles[i]->weight)));
    }
}

void Widget::clear()
{
    ui->edit_id->clear();
    ui->edit_user->clear();
    ui->edit_date->clear();
    ui->edit_com->clear();
    ui->tb_terms->clearContents();
    ui->tb_shingles->clearContents();
}
