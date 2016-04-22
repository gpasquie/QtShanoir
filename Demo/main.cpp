#include <QtGui>
#include <demo.h>

#ifdef _WIN32
#define GUI_LIB "GUI.dll"
#define DAO_LIB "DAO.dll"
#endif

#ifdef __gnu_linux__
#define GUI_LIB "GUI"
#define DAO_LIB "DAO"
#endif


int main( int argc , char *argv[] )
{
    QApplication app(argc, argv);

    QDir iniDir (QDir::homePath() + QDir::separator() + ".shanoir");
    if (!iniDir.exists())
        QDir(QDir::homePath()).mkdir(".shanoir");
    QString iniFile = iniDir.absolutePath() + QDir::separator() + "properties";

    QLibrary library(DAO_LIB);
    if (!library.load())
        qDebug() << library.errorString();
    else
        qDebug() << "library loaded";
    typedef void (* CallFunction)(QString);
    CallFunction cf = (CallFunction)library.resolve("configureSettings");
    if (cf)
    {
        cf(iniFile);
    }
    else
        qDebug() << "could not call function";

    Demo * demo = new Demo();
    demo->show();

    return app.exec();
}

