#include "qtshanoirtree.h"
#include "qtshanoirmain.h"
#include "parsetree.h"

QtShanoirTree::QtShanoirTree()
{

}

QtShanoirTree* QtShanoirTree::buildTree()
{
    QtShanoirTree* tree;
//    QtShanoirMain* servicePopulate = new QtShanoirMain();
//    ParseTree* parse = new ParseTree();
//    QString xmlSerializer;

//    //Build Study_Subject
//    xmlSerializer = servicePopulate->acquireStudy_Subject();
//    tree = parse->parseStudy_Subject(xmlSerializer);


////    //Build Exam

////    for( int i=0; i<tree->studyList.size();i++)
////    {
////        qDebug()<<"jawww";
////        tree->studyList.value(i).acquireRefreshExamListAllSubject();
////    }


//   //for (int i=0; tree->studyList.size();i++);
////    {
////        for (int j=0; tree->studyList.value(i).subjectList.size();j++)
////         xmlSerializer = servicePopulate->acquireExam(studyList.value(i).subjectList.value(j).getId().toString());
//        // qDebug()<<tree->studyList.size();
////    }

////    ParseTree parse_exam;
////    QString xmlSerializer;
////    QtShanoirMain servicePopulate;
////    tree = parsestudy->parseStudy_Subject(xmlSerializer);
////   for (int i=0; i<studyList.size();i++)
////       for (int j=0; j<studyList.value(i).subjectList.size();j++)
////       {
////           xmlSerializer = servicePopulate->acquireExam();
////           parse_exam(xmlSerializer,studyList.value(i).subjectList.value(j).getId());
////       }

return tree;

}

void QtShanoirTree::acquireRefreshExamList(int idStudy,int idSubject)
{
    QtShanoirMain* acquireExam =  new QtShanoirMain();
    ParseTree* parse = new ParseTree();
    QString xmlSerializer;
    xmlSerializer = acquireExam->acquireExam(QString::number(idSubject));
    parse->parseExam(xmlSerializer,idStudy,idSubject,this);
}

void QtShanoirTree::acquireRefreshDatasetList(int idStudy, int idSubject, int idExam)
{
    QtShanoirMain* acquireDataset =  new QtShanoirMain();
    ParseTree* parse = new ParseTree();
    QString xmlSerializer;
    xmlSerializer = acquireDataset->acquireDataset(QString::number(idSubject),QString::number(idExam));
    parse->parseDataset(xmlSerializer,idStudy,idSubject,idExam,this);
}

void QtShanoirTree::acquireRefreshProcess_processedDatasetList(int idStudy, int idSubject, int idExam, int idDataset)
{
    QtShanoirMain* acquireProcess_ProcessedDataset =  new QtShanoirMain();
    ParseTree* parse = new ParseTree();
    QString xmlSerializer;
    xmlSerializer = acquireProcess_ProcessedDataset->acquireProcess_ProcessedDataset(QString::number(idDataset));
    parse->parseProcess_ProcessedDataset(xmlSerializer,idStudy,idSubject,idExam,idDataset,this);
}

QString QtShanoirTree::getServer()
{
    return this->server;
}

void QtShanoirTree::setServer(QString server)
{
    this->server = server;
}

void QtShanoirTree::insert(QtShanoirStudy study)
{
   this->studyList.append(study);
}

void QtShanoirTree::replaceStudy(int idStudy,QtShanoirStudy study)
{
   this->studyList.replace(idStudy, study);
}


void QtShanoirTree::insertExam(QtShanoirExam exam,int idStudy, int idSubject)
{
//    getStudyById(idStudy).getSubjectById(idSubject).insertExam(exam);
//    getIdStudyIdSubject(&idStudy,&idSubject, subjectName);
//    getStudyById(idStudy).insertExam(exam,idSubject);

}

void QtShanoirTree::displayStudyList()
{
    for (int i=0; i<studyList.size();i++)
       qDebug()<<"Study:"<<studyList.value(i).getName();
}

QMap<int,QString> QtShanoirTree::getStudyList(QString studyFilter)
{
    QMap<int,QString> list;
    for (int i=0; i<studyList.size();i++)
    {
        if ((studyFilter=="")||(studyList.value(i).getName().contains(studyFilter)))
        {
            qDebug()<<studyList.value(i).getName();
            list.insert(studyList.value(i).getId(),studyList.value(i).getName());
        }
    }

    return list;
}

void QtShanoirTree::displayAllTree()
{
    for (int i=0; i<studyList.size();i++)
   {
       qDebug()<<"Study:"<<studyList.value(i).getName();
      // studyList.value(i).displaySubjectList();
       studyList.value(i).displaySubjectList_exam();
   }
}

QMap<int,QString> QtShanoirTree::getSubjectList(QString study, QString subjectFilter)
{
    displayAllTree();
    QMap<int,QString> list;
    int i = 0;
    bool find = false;
    while (!find && i<studyList.size())
    {
        if (studyList.value(i).getName()==study)
            find = true;
        i++;
    }
    i = i-1;
    if (find)
    {
        displayStudyList();
            studyList.value(i).displaySubjectList();
           // if (filter.exactMatch(studyList.value(i).getName()))
        list = studyList.value(i).getSubjectList(subjectFilter);
    }

    return list;
}

QtShanoirStudy& QtShanoirTree::getStudyById(int id)
{
    QtShanoirStudy object;
    for(int i=0;i<studyList.size();i++)
        if(studyList.value(i).getId()== id)
            return studyList[i];
}

QtShanoirStudy QtShanoirTree::getStudyByName(QString name)
{
    for(int i=0;i<studyList.size();i++)
        if(studyList.value(i).getName()== name)
            return studyList.value(i);
}

void QtShanoirTree::getIdStudyIdSubject(int *idStudy,int *idSubject, QString subjectName)
{
    QtShanoirStudy study;
    QtShanoirSubject subject = study.getSubjectByName(subjectName);
    *idSubject = subject.getId();
    *idStudy = subject.getIdStudy();
}

QMap<int,QString> QtShanoirTree::getExamList(int idStudy,int idSubject)
{
    return getStudyById(idStudy).getSubjectById(idSubject).getExamList();
}

QMap<int,QString> QtShanoirTree::getDatasetList(int idStudy, int idSubject, int idExam)
{
    return getStudyById(idStudy).getSubjectById(idSubject).getExamById(idExam).getDatasetList();
}

QMap<int,QString> QtShanoirTree::getDatasetList(int idStudy, int idSubject, int idExam,const QString & queryField)
{
    return getStudyById(idStudy).getSubjectById(idSubject).getExamById(idExam).getDatasetListByField(queryField);
}


QMap<int,QString> QtShanoirTree::getProcessList(int idStudy, int idSubject, int idExam, int idDataset)
{
    return getStudyById(idStudy).getSubjectById(idSubject).getExamById(idExam).getDatasetById(idDataset).getProcessList();
}


QMap<int,QString> QtShanoirTree::getProcessedDatasetList(int idStudy, int idSubject, int idExam, int idDataset, int idProcess)
{
    return getStudyById(idStudy).getSubjectById(idSubject).getExamById(idExam).getDatasetById(idDataset).getProcessById(idProcess).getProcessedDatasetList();
}
