#include "mainwindow.h"

#include <QApplication>
#include <QSettings>

int main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName("cadcg");
	QCoreApplication::setApplicationName("MRC Marker");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
