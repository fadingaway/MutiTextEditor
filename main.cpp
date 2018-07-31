#include "QApplication"
#include "mainwindow.h"
#include "tabdialog.h"
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName("NotePad");
    QCoreApplication::setOrganizationName("BurnSoft");

    MainWindow w;
    w.show();
    return app.exec();
}
