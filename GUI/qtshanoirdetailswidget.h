#ifndef DETAILSWIDGET_H
#define DETAILSWIDGET_H

#include <QtGui>
#include "ui_qtshanoirdetaislwidget.h"

namespace Ui
{
class QtShanoirDetailsWidget;
}

class QtShanoirDetailsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QtShanoirDetailsWidget(QWidget *parent = 0);
public slots:
    void displayStudyDetails(QString);
    void displaySubjectDetails(int,int);
    void displayExamDetails(int,int,int);
    void displayDatasetDetails(int,int,int,int);
    void displayProcessDetails(int,int,int,int,int);
    void displayProcessedDatasetDetails(int,int,int,int,int,int);
private:
    Ui::QtShanoirDetailsWidget *ui;
};


#endif // DETAILSWIDGET_H
