#ifndef VIEWIP_H
#define VIEWIP_H

#include <QWidget>
#include <QString>

#include <vector>

#define HEADER "id\nСлово\nВес"

using namespace std;


namespace Ui {
class Widget;
}

struct IPTerm
{
    ulong			id;				//
    QString 		term;			//
    double			weight;			//

    IPTerm(ulong i, QString t, double w) {id = i, term = t, weight = w; }
};

struct IPShingle
{
    ulong			id;				//
    QString 		term;			//
    double			weight;			//

    IPShingle(ulong i, QString t, double w) {id = i, term = t, weight = w; }
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    void setData(QString id, QString user, QString date, QString comment, vector<IPTerm*> terms, vector<IPShingle*> shingles);


private:
    Ui::Widget *ui;

public slots:
//    void genData();
    void clear();
};

#endif // VIEWIP_H
