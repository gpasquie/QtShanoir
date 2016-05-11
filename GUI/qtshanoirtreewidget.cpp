#include "qtshanoirtreewidget.h"
#include "qtshanoirmain.h"

#include "dao.h"

QtShanoirTreeWidget::QtShanoirTreeWidget(QWidget *parent) : QWidget(parent),ui (new Ui::QtShanoirTree)
{
    ui->setupUi(this);
    ui->treeWidget->setColumnWidth(0,350);
    ui->treeWidget->setColumnWidth(1,50);
}

void QtShanoirTreeWidget::initConnections()
{
    QObject::connect(ui->treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this, SLOT(itemClicked(QTreeWidgetItem*,int)));
    QObject::connect(ui->treeWidget,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this, SLOT(updateCheckBoxes(QTreeWidgetItem*,int)));
}

void QtShanoirTreeWidget::itemClicked(QTreeWidgetItem* item,int column)
{
    QObject::disconnect(ui->treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this, SLOT(itemClicked(QTreeWidgetItem*,int)));
    extend(item,column);
    qDebug()<<"size"<<selectedIds.size();
    QObject::connect(ui->treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this, SLOT(itemClicked(QTreeWidgetItem*,int)));
}


void QtShanoirTreeWidget::clearTree()
{
    ui->treeWidget->clear();
}

void QtShanoirTreeWidget::buildTree()
{
    clearTree();
    tree = ui->treeWidget->invisibleRootItem();
    QTreeWidgetItem* root = new QTreeWidgetItem(QTreeWidgetItem::UserType + 1);
    root->setData(0, QTreeWidgetItem::UserType + 1, 0);
    tree->addChild(root);
    root->setText(0, "Shanoir database"); // a changer avec le nom du host
    root->setText(1, QString("SERVER"));
    root->setIcon(0, QIcon(":Images/network.64x64.png"));
    root->setExpanded(true);
    initConnections();
}

//Cette fonction est relative au bouton "find"
void QtShanoirTreeWidget::build(QString studyFilter,QString subjectFilter,QDate examinationDateFilter,QString datasetFilter)
{
    QList<QTreeWidgetItem *> root;
    QList<QTreeWidgetItem *> node;
    buildTree();
    QDate defaultDate (1752,9,14);

    if((studyFilter!="")&&(subjectFilter=="")&&(examinationDateFilter==defaultDate)&&(datasetFilter==""))
    {
        root = ui->treeWidget->findItems("Shanoir database",Qt::MatchExactly); //inconvenient des noms similaires
        for(int i=0; i<root.size();i++)
        {
            developTree(studyFilter,root[i]);
            break;
        }
    }
    else if((studyFilter!="")&&(subjectFilter!="")&&(examinationDateFilter==defaultDate)&&(datasetFilter==""))
    {
        //FindStudy
        root = ui->treeWidget->findItems("Shanoir database",Qt::MatchExactly); //inconvenient des noms similaires
        for(int i=0; i<root.size();i++)
        {
            developTree(studyFilter,root[i]);
            break;
        }

        //FindSubject
        node = root [0]->takeChildren();
        qDebug()<<"size"<<node.size();
        for(int j=0; j<node.size();j++)
        {
            if(node[j]->text(0)==studyFilter)
            {
                QTreeWidgetItem* nodef = new QTreeWidgetItem(QTreeWidgetItem::UserType + 2);
                qDebug()<<"address"<<&node[j];
                nodef->setData(0, QTreeWidgetItem::UserType + 2, 4);
                nodef->setText(0,"toto");
                nodef->setIcon(0, QIcon(":Images/subject.64x64.png"));
                nodef->setText(1, "SUBJECT");
                nodef->setCheckState(0, Qt::Unchecked);
                qDebug()<<"size"<<node.size();
                node[j]->addChild(nodef);

                ////                developStudy(studyFilter,subjectFilter,node[i]);
                ////                qDebug() << "Children count i"<<node[i]->childCount();
                break;
            }
        }

    }

}

void QtShanoirTreeWidget::developTree(QString studyFilter, QTreeWidgetItem* item)
{
    QMap<int,QString> list = findStudyList(studyFilter);
    if (!list.isEmpty())
    {
        for (int i=0; i<list.size();i++)
        {
            qDebug()<<list.values().at(i);
            QTreeWidgetItem * node = new QTreeWidgetItem(QTreeWidgetItem::UserType + 2);
            node->setData(0, QTreeWidgetItem::UserType + 2, list.keys().at(i));
            node->setText(0, list.values().at(i));
            node->setText(1, "STUDY");
            node->setIcon(0, QIcon(":Images/study.64x64.png"));
            item->addChild(node);
        }
        item->setExpanded(true);
    }
    else if (studyFilter != "")
    {
        emit queryFailed("Study does not exist");
        qDebug() << "Study does not exist"; // affichage
    }
}

void QtShanoirTreeWidget::developStudy(QString study,QString subjectFilter,QTreeWidgetItem* item)
{
    QMap<int,QString> list = findSubjectList(study,subjectFilter);
    if (!list.isEmpty())
    {
        for (int i=0; i<list.size();i++)
        {
            qDebug()<<list.values().at(i);
            QTreeWidgetItem* node = new QTreeWidgetItem(QTreeWidgetItem::UserType + 3);
            node->setData(0, QTreeWidgetItem::UserType + 3, list.keys().at(i));
            node->setText(0,list.values().at(i));
            node->setIcon(0, QIcon(":Images/subject.64x64.png"));
            node->setText(1, "SUBJECT");
            node->setCheckState(0, Qt::Unchecked);
            item->addChild(node);
            qDebug() << "Children count"<<item->childCount(); // affichage

        }
        item->setExpanded(true);
        //emit getStudyDetails(study);
    }
}

void QtShanoirTreeWidget::developSubject(QTreeWidgetItem* item)
{
    QTreeWidgetItem* parent = item->parent();

    QMap<int,QString> list = findExamList(parent->data(0, QTreeWidgetItem::UserType + 2).toInt(), item->data(0, QTreeWidgetItem::UserType + 3).toInt());
    if (!list.isEmpty())
    {
        foreach(QTreeWidgetItem *item, ui->treeWidget->selectedItems())
        {
            qDebug() <<"item"<< item->text(0);
            //str is what you want
        }
        // QTreeWidgetItem* sub = ui->treeWidget->selectedItems().front();
        for (int i=0; i<list.size();i++)
        {
            qDebug()<<list.values().at(i);
            QTreeWidgetItem* node = new QTreeWidgetItem(QTreeWidgetItem::UserType + 4);
            node->setData(0, QTreeWidgetItem::UserType + 4, list.keys().at(i));
            node->setText(0,list.values().at(i));
            node->setIcon(0, QIcon(":Images/examination.64x64.png"));
            node->setText(1, "EXAM");
            node->setCheckState(0, Qt::Unchecked);
            item->addChild(node);
        }
        item->setExpanded(true);
    }


}

void QtShanoirTreeWidget::developExam(QTreeWidgetItem* item)
{
    QTreeWidgetItem* parentSubject = item->parent();
    QTreeWidgetItem* parentStudy = parentSubject->parent();

    QMap<int,QString> list = findDatasetList(parentStudy->data(0, QTreeWidgetItem::UserType + 2).toInt(),parentSubject->data(0, QTreeWidgetItem::UserType + 3).toInt(),item->data(0, QTreeWidgetItem::UserType + 4).toInt());
    if (!list.isEmpty())
    {
        foreach(QTreeWidgetItem *item, ui->treeWidget->selectedItems())
        {
            qDebug() <<"item"<< item->text(0);
            //str is what you want
        }
        //QTreeWidgetItem* sub = ui->treeWidget->selectedItems().front(); // la faute est ici
        for (int i=0; i<list.size();i++)
        {
            qDebug()<<list.values().at(i);
            QTreeWidgetItem* node = new QTreeWidgetItem(QTreeWidgetItem::UserType + 5);
            node->setData(0, QTreeWidgetItem::UserType + 5, list.keys().at(i));
            node->setText(0,list.values().at(i));
            node->setIcon(0, QIcon(":Images/dataset.64x64.png"));
            node->setText(1, "DATASET");
            node->setCheckState(0, Qt::Unchecked);
            item->addChild(node);
        }
        item->setExpanded(true);
    }

}

void QtShanoirTreeWidget::developDataset(QTreeWidgetItem* item)
{
    QTreeWidgetItem* parentExam = item->parent();
    QTreeWidgetItem* parentSubject = parentExam->parent();
    QTreeWidgetItem* parentStudy = parentSubject->parent();


    QMap<int,QString> list = findProcessList(parentStudy->data(0, QTreeWidgetItem::UserType + 2).toInt(), parentSubject->data(0, QTreeWidgetItem::UserType + 3).toInt(),parentExam->data(0, QTreeWidgetItem::UserType + 4).toInt(),item->data(0, QTreeWidgetItem::UserType + 5).toInt());
    if (!list.isEmpty())
    {
        foreach(QTreeWidgetItem *item, ui->treeWidget->selectedItems())
        {
            qDebug() <<"item"<< item->text(0);
            //str is what you want
        }
        for (int i=0; i<list.size();i++)
        {
            qDebug()<<list.values().at(i);
            QTreeWidgetItem* node = new QTreeWidgetItem(QTreeWidgetItem::UserType + 6);
            node->setData(0, QTreeWidgetItem::UserType + 6, list.keys().at(i));
            node->setText(0,list.values().at(i));
            node->setIcon(0, QIcon(":Images/process.64x64.png"));
            node->setText(1, "PROCESS");
            item->addChild(node);
        }
        item->setExpanded(true);
    }

}

void QtShanoirTreeWidget::developProcess(QTreeWidgetItem* item)
{
    QTreeWidgetItem* parentDataset = item->parent();
    QTreeWidgetItem* parentExam = parentDataset->parent();
    QTreeWidgetItem* parentSubject = parentExam->parent();
    QTreeWidgetItem* parentStudy = parentSubject->parent();


    QMap<int,QString> list = findProcessedDatasetList(parentStudy->data(0, QTreeWidgetItem::UserType + 2).toInt(), parentSubject->data(0, QTreeWidgetItem::UserType + 3).toInt(),parentExam->data(0, QTreeWidgetItem::UserType + 4).toInt(),parentDataset->data(0, QTreeWidgetItem::UserType + 5).toInt(),item->data(0, QTreeWidgetItem::UserType + 6).toInt());
    if (!list.isEmpty())
    {
        foreach(QTreeWidgetItem *item, ui->treeWidget->selectedItems())
        {
            qDebug() <<"item"<< item->text(0);
            //str is what you want
        }
        for (int i=0; i<list.size();i++)
        {
            qDebug()<<list.values().at(i);
            QTreeWidgetItem* node = new QTreeWidgetItem(QTreeWidgetItem::UserType + 7);
            node->setData(0, QTreeWidgetItem::UserType + 7, list.keys().at(i));
            node->setText(0,list.values().at(i));
            node->setIcon(0, QIcon(":Images/processedDataset.64x64.png"));
            node->setText(1, "PROCESSED DATASET");
            node->setCheckState(0, Qt::Unchecked);
            item->addChild(node);
        }
        item->setExpanded(true);
    }

}

void QtShanoirTreeWidget::updateCheckBoxes(QTreeWidgetItem * item,int column)
{
    if (item->checkState(0) == Qt::Checked)
    {
        if ((item->type() == QTreeWidgetItem::UserType + 5) && !selectedIds.contains(item->data(0, QTreeWidgetItem::UserType + 5).toInt()))
        {
            selectedIds.insert(item->data(0, QTreeWidgetItem::UserType + 5).toInt(), item->text(0));
        }
        //        if ((item->type() == QTreeWidgetItem::UserType + 7) && !selectedIds.contains(item->data(0, QTreeWidgetItem::UserType + 7).toInt()))
        //        {
        //            qDebug()<<" +7";
        //            selectedIds.insert(item->data(0, QTreeWidgetItem::UserType + 7).toInt(), item->text(0));
        //        }
        if ((item->type() != QTreeWidgetItem::UserType + 5))//&&(item->type() != QTreeWidgetItem::UserType + 7))
        {
            if (item->childCount()==0)
            {
                extend(item, column);
                updateCheckBoxes(item,column);

            }

            for (int i = 0; i < item->childCount(); i++)
            {
                item->child(i)->setCheckState(0, Qt::Checked);
                if ((item->child(i)->type() == QTreeWidgetItem::UserType + 5) && !selectedIds.contains(item->child(i)->data(0, QTreeWidgetItem::UserType + 5).toInt()))
                    selectedIds.insert(item->child(i)->data(0, QTreeWidgetItem::UserType + 5).toInt(), item->child(i)->text(0));
                for (int j = 0; j < item->child(i)->childCount(); j++)
                {
                    item->child(i)->child(j)->setCheckState(0, Qt::Checked);
                    if ((item->child(i)->child(j)->type() == QTreeWidgetItem::UserType + 5) && !selectedIds.contains(item->child(i)->child(j)->data(0, QTreeWidgetItem::UserType + 7).toInt()))
                        selectedIds.insert(item->child(i)->child(j)->data(0, QTreeWidgetItem::UserType + 5).toInt(),item->child(i)->child(j)->text(0));
                }
            }
        }
    }
    else
    {
        if (item->type() == QTreeWidgetItem::UserType + 3)
        {
            for (int i = 0; i < item->childCount(); i++)
            {
                item->child(i)->setCheckState(0, Qt::Unchecked);
                for (int j = 0; j < item->child(i)->childCount(); j++)
                {
                    selectedIds.remove(item->child(i)->child(j)->data(0, QTreeWidgetItem::UserType + 5).toInt());
                    item->child(i)->child(j)->setCheckState(0, Qt::Unchecked);
                }
            }
        }
        if (item->type() == QTreeWidgetItem::UserType + 4)
        {
            for (int i = 0; i < item->childCount(); i++)
            {
                selectedIds.remove(item->child(i)->data(0, QTreeWidgetItem::UserType + 5).toInt());
                item->child(i)->setCheckState(0, Qt::Unchecked);
            }
            item->parent()->setCheckState(0, Qt::Unchecked);
        }

        if (item->type() == QTreeWidgetItem::UserType + 5)
        {
            for (int i = 0; i < item->childCount(); i++)
            {
                selectedIds.remove(item->child(i)->data(0, QTreeWidgetItem::UserType + 6).toInt());
                item->child(i)->setCheckState(0, Qt::Unchecked);
            }
            item->parent()->setCheckState(0, Qt::Unchecked);
        }

    }
    emit callUpdateSelectedIds (selectedIds);
}

void QtShanoirTreeWidget::extend(QTreeWidgetItem* item,int)
{
    if (!item->data(0, QTreeWidgetItem::UserType + 1).isNull())
    {
        qDebug()<<"adresse"<<&item;
        qDebug()<<"Server"<<item->data(0, QTreeWidgetItem::UserType + 2).toString();
        if (item->childCount() == 0)
            developTree("",item);
    }
    if (!item->data(0, QTreeWidgetItem::UserType + 2).isNull())
    {
        qDebug()<<"adresse"<<&item;
        qDebug()<<"study"<<item->data(0, QTreeWidgetItem::UserType + 2).toString();
        if (item->childCount() == 0)
            developStudy(item->text(0),"",item);
        emit getStudyDetails(item->text(0));
    }
    if (!item->data(0, QTreeWidgetItem::UserType + 3).isNull())
    {
        qDebug()<<"adresse"<<&item;
        QTreeWidgetItem* parent = item->parent();

        if ( item->childCount() == 0)
            developSubject(item);
        emit getSubjectDetails(parent->data(0, QTreeWidgetItem::UserType + 2).toInt(),item->data(0, QTreeWidgetItem::UserType + 3).toInt());
    }
    if (!item->data(0, QTreeWidgetItem::UserType + 4).isNull())
    {
        QTreeWidgetItem* parentSubject = item->parent();
        QTreeWidgetItem* parentStudy = parentSubject->parent();
        if ( item->childCount() == 0)
            developExam(item);
        emit getExamDetails(parentStudy->data(0, QTreeWidgetItem::UserType + 2).toInt(),parentSubject->data(0, QTreeWidgetItem::UserType + 3).toInt(),item->data(0, QTreeWidgetItem::UserType + 4).toInt());
    }
    if (!item->data(0, QTreeWidgetItem::UserType + 5).isNull())
    {
        QTreeWidgetItem* parentExam = item->parent();
        QTreeWidgetItem* parentSubject = parentExam->parent();
        QTreeWidgetItem* parentStudy = parentSubject->parent();
        if ( item->childCount() == 0)
            developDataset(item);
        emit getDatasetDetails(parentStudy->data(0, QTreeWidgetItem::UserType + 2).toInt(),parentSubject->data(0, QTreeWidgetItem::UserType + 3).toInt(),parentExam->data(0, QTreeWidgetItem::UserType + 4).toInt(),item->data(0, QTreeWidgetItem::UserType + 5).toInt());
    }
    if (!item->data(0, QTreeWidgetItem::UserType + 6).isNull())
    {
        QTreeWidgetItem* parentDataset = item->parent();
        QTreeWidgetItem* parentExam = parentDataset->parent();
        QTreeWidgetItem* parentSubject = parentExam->parent();
        QTreeWidgetItem* parentStudy = parentSubject->parent();
        if ( item->childCount() == 0)
            developProcess(item);
        emit getProcessDetails(parentStudy->data(0, QTreeWidgetItem::UserType + 2).toInt(),parentSubject->data(0, QTreeWidgetItem::UserType + 3).toInt(),parentExam->data(0, QTreeWidgetItem::UserType + 4).toInt(),parentDataset->data(0, QTreeWidgetItem::UserType + 5).toInt(),item->data(0, QTreeWidgetItem::UserType + 6).toInt());
    }
    if (!item->data(0, QTreeWidgetItem::UserType + 7).isNull())
    {
        QTreeWidgetItem* parentProcess = item->parent();
        QTreeWidgetItem* parentDataset = parentProcess->parent();
        QTreeWidgetItem* parentExam = parentDataset->parent();
        QTreeWidgetItem* parentSubject = parentExam->parent();
        QTreeWidgetItem* parentStudy = parentSubject->parent();
        emit getProcessedDatasetDetails(parentStudy->data(0, QTreeWidgetItem::UserType + 2).toInt(),parentSubject->data(0, QTreeWidgetItem::UserType + 3).toInt(),parentExam->data(0, QTreeWidgetItem::UserType + 4).toInt(),parentDataset->data(0, QTreeWidgetItem::UserType + 5).toInt(),parentProcess->data(0, QTreeWidgetItem::UserType + 6).toInt(),item->data(0, QTreeWidgetItem::UserType + 7).toInt());
    }
}
