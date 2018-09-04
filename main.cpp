#include "widgets/mainwindow.h"

#include <QApplication>
#include <QSettings>
#include <QSurfaceFormat>


#include <iostream>
#include <chrono>
#include <functional>
#include <string>
#include <vector>
int main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName("cadcg");
	QCoreApplication::setApplicationName("MRC Marker");
    QApplication a(argc, argv);

	//QSurfaceFormat format;
	//format.setDepthBufferSize(24);
	//format.setStencilBufferSize(8);
	//format.setVersion(3, 2);
	//format.setProfile(QSurfaceFormat::CoreProfile);
	//QSurfaceFormat::setDefaultFormat(format);

    MainWindow w;
    w.show();
    return a.exec();
}
