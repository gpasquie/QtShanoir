/*
    The QtShanoir library.
    Copyright (C) 2011  INRIA, Université de Rennes 1

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <QtShanoir.h>
#include <QtShanoirSettings.h>
#include <QtShanoirTreeWidget.h>
#include <QtShanoirProgressWidget.h>
#include <QtShanoirSettingsWidget.h>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslConfiguration>

#include "QtShanoirWebService.h"
#include "QtShanoirUploadWidget.h"

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

QtShanoir * QtShanoir::_instance = 0;

class QtShanoirPrivate
{
public:
    QtShanoirTreeWidget * tree;
    QtShanoirProgressWidget * progress;
    QtShanoirUploadWidget * upload;
    int curId;
    QList<int> selectedIds;
    QString downloadDir;
    QString downloadFileName;
    bool downloadMetadata;
    QList<QString> filesToUploadList;

    QMultiMap<QString, QString> dataToUpload;

    QRegExp studyNameFilter;
    QString dateFilter;
    QRegExp patientNameFilter;
    QRegExp datasetNameFilter;
};

QtShanoir *
QtShanoir::instance()
{
    if ( _instance == 0 )
        _instance = new QtShanoir();
    return _instance;
}

QtShanoir::QtShanoir() :
        d ( new QtShanoirPrivate )
{
    d->tree = 0;
    d->progress = 0;
    d->downloadFileName = "";
    d->downloadMetadata = false;

    d->studyNameFilter.setPattern ( "*" );
    d->patientNameFilter.setPattern ( "*" );
    d->dateFilter = "";
    d->datasetNameFilter.setPattern ( "*" );

    d->studyNameFilter.setPatternSyntax ( QRegExp::Wildcard );
    d->patientNameFilter.setPatternSyntax ( QRegExp::Wildcard );
    d->datasetNameFilter.setPatternSyntax ( QRegExp::Wildcard );
}

void
QtShanoir::setDateFilter ( QString text )
{

    QDate tmpDate = QDate::fromString ( text, "dd/MM/yyyy" );
    d->dateFilter = tmpDate.toString ( Qt::ISODate );

}

void
QtShanoir::setStudyNameFilter ( QString text )
{
    if ( text != "" )
        d->studyNameFilter.setPattern ( text );
    else
        d->studyNameFilter.setPattern ( "*" );
}

void
QtShanoir::setPatientNameFilter ( QString text )
{
    if ( text != "" )
        d->patientNameFilter.setPattern ( text );
    else
        d->patientNameFilter.setPattern ( "*" );
}

void
QtShanoir::setDatasetNameFilter ( QString text )
{
    if ( text != "" )
        d->datasetNameFilter.setPattern ( text );
    else
        d->datasetNameFilter.setPattern ( "*" );
}

void
QtShanoir::setDownloadFilename ( QString filename )
{
    d->downloadFileName = filename;
}

void
QtShanoir::setDownloadMetadata ( int state )
{
    d->downloadMetadata = ( state == Qt::Checked );
}

void
QtShanoir::clearTree()
{
    d->selectedIds.clear();
    if ( d->tree )
        d->tree->treeWidget->clear();
}

void
QtShanoir::init()
{
    if ( d->tree )
        QtShanoirSettings::Instance();
    QObject::connect ( this, SIGNAL ( startDownload() ), this, SLOT ( callDownload() ) );
    QObject::connect ( this, SIGNAL ( startUpload() ), this, SLOT ( upload() ) );
}

void
QtShanoir::attachTreeWidget ( QtShanoirTreeWidget * widget )
{
    d->tree = widget;
    if ( d->tree )
    {
        QObject::connect ( d->tree, SIGNAL ( mrExamQuery ( QString ) ), this, SLOT ( findExam ( QString ) ) );
        QObject::connect ( d->tree, SIGNAL ( datasetQuery ( QString,QString ) ), this, SLOT ( findDataset ( QString,QString ) ) );
        QObject::connect ( d->tree, SIGNAL ( processingQuery ( QString ) ), this, SLOT ( findProcessing ( QString ) ) );
        QObject::connect ( d->tree, SIGNAL ( id ( int ) ), this, SLOT ( currentId ( int ) ) );
        QObject::connect ( d->tree, SIGNAL ( selected ( QMap<int,QString> ) ), this, SLOT ( updateSelected ( QMap<int,QString> ) ) );
        QObject::connect ( d->tree, SIGNAL ( filename ( QString ) ), this, SLOT ( setDownloadFilename ( QString ) ) );
    }
}

void
QtShanoir::attachProgressWidget ( QtShanoirProgressWidget * widget )
{
    d->progress = widget;
    if ( d->progress )
        d->progress->download->hide();

    emit progressHidden();
}

void
QtShanoir::attachUploadWidget ( QtShanoirUploadWidget * widget )
{
    d->upload = widget;
    if ( d->upload && d->tree )
    {
        QObject::connect ( d->tree, SIGNAL ( selected ( QMap<int,QString> ) ), d->upload, SLOT ( updateInputDataset ( QMap<int,QString> ) ) );
        QObject::connect ( d->tree, SIGNAL ( studyMap ( QMap<int,QString> ) ), d->upload, SLOT ( updateStudyComboBox ( QMap<int,QString> ) ) );
        QObject::connect ( this, SIGNAL ( processingMap ( QMap<int, QString> ) ), d->upload, SLOT ( updateProcessingComboBox ( QMap<int, QString> ) ) );
        QObject::connect ( d->upload, SIGNAL ( uploadData ( QMultiMap<QString, QString> ) ), this, SLOT ( receiveUploadData ( QMultiMap<QString, QString> ) ) );
    }
}

QString QtShanoir::getErrorMessage(QString xmlserial)
{
    QDomDocument doc;
    doc.setContent ( xmlserial );

    doc.appendChild ( doc.firstChild().firstChildElement ( "SOAP-ENV:Body" ).firstChild() );
    doc.removeChild ( doc.firstChild() );
    QString errmsg = doc.firstChild().firstChild().firstChild().nodeValue();

    return errmsg;
}

void
QtShanoir::populate()
{
    QtSoapNamespaces &registry = QtSoapNamespaces::instance();
    registry.registerNamespace ( "ns1", "http://impl.webservices.shanoir.org/" );
    registry.registerNamespace ( "ns2", "http://finder.impl.webservices.shanoir.org/" );
    registry.registerNamespace ( "ns3", "http://importer.impl.webservices.shanoir.org/" );

    QString ws = "StudyFinder";
    QString impl ( "http://finder.impl.webservices.shanoir.org/" );

    QString xmlserial;
    bool querySuccess = QtShanoirWebService::Query ( ws, "setUsername", impl, QStringList() << "username", QStringList() << QtShanoirSettings::Instance()->login(), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    querySuccess = QtShanoirWebService::Query ( ws, "setPassword", impl, QStringList() << "dummy", QStringList() << QtShanoirSettings::Instance()->password(), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    querySuccess = QtShanoirWebService::Query ( ws, "setStudyName", impl, QStringList() << "studyName", QStringList() << d->studyNameFilter.pattern(), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    querySuccess = QtShanoirWebService::Query ( ws, "find", impl, QStringList(), QStringList(), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    QString xmlserialError;
    querySuccess = QtShanoirWebService::Query ( ws, "getErrorMessage", impl, QStringList(), QStringList(), xmlserialError);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    if (this->getErrorMessage(xmlserialError) != "")
    {
        emit queryFailed(this->getErrorMessage(xmlserialError));
        return;
    }

    d->tree->parseStudy ( xmlserial, d->patientNameFilter );

    this->getProcessingListId();
}

void
QtShanoir::findExam ( QString str )
{
    QString ws = "ExaminationFinder";
    QString impl = "http://finder.impl.webservices.shanoir.org/";

    QString xmlserial;
    bool querySuccess = QtShanoirWebService::Query ( ws, "setUsername", impl, QStringList() << "username", QStringList() << QtShanoirSettings::Instance()->login(), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    querySuccess = QtShanoirWebService::Query ( ws, "setPassword", impl, QStringList() << "dummy", QStringList() << QtShanoirSettings::Instance()->password(), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    querySuccess = QtShanoirWebService::Query ( ws, "setSubjectIds", impl, QStringList() << "examinationSubjectIds", QStringList() << str, xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    querySuccess = QtShanoirWebService::Query ( ws, "find", impl, QStringList(), QStringList(), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    QString xmlserialError;
    querySuccess = QtShanoirWebService::Query ( ws, "getErrorMessage", impl, QStringList(), QStringList(), xmlserialError);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    if (this->getErrorMessage(xmlserialError) != "")
    {
        emit queryFailed(this->getErrorMessage(xmlserialError));
        return;
    }

    d->tree->parseMrExamination ( xmlserial, d->dateFilter );
}

void
QtShanoir::findProcessing ( QString datasetId )
{
    QString ws = "DatasetProcessingFinder";
    QString impl = "http://finder.impl.webservices.shanoir.org/";

    QString xmlserial;
    bool querySuccess = QtShanoirWebService::Query ( ws, "setUsername", impl, QStringList() << "username", QStringList() << QtShanoirSettings::Instance()->login(), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    querySuccess = QtShanoirWebService::Query ( ws, "setPassword", impl, QStringList() << "dummy", QStringList() << QtShanoirSettings::Instance()->password(), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    querySuccess = QtShanoirWebService::Query ( ws, "setInputDatasetIds", impl, QStringList() << "datasetIds", QStringList() << datasetId, xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    querySuccess = QtShanoirWebService::Query ( ws, "find", impl, QStringList(), QStringList(), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    QString xmlserialError;
    querySuccess = QtShanoirWebService::Query ( ws, "getErrorMessage", impl, QStringList(), QStringList(), xmlserialError);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    if (this->getErrorMessage(xmlserialError) != "")
    {
        emit queryFailed(this->getErrorMessage(xmlserialError));
        return;
    }

    d->tree->parseProcessingData ( xmlserial );
}

void
QtShanoir::findDataset ( QString examId, QString subjectId )
{
    QString ws = "DatasetFinder";
    QString impl = "http://finder.impl.webservices.shanoir.org/";

    QString xmlserial;
    bool querySuccess = QtShanoirWebService::Query ( ws, "setUsername", impl, QStringList() << "username", QStringList() << QtShanoirSettings::Instance()->login(), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    querySuccess = QtShanoirWebService::Query ( ws, "setPassword", impl, QStringList() << "dummy", QStringList() << QtShanoirSettings::Instance()->password(), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    querySuccess = QtShanoirWebService::Query ( ws, "setSubjectIds", impl, QStringList() << "datasetSubjectIds", QStringList() << subjectId, xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    querySuccess = QtShanoirWebService::Query ( ws, "setExaminationIds", impl, QStringList() << "datasetExaminations", QStringList() << examId, xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    querySuccess = QtShanoirWebService::Query ( ws, "find", impl, QStringList(), QStringList(), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    QString xmlserialError;
    querySuccess = QtShanoirWebService::Query ( ws, "getErrorMessage", impl, QStringList(), QStringList(), xmlserialError);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    if (this->getErrorMessage(xmlserialError) != "")
    {
        emit queryFailed(this->getErrorMessage(xmlserialError));
        return;
    }

    d->tree->parseAcquisition ( xmlserial, d->datasetNameFilter );
}

void
QtShanoir::getDatasetFilename ( QString datasetId )
{
    QString ws = "Downloader";
    QString impl = "http://impl.webservices.shanoir.org/";

    d->curId = datasetId.toInt();

    QString xmlserial;
    bool querySuccess = QtShanoirWebService::Query ( ws, "setUsername", impl, QStringList() << "username", QStringList() << QtShanoirSettings::Instance()->login(), xmlserial);
    querySuccess = QtShanoirWebService::Query ( ws, "setPassword", impl, QStringList() << "dummy", QStringList() << QtShanoirSettings::Instance()->password(), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    querySuccess = QtShanoirWebService::Query ( ws, "setDatasetId", impl, QStringList() << "datasetId", QStringList() << datasetId, xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    querySuccess = QtShanoirWebService::Query ( ws, "getFileName", impl, QStringList(), QStringList(), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    QString xmlserialError;
    querySuccess = QtShanoirWebService::Query ( ws, "getErrorMessage", impl, QStringList(), QStringList(), xmlserialError);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    if (this->getErrorMessage(xmlserialError) != "")
    {
        emit queryFailed(this->getErrorMessage(xmlserialError));
        return;
    }

    QDomDocument doc;
    doc.setContent ( xmlserial );
    QDomNode n = doc.firstChild().firstChild().nextSibling().firstChild().firstChild();
    if ( n.isElement() )
    {
        d->downloadFileName = n.toElement().text();
    }
}

void
QtShanoir::getProcessingListId()
{
    QString ws = "ReferenceLister";
    QString impl = "http://impl.webservices.shanoir.org/";

    QString xmlserial;
    bool querySuccess = QtShanoirWebService::Query ( ws, "setUsername", impl, QStringList() << "username", QStringList() << QtShanoirSettings::Instance()->login(), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    querySuccess = QtShanoirWebService::Query ( ws, "setPassword", impl, QStringList() << "dummy", QStringList() << QtShanoirSettings::Instance()->password(), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    querySuccess = QtShanoirWebService::Query ( ws, "listReferenceValue", impl, QStringList() << "arg0", QStringList() << "DatasetProcessing", xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    QString xmlserialError;
    querySuccess = QtShanoirWebService::Query ( ws, "getErrorMessage", impl, QStringList(), QStringList(), xmlserialError);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    if (this->getErrorMessage(xmlserialError) != "")
    {
        emit queryFailed(this->getErrorMessage(xmlserialError));
        return;
    }

    QMap<int, QString> map;
    int id;
    QString label;
    QDomDocument doc;
    doc.setContent ( xmlserial );
    doc.appendChild ( doc.firstChild().firstChildElement ( "SOAP-ENV:Body" ).firstChild() );
    doc.removeChild ( doc.firstChild() );

    QDomNode n = doc.firstChild().firstChild();
    while ( !n.isNull() )
    {
        id = n.firstChildElement ( "id" ).firstChild().toText().nodeValue().toInt();
        label = n.firstChildElement ( "label" ).firstChild().toText().nodeValue();
        map.insert ( id, label );
        n = n.nextSibling();
    }
    emit processingMap ( map );
}

void
QtShanoir::downloadMetadata ( QString datasetId )
{
    QString ws = "DatasetFinder";
    QString impl = "http://finder.impl.webservices.shanoir.org/";

    QString xmlserial;
    bool querySuccess = QtShanoirWebService::Query ( ws, "setUsername", impl, QStringList() << "username", QStringList() << QtShanoirSettings::Instance()->login(), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    querySuccess = QtShanoirWebService::Query ( ws, "setPassword", impl, QStringList() << "dummy", QStringList() << QtShanoirSettings::Instance()->password(), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    querySuccess = QtShanoirWebService::Query ( ws, "setDatasetIds", impl, QStringList() << "datasetIds", QStringList() << datasetId, xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    querySuccess = QtShanoirWebService::Query ( ws, "find", impl, QStringList(), QStringList(), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    QString xmlserialError;
    querySuccess = QtShanoirWebService::Query ( ws, "getErrorMessage", impl, QStringList(), QStringList(), xmlserialError);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    if (this->getErrorMessage(xmlserialError) != "")
    {
        emit queryFailed(this->getErrorMessage(xmlserialError));
        return;
    }

    QDomDocument doc;
    doc.setContent ( xmlserial );

    QString tmpName = d->downloadFileName.replace ( ".nii", ".xml" ).replace ( ".zip", ".xml" );
    tmpName.replace ( QDir::separator(),"_" );

    QFile dFile ( d->downloadDir + QDir::separator() + tmpName );
    dFile.open ( QFile::WriteOnly );
    dFile.write ( doc.toString().toUtf8().data() );
    dFile.close();

}

void
QtShanoir::downloadDataset ( QString datasetId )
{
    this->getDatasetFilename ( datasetId );
    d->curId = datasetId.toInt();

    QString ws = "Downloader";
    QString impl = "http://impl.webservices.shanoir.org/";

    d->curId = datasetId.toInt();

    QString xmlserial;
    bool querySuccess = QtShanoirWebService::Query ( ws, "setUsername", impl, QStringList() << "username", QStringList() << QtShanoirSettings::Instance()->login(), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    querySuccess = QtShanoirWebService::Query ( ws, "setPassword", impl, QStringList() << "dummy", QStringList() << QtShanoirSettings::Instance()->password(), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    querySuccess = QtShanoirWebService::Query ( ws, "setDatasetId", impl, QStringList() << "datasetId", QStringList() << datasetId, xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    QByteArray bin;
    querySuccess = QtShanoirWebService::BinaryQuery ( ws, "download", impl, QStringList(), QStringList(), bin);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    QString xmlserialError;
    querySuccess = QtShanoirWebService::Query ( ws, "getErrorMessage", impl, QStringList(), QStringList(), xmlserialError);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    if (bin.isEmpty())
    {
        emit queryFailed("Empty binary");
        return;
    }

    if (this->getErrorMessage(xmlserialError) != "")
    {
        emit queryFailed(this->getErrorMessage(xmlserialError));
    }

    QString tmpName = d->downloadFileName.isEmpty() ? QString ( "%1.nii.gz" ).arg ( d->curId ) : d->downloadFileName;

    tmpName.replace ( QDir::separator(),"_" );

    QFile dFile ( d->downloadDir + QDir::separator() + tmpName );

    dFile.open ( QFile::WriteOnly );
    dFile.write ( bin );
    dFile.close();

    QString metadataFileName = "";
    if ( d->downloadMetadata )
    {
        this->downloadMetadata ( datasetId );
        metadataFileName = dFile.fileName().replace ( ".nii.gz", ".xml" ).replace ( ".nii", ".xml" ).replace ( ".zip", ".xml" );
    }
    if ( dFile.fileName().contains ( ".zip" ) )
    {
        // Decompression using QuaZIP
        QuaZip zipFile ( dFile.fileName() );
        zipFile.open ( QuaZip::mdUnzip );
        QuaZipFileInfo info;
        QuaZipFile file ( &zipFile );

        for ( bool more = zipFile.goToFirstFile(); more; more = zipFile.goToNextFile() )
        {
            zipFile.getCurrentFileInfo ( &info );
            file.open ( QIODevice::ReadOnly );
            QString name = file.getActualFileName();

            QString outFileName = d->downloadDir + QDir::separator() + name;
            QFile out ( outFileName );
            out.open ( QIODevice::WriteOnly );

            char buf[4096];
            char c;
            int len = 0;
            while ( file.getChar ( &c ) )
            {
                buf[len++] = c;
                if ( len >= 4096 )
                {
                    out.write ( buf, len );
                    len = 0;
                }
            }
            if ( len > 0 )
            {
                out.write ( buf, len );
            }
            out.close();

            if ( name.contains ( ".nii" ) ||name.contains ( ".nii.gz" ) )
                emit downloadFinished ( outFileName, metadataFileName );

            file.close();
        }
        zipFile.close();
    }
    else
        emit downloadFinished ( dFile.fileName(), metadataFileName );
}

void
QtShanoir::currentId ( int id )
{
    d->curId = id;
}

void
QtShanoir::find()
{
    this->clearTree();
    this->populate();
}

void
QtShanoir::downloadToDir ( QString directory )
{
    d->downloadDir = directory;
    emit startDownload();
}

void
QtShanoir::download()
{
    if ( ( d->selectedIds.size() != 0 ) || !d->downloadFileName.isEmpty() )
    {
        // Open a QFileDialog in directory mode.
        QFileDialog * dialog = new QFileDialog ( d->tree );
        dialog->setFileMode ( QFileDialog::Directory );
        dialog->setOption ( QFileDialog::ShowDirsOnly, true );
        dialog->setDirectory ( QDir::home().dirName() );
        dialog->setWindowTitle ( tr ( "Choose datasets download directory" ) );
        QString directory;
        if ( dialog->exec() )
        {
            directory = dialog->selectedFiles() [0];
        }
        dialog->close();

        if ( !directory.isEmpty() ) // A file has been chosen
        {
            d->downloadDir = directory;
            emit startDownload();
        }
        delete dialog;
    }
}

void
QtShanoir::callDownload()
{
    qDebug() << "Start Download";
    if ( d->progress )
    {
        d->progress->download->setValue ( 0 );
        d->progress->download->show();
        emit progressShown();
    }
    for ( int i = 0; i < d->selectedIds.size(); i++ )
    {
        d->curId = d->selectedIds.at ( i );
        this->downloadDataset ( QString::number ( d->curId ) );
        if ( d->progress )
            d->progress->download->setValue ( ( int ) ( 100.00 * ( i + 1 ) / d->selectedIds.size() ) );
    }
    if ( d->progress )
    {
        d->progress->download->hide();
        emit progressHidden();
    }

    qDebug() << "Download finished";
}

void
QtShanoir::upload()
{
    QString ws = "DatasetImporter";
    QString impl = "http://importer.impl.webservices.shanoir.org/";

    QString xmlserial;
    bool querySuccess = QtShanoirWebService::Query ( ws, "setUsername", impl, QStringList() << "username", QStringList() << QtShanoirSettings::Instance()->login(), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    querySuccess = QtShanoirWebService::Query ( ws, "setPassword", impl, QStringList() << "dummy", QStringList() << QtShanoirSettings::Instance()->password(), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    querySuccess = QtShanoirWebService::Query ( ws, "setDatasetName", impl, QStringList() << "datasetName", QStringList() << d->dataToUpload.value ( "datasetName" ), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    querySuccess = QtShanoirWebService::Query ( ws, "setDatasetComment", impl, QStringList() << "datasetComment", QStringList() << d->dataToUpload.value ( "datasetComment" ), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    querySuccess = QtShanoirWebService::Query ( ws, "setRefDatasetProcessingId", impl, QStringList() << "refDatasetProcessingId", QStringList() << d->dataToUpload.value ( "processingId" ), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    querySuccess = QtShanoirWebService::Query ( ws, "setProcessingComment", impl, QStringList() << "processingComment", QStringList() << d->dataToUpload.value ( "processingComment" ), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    querySuccess = QtShanoirWebService::Query ( ws, "setDatasetClass", impl, QStringList() << "datasetClass", QStringList() << d->dataToUpload.value ( "datasetType" ), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    querySuccess = QtShanoirWebService::Query ( ws, "setStudyId", impl, QStringList() << "studyId", QStringList() << d->dataToUpload.value ( "studyId" ), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    QStringList argNames;

    for ( int i = 0;i < d->dataToUpload.values ( "inputDatasets" ).size();++i )
        argNames << "inputDatasetIdList";

    querySuccess = QtShanoirWebService::Query ( ws, "setInputDatasetIdList", impl, argNames, QStringList() << d->dataToUpload.values ( "inputDatasets" ), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    QFile file ( d->dataToUpload.value ( "datasetPath" ) );
    file.open ( QIODevice::ReadOnly );
    querySuccess = QtShanoirWebService::Query ( ws, "uploadFile", impl, QStringList() << "dataHandler" << "filename", QStringList() << file.readAll().toBase64() << QDir ( d->dataToUpload.value ( "datasetPath" ) ).dirName(),xmlserial);
    file.close();

    querySuccess = QtShanoirWebService::Query ( ws, "importDataset", impl, QStringList(), QStringList(), xmlserial);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    QString xmlserialError;
    querySuccess = QtShanoirWebService::Query ( ws, "getErrorMessage", impl, QStringList(), QStringList(), xmlserialError);
    if (!querySuccess)
    {
        emit queryFailed(xmlserial);
        return;
    }

    if (this->getErrorMessage(xmlserialError) != "")
    {
        emit queryFailed(this->getErrorMessage(xmlserialError));
    }
}

void
QtShanoir::callUpload()
{
    qDebug() << "Start upload from qtshanoir";
}

void
QtShanoir::receiveUploadData ( QMultiMap<QString, QString> mmap )
{
    d->dataToUpload = mmap;
    emit startUpload();
}

void
QtShanoir::updateSelected ( QMap<int, QString> listId )
{
    d->selectedIds = listId.keys();
}

void
QtShanoir::queryFinished()
{
    if ( d->progress )
        d->progress->download->setValue ( 0 );
}

void
QtShanoir::downloadProgress ( qint64 bytesReceived, qint64 bytesTotal )
{
    d->progress->download->setValue ( 100 * bytesReceived / bytesTotal );
    qApp->processEvents();
}

