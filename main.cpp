#include <QApplication>
#include "mainwindow.h"
#include <QDebug>
#include <QTranslator>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(pcm);

    QApplication app(argc, argv);
    QTranslator myTranslator;
    myTranslator.load("pcm_" + QLocale::system().name());
    qDebug() << QLocale::system().name();
    app.installTranslator(&myTranslator);
    MainWindow mainWin;
    mainWin.show();
    return app.exec();
}
