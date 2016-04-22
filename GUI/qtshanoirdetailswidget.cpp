#include "qtshanoirdetailswidget.h"
#include "qtshanoirstudy.h"
#include "dao.h"

#include <QLabel>

QtShanoirDetailsWidget::QtShanoirDetailsWidget(QWidget *parent) : QWidget(parent),ui (new Ui::QtShanoirDetailsWidget)
{
    ui->setupUi(this);
}

void QtShanoirDetailsWidget::displayStudyDetails(QString studyName)
{
    Study* study = getStudyDetails(studyName);

    //Clear GridLayout
    while(ui->gridLayout->count())
    {
        QWidget* widget = ui->gridLayout->itemAt(0)->widget();
        if(widget)
        {
            ui->gridLayout->removeWidget(widget);
            delete widget;
        }
    }

    QImage image;
    image.load(":Images/study.64x64.png");
    QLabel* imageLabel = new QLabel();
    imageLabel->setPixmap(QPixmap::fromImage(image));
    QLabel *studyNameLabel = new QLabel("Name :");
    QLabel *studyNameLabelValue = new QLabel(study->name);

    ui->gridLayout->addWidget(imageLabel,0,0);
    ui->gridLayout->addWidget(studyNameLabel,0,1);
    ui->gridLayout->addWidget(studyNameLabelValue,0,2);
    setLayout(ui->gridLayout);
}

void QtShanoirDetailsWidget::displaySubjectDetails(int idStudy, int idSubject)
{
    Subject* subject = getSubjectDetails(idStudy,idSubject);

    //Clear GridLayout
    while(ui->gridLayout->count())
    {
        QWidget* widget = ui->gridLayout->itemAt(0)->widget();
        if(widget)
        {
            ui->gridLayout->removeWidget(widget);
            delete widget;
        }
    }

    QImage image;
    image.load(":Images/subject.64x64.png");
    QLabel* labelImage = new QLabel();
    labelImage->setPixmap(QPixmap::fromImage(image));

    QLabel *subjectNameLabel = new QLabel("Name :");
    QLabel *subjectName = new QLabel(subject->name);
    QLabel *subjectBirthLabel = new QLabel("Birth Date :");
    QLabel *subjectBirth = new QLabel(subject->birthDate.toString(Qt::ISODate));
    QLabel *subjectSexeLabel= new QLabel("Sexe :");
    QLabel *subjectSexe = new QLabel(subject->sexe);

    ui->gridLayout->addWidget(labelImage,0,0,3,0);
    ui->gridLayout->setColumnMinimumWidth(0,70);
    ui->gridLayout->addWidget(subjectNameLabel,0,1);
    ui->gridLayout->addWidget(subjectName,0,2);
    ui->gridLayout->addWidget(subjectBirthLabel,1,1);
    ui->gridLayout->addWidget(subjectBirth,1,2);
    ui->gridLayout->addWidget(subjectSexeLabel,2,1);
    ui->gridLayout->addWidget(subjectSexe,2,2);

}

void QtShanoirDetailsWidget::displayExamDetails(int idStudy, int idSubject, int idExam)
{
    Exam* exam = getExamDetails(idStudy,idSubject,idExam);

    //Clear GridLayout
    while(ui->gridLayout->count())
    {
        QWidget* widget = ui->gridLayout->itemAt(0)->widget();
        if(widget)
        {
            ui->gridLayout->removeWidget(widget);
            delete widget;
        }
    }

    QImage image;
    image.load(":Images/examination.64x64.png");
    QLabel* labelImage = new QLabel();
    labelImage->setPixmap(QPixmap::fromImage(image));

    QLabel *examNameLabel = new QLabel("Name :");
    QLabel *examName = new QLabel(exam->comment);
    QLabel *examDateLabel = new QLabel("Date :");
    QLabel *examDate = new QLabel(exam->date.toString(Qt::ISODate));

    ui->gridLayout->addWidget(labelImage,0,0,2,0);
    ui->gridLayout->setColumnMinimumWidth(0,70);
    ui->gridLayout->addWidget(examNameLabel,0,1);
    ui->gridLayout->addWidget(examName,0,2);
    ui->gridLayout->addWidget(examDateLabel,1,1);
    ui->gridLayout->addWidget(examDate,1,2);

}

void QtShanoirDetailsWidget::displayDatasetDetails(int idStudy, int idSubject, int idExam, int idDataset)
{
    struct Dataset* dataset = getDatasetDetails(idStudy,idSubject,idExam,idDataset);

    //Clear GridLayout
    while(ui->gridLayout->count())
    {
        QWidget* widget = ui->gridLayout->itemAt(0)->widget();
        if(widget)
        {
            ui->gridLayout->removeWidget(widget);
            delete widget;
        }
    }

    QImage image;
    image.load(":Images/dataset.64x64.png");
    QLabel* labelImage = new QLabel();
    labelImage->setPixmap(QPixmap::fromImage(image));

    QLabel *datasetNameLabel = new QLabel("Name :");
    QLabel *datasetName = new QLabel(dataset->name);
    QLabel *datasetCreationDateLabel = new QLabel("Creation Date :");
    QLabel *datasetCreationDate = new QLabel(dataset->creationDate.toString(Qt::ISODate));
    QLabel *datasetCommentLabel = new QLabel("Comment :");
    QLabel *datasetComment = new QLabel(dataset->comment);
    QLabel *datasetRepetitionTimeLabel = new QLabel("Repetition Time :");
    QLabel *datasetRepetitionTime = new QLabel(QString::number(dataset->repetitionTime));
    QLabel *datasetFlipAngleLabel = new QLabel("Flip Angle :");
    QLabel *datasetFlipAngle = new QLabel(QString::number(dataset->flipAngle));
    QLabel *datasetEchoTimeLabel = new QLabel("Echo Time :");
    QLabel *datasetEchoTime = new QLabel(QString::number(dataset->echoTime));

    ui->gridLayout->addWidget(labelImage,0,0,6,0);
    ui->gridLayout->setColumnMinimumWidth(0,70);
    ui->gridLayout->addWidget(datasetNameLabel,0,1);
    ui->gridLayout->addWidget(datasetName,0,2);
    ui->gridLayout->addWidget(datasetCreationDateLabel,1,1);
    ui->gridLayout->addWidget(datasetCreationDate,1,2);
    ui->gridLayout->addWidget(datasetCommentLabel,2,1);
    ui->gridLayout->addWidget(datasetComment,2,2);
    ui->gridLayout->addWidget(datasetRepetitionTimeLabel,3,1);
    ui->gridLayout->addWidget(datasetRepetitionTime,3,2);
    ui->gridLayout->addWidget(datasetFlipAngleLabel,4,1);
    ui->gridLayout->addWidget(datasetFlipAngle,4,2);
    ui->gridLayout->addWidget(datasetEchoTimeLabel,5,1);
    ui->gridLayout->addWidget(datasetEchoTime,5,2);

}

void QtShanoirDetailsWidget::displayProcessDetails(int idStudy,int idSubject,int idExam,int idDataset,int idProcess)
{
    Process* process = getProcessDetails(idStudy,idSubject,idExam,idDataset,idProcess);

    //Clear GridLayout
    while(ui->gridLayout->count())
    {
        QWidget* widget = ui->gridLayout->itemAt(0)->widget();
        if(widget)
        {
            ui->gridLayout->removeWidget(widget);
            delete widget;
        }
    }

    QImage image;
    image.load(":Images/process.64x64.png");
    QLabel* labelImage = new QLabel();
    labelImage->setPixmap(QPixmap::fromImage(image));

    QLabel *processNameLabel = new QLabel("Name :");
    QLabel *processName = new QLabel(process->name);
    QLabel *processCreationDateLabel = new QLabel("Creation Date :");
    QLabel *processCreationDate = new QLabel(process->creationDate.toString(Qt::ISODate));

    ui->gridLayout->addWidget(labelImage,0,0,2,0);
    ui->gridLayout->setColumnMinimumWidth(0,70);
    ui->gridLayout->addWidget(processNameLabel,0,1);
    ui->gridLayout->addWidget(processName,0,2);
    ui->gridLayout->addWidget(processCreationDateLabel,1,1);
    ui->gridLayout->addWidget(processCreationDate,1,2);

}

void QtShanoirDetailsWidget::displayProcessedDatasetDetails(int idStudy,int idSubject,int idExam,int idDataset,int idProcess, int idProcessedDataset)
{
    ProcessedDataset* process = getProcessedDatasetDetails(idStudy,idSubject,idExam,idDataset,idProcess,idProcessedDataset);

    //Clear GridLayout
    while(ui->gridLayout->count())
    {
        QWidget* widget = ui->gridLayout->itemAt(0)->widget();
        if(widget)
        {
            ui->gridLayout->removeWidget(widget);
            delete widget;
        }
    }

    QImage image;
    image.load(":Images/processedDataset.64x64.png");
    QLabel* labelImage = new QLabel();
    labelImage->setPixmap(QPixmap::fromImage(image));

    QLabel *processedDatasetNameLabel = new QLabel("Name :");
    QLabel *processedDatasetName = new QLabel(process->name);
    QLabel *processedDatasetCommentLabel = new QLabel("Comment :");
    QLabel *processedDatasetComment = new QLabel(process->comment);
    QLabel *processedDatasetCreationDateLabel = new QLabel("Creation Date :");
    QLabel *processedDatasetCreationDate = new QLabel(process->processedDatasetCreationDate.toString(Qt::ISODate));

    ui->gridLayout->addWidget(labelImage,0,0,3,0);
    ui->gridLayout->setColumnMinimumWidth(0,70);
    ui->gridLayout->addWidget(processedDatasetNameLabel,0,1);
    ui->gridLayout->addWidget(processedDatasetName,0,2);
    ui->gridLayout->addWidget(processedDatasetCommentLabel,1,1);
    ui->gridLayout->addWidget(processedDatasetComment,1,2);
    ui->gridLayout->addWidget(processedDatasetCreationDateLabel,2,1);
    ui->gridLayout->addWidget(processedDatasetCreationDate,2,2);

}
