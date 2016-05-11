#include <QtGui>
#include <demo.h>
#include <dao.h>

int main( int argc , char *argv[] )
{
    QApplication app(argc, argv);

    QDir iniDir (QDir::homePath() + QDir::separator() + ".shanoir");
    if (!iniDir.exists())
        QDir(QDir::homePath()).mkdir(".shanoir");
    QString iniFile = iniDir.absolutePath() + QDir::separator() + "properties";

    configureSettings(iniFile);

    Demo * demo = new Demo();
    demo->show();

    return app.exec();
}

