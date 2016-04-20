#include "qtshanoirsubject.h"
#include "qtshanoirmain.h"


int QtShanoirSubject::getId()
{
    return this->id;
}

void QtShanoirSubject::setId(int id)
{
    this->id = id;
}

int QtShanoirSubject::getIdStudy()
{
    return this->idStudy;
}

void QtShanoirSubject::setIdStudy(int idStudy)
{
    this->idStudy = idStudy;
}

QString QtShanoirSubject::getName()
{
    return this->name;
}

void QtShanoirSubject::setName(QString name)
{
    this->name = name;
}

QDate QtShanoirSubject::getBirthDate()
{
    return this->birthDate;
}

void QtShanoirSubject::setBirthDate(QDate date)
{
    this->birthDate = date;
}

QChar QtShanoirSubject::getSexe()
{
    return this->sexe;
}

void QtShanoirSubject::setSexe(QChar sexe)
{
    this->sexe = sexe;
}

QtShanoirExam& QtShanoirSubject::getExamById(int id)
{
    for (int i=0; i<examList.size();i++)
        if(examList.value(i).getId()== id)
            return examList[i];
}

void QtShanoirSubject::insertExam(QtShanoirExam exam)
{
    this->examList.append(exam);
}

void QtShanoirSubject::displayExamList()
{
    for (int i=0; i<examList.size();i++)
      qDebug()<<"Exam:"<<examList.value(i).getComment();
}

QMap<int,QString> QtShanoirSubject::getExamList()
{
    QMap<int,QString> list;

    for (int i=0; i<examList.size();i++)
    {
        list.insert(examList.value(i).getId(),examList.value(i).getComment());
    }
    return list;
}

void QtShanoirSubject::acquireRefreshExamList()
{
//    //examList.clear();
//    qDebug()<<id;
//    QtShanoirMain* acquireExam =  new QtShanoirMain();
//    ParseTree* parse = new ParseTree();
//    QString xmlSerializer;
//    xmlSerializer = acquireExam->acquireExam(QString::number(getId()));
//    qDebug()<<"hell13o";
//    qDebug()<<"xmlSerializer"<<xmlSerializer;

//    parse->parseExam(xmlSerializer,this);
//this->displayExamList();
}

int QtShanoirSubject::sizeExamList()
{
    return examList.size();
}
