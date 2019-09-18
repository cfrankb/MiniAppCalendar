//#include <QtGui/QApplication>
#include <QtWidgets/QApplication>
#include <QTranslator>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    if (!translator.load(":/translations/translation_fr")) {
        printf("cannot find translation\n");
    }
    a.installTranslator(&translator);

    MainWindow w;
    w.setWindowIcon(QIcon(":/images/icon.gif"));
    if (argc > 1) {
        w.setDocument(argv[1]);
    }

    w.show();
    return a.exec();
}
