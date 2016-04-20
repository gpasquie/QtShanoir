#include "qtshanoirdataset.h"

int QtShanoirDataset::getId()
{
    return this->id;
}

void QtShanoirDataset::setId(int id)
{
    this->id = id;
}

QString QtShanoirDataset::getName()
{
    return this->name;
}

QString QtShanoirDataset::getField(const QString & field)
{
    if(field=="name")
        return getName();
    if(field=="filter")
        return getFilter();
    if(field=="echoNumber")
        return QString::number(getEchoNumber());
    else return "";
}

int QtShanoirDataset::getEchoNumber()
{
    return echoNumber;
}

void QtShanoirDataset::setEchoNumber(int number)
{
    this->echoNumber=number;
}

void QtShanoirDataset::setName(QString name)
{
    this->name = name;
}

QDate QtShanoirDataset::getCreationDate()
{
    return this->creationDate;
}

void QtShanoirDataset::setCreationDate(QDate date)
{
    this->creationDate = date;
}

QString QtShanoirDataset::getComment()
{
    return this->comment;
}

void QtShanoirDataset::setComment(QString comment)
{
    this->comment = comment;
}

float QtShanoirDataset::getRepetitionTime()
{
    return this->repetitionTime;
}

void QtShanoirDataset::setRepetitionTime(float occurence)
{
    this->repetitionTime = occurence;
}

float QtShanoirDataset::getFlipAngle()
{
    return this->flipAngle;
}

void QtShanoirDataset::setFlipAngle(float angle)
{
    this->flipAngle = angle;
}

QString QtShanoirDataset::getFilter()
{
    return this->filter;
}

void QtShanoirDataset::setFilter(QString filterName)
{
    this->filter = filterName;
}

float QtShanoirDataset::getEchoTime()
{
    return this->echoTime;
}

void QtShanoirDataset::setEchoTime(float echo)
{
    this->echoTime = echo;
}

QtShanoirProcess& QtShanoirDataset::getProcessById(int id)
{

    for (int i=0; i<processList.size();i++)
        if(processList.value(i).getId()== id)
            return processList[i];
}

void QtShanoirDataset::insertProcess(QtShanoirProcess process)
{
    this->processList.append(process);
}

void QtShanoirDataset::displayProcessList()
{
    for (int i=0; i<processList.size();i++)
      qDebug()<<"Process:"<<processList.value(i).getName();
}

QMap<int,QString> QtShanoirDataset::getProcessList()
{
    QMap<int,QString> list;
    for (int i=0; i<processList.size();i++)
    {
       // if (filter.exactMatch(studyList.value(i).getName()))
           list.insert(processList.value(i).getId(),processList.value(i).getName());
    }
    return list;
}
