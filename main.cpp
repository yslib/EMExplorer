#include "mainwindow.h"

#include <QApplication>
#include <QSettings>
#include <QSurfaceFormat>

int main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName("cadcg");
	QCoreApplication::setApplicationName("MRC Marker");
    QApplication a(argc, argv);

	QSurfaceFormat format;
	format.setDepthBufferSize(24);
	format.setStencilBufferSize(8);
	format.setVersion(3, 2);
	format.setProfile(QSurfaceFormat::CoreProfile);
	QSurfaceFormat::setDefaultFormat(format);

    MainWindow w;
    w.show();
    return a.exec();
}
