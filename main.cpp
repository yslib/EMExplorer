#include "widgets/mainwindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName("cadcg");
	QCoreApplication::setApplicationName("MRC Marker");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
